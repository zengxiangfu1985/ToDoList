#include "task_repository.h"
#include "task_archive.h"
#include "schema_migrator.h"
#include "../utils/app_logger.h"

#include <QHash>
#include <QSet>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

class TaskRepository::Private
{
public:
    QSqlDatabase db;
    QString connectionName;
};

static QString behaviorEventTypeName(BehaviorEventType type)
{
    switch (type) {
    case BehaviorEventType::QuadrantChanged: return QStringLiteral("quadrant_changed");
    case BehaviorEventType::TaskCompleted: return QStringLiteral("task_completed");
    case BehaviorEventType::AiRecommendationAccepted: return QStringLiteral("ai_accepted");
    case BehaviorEventType::FocusStarted: return QStringLiteral("focus_started");
    case BehaviorEventType::FocusCompleted: return QStringLiteral("focus_completed");
    case BehaviorEventType::FocusAbandoned: return QStringLiteral("focus_abandoned");
    }
    return QStringLiteral("unknown");
}

TaskRepository::TaskRepository(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    d->connectionName = QStringLiteral("todolist_conn_%1").arg(reinterpret_cast<quintptr>(this));
}

TaskRepository::~TaskRepository()
{
    if (d->db.isOpen())
        d->db.close();
    QSqlDatabase::removeDatabase(d->connectionName);
    delete d;
}

bool TaskRepository::open(const QString &dbPath, QString *errorMsg)
{
    if (d->db.isOpen())
        d->db.close();

    d->db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), d->connectionName);
    d->db.setDatabaseName(dbPath);
    if (!d->db.open()) {
        if (errorMsg)
            *errorMsg = d->db.lastError().text();
        AppLogger::error("DB", QStringLiteral("打开数据库失败: %1 path=%2").arg(d->db.lastError().text(), dbPath));
        return false;
    }
    AppLogger::info("DB", QStringLiteral("数据库已打开: %1").arg(dbPath));
    if (!ensureSchema(errorMsg))
        return false;
    if (!SchemaMigrator::migrate(d->db, SchemaMigrator::kCurrentSchemaVersion, errorMsg))
        return false;
    return true;
}

bool TaskRepository::isOpen() const
{
    return d->db.isOpen();
}

bool TaskRepository::ensureSchema(QString *errorMsg)
{
    QSqlQuery q(d->db);
    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS tasks ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "title TEXT NOT NULL,"
            "due_at TEXT,"
            "quadrant INTEGER NOT NULL DEFAULT 2,"
            "completed INTEGER NOT NULL DEFAULT 0,"
            "rule_score REAL NOT NULL DEFAULT 0,"
            "ai_rank INTEGER NOT NULL DEFAULT 0,"
            "ai_reason TEXT,"
            "external_source TEXT,"
            "external_id TEXT,"
            "created_at TEXT NOT NULL,"
            "updated_at TEXT NOT NULL"
            ")"))) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        return false;
    }

    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS behavior_events ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "event_type TEXT NOT NULL,"
            "task_id INTEGER,"
            "quadrant INTEGER,"
            "hour_of_day INTEGER,"
            "created_at TEXT NOT NULL"
            ")"))) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        return false;
    }

    return true;
}

static TaskItem taskFromQuery(const QSqlQuery &q)
{
    TaskItem t;
    t.id = q.value(QStringLiteral("id")).toLongLong();
    t.title = q.value(QStringLiteral("title")).toString();
    const auto due = q.value(QStringLiteral("due_at")).toString();
    if (!due.isEmpty())
        t.dueAt = QDateTime::fromString(due, Qt::ISODate);
    t.quadrant = static_cast<EisenhowerQuadrant>(q.value(QStringLiteral("quadrant")).toInt());
    t.completed = q.value(QStringLiteral("completed")).toInt() != 0;
    const auto completedAt = q.value(QStringLiteral("completed_at")).toString();
    if (!completedAt.isEmpty())
        t.completedAt = QDateTime::fromString(completedAt, Qt::ISODate);
    else if (t.completed)
        t.completedAt = QDateTime::fromString(q.value(QStringLiteral("updated_at")).toString(), Qt::ISODate);
    t.ruleScore = q.value(QStringLiteral("rule_score")).toDouble();
    t.aiRank = q.value(QStringLiteral("ai_rank")).toInt();
    t.aiReason = q.value(QStringLiteral("ai_reason")).toString();
    t.externalSource = q.value(QStringLiteral("external_source")).toString();
    t.externalId = q.value(QStringLiteral("external_id")).toString();
    t.createdAt = QDateTime::fromString(q.value(QStringLiteral("created_at")).toString(), Qt::ISODate);
    t.updatedAt = QDateTime::fromString(q.value(QStringLiteral("updated_at")).toString(), Qt::ISODate);
    t.archived = q.value(QStringLiteral("archived")).toInt() != 0;
    const auto archivedAt = q.value(QStringLiteral("archived_at")).toString();
    if (!archivedAt.isEmpty())
        t.archivedAt = QDateTime::fromString(archivedAt, Qt::ISODate);
    return t;
}

QVector<TaskItem> TaskRepository::allTasks() const
{
    QVector<TaskItem> items;
    QSqlQuery q(d->db);
    if (!q.exec(QStringLiteral("SELECT * FROM tasks ORDER BY id DESC")))
        return items;

    while (q.next())
        items.append(taskFromQuery(q));
    return items;
}

QVector<TaskItem> TaskRepository::activeTasks() const
{
    QVector<TaskItem> items;
    for (const TaskItem &t : allTasks()) {
        if (!t.archived)
            items.append(t);
    }
    return items;
}

int TaskRepository::archiveOverdueTasks(QString *errorMsg)
{
    const QDateTime nowLocal = QDateTime::currentDateTime();
    QVector<TaskItem> toArchive;
    for (const TaskItem &t : allTasks()) {
        if (t.archived || !t.dueAt.isValid())
            continue;
        if (t.dueAt.toLocalTime() < nowLocal)
            toArchive.append(t);
    }
    if (toArchive.isEmpty())
        return 0;

    QHash<QDate, QVector<TaskItem>> byDueDate;
    const QDateTime nowUtc = QDateTime::currentDateTimeUtc();
    for (const TaskItem &t : toArchive) {
        QSqlQuery q(d->db);
        q.prepare(QStringLiteral("UPDATE tasks SET archived=1, archived_at=?, updated_at=? WHERE id=?"));
        q.addBindValue(nowUtc.toString(Qt::ISODate));
        q.addBindValue(nowUtc.toString(Qt::ISODate));
        q.addBindValue(t.id);
        if (!q.exec()) {
            if (errorMsg)
                *errorMsg = q.lastError().text();
            AppLogger::error("DB", QStringLiteral("归档过期任务失败 id=%1").arg(t.id));
            continue;
        }
        const QDate dueDate = t.dueAt.toLocalTime().date();
        byDueDate[dueDate].append(t);
    }

    for (auto it = byDueDate.constBegin(); it != byDueDate.constEnd(); ++it) {
        QString fileErr;
        TaskArchive::appendExpiredTasks(it.value(), it.key(), &fileErr);
        if (!fileErr.isEmpty())
            AppLogger::warn("DB", QStringLiteral("写入过期任务历史失败: %1").arg(fileErr));
    }

    AppLogger::info("DB", QStringLiteral("已归档 %1 条过期任务").arg(toArchive.size()));
    emit tasksChanged();
    return toArchive.size();
}

QVector<TaskItem> TaskRepository::yesterdayUnfinishedTasks() const
{
    const QDate yesterday = QDate::currentDate().addDays(-1);
    QVector<TaskItem> result;

    QString err;
    const QVector<TaskItem> history = TaskArchive::loadHistoryForDate(yesterday, &err);
    if (!history.isEmpty()) {
        for (const TaskItem &t : history) {
            if (!t.completed)
                result.append(t);
        }
        return result;
    }

    for (const TaskItem &t : activeTasks()) {
        if (t.completed)
            continue;
        if (t.dueAt.isValid() && t.dueAt.toLocalTime().date() <= yesterday)
            result.append(t);
    }
    return result;
}

QVector<TaskItem> TaskRepository::activeTasksDueOnDate(const QDate &localDate) const
{
    QVector<TaskItem> out;
    if (!localDate.isValid())
        return out;

    for (const TaskItem &t : activeTasks()) {
        if (t.dueAt.isValid() && t.dueAt.toLocalTime().date() == localDate)
            out.append(t);
    }
    return out;
}

TaskItem TaskRepository::findByExternal(const QString &source, const QString &externalId) const
{
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral("SELECT * FROM tasks WHERE external_source=? AND external_id=?"));
    q.addBindValue(source);
    q.addBindValue(externalId);
    if (q.exec() && q.next())
        return taskFromQuery(q);
    return {};
}

bool TaskRepository::addTask(TaskItem *task, QString *errorMsg)
{
    if (!task || task->title.trimmed().isEmpty()) {
        if (errorMsg)
            *errorMsg = QStringLiteral("标题不能为空");
        return false;
    }

    const QDateTime now = QDateTime::currentDateTimeUtc();
    task->createdAt = now;
    task->updatedAt = now;

    QSqlQuery q(d->db);
    q.prepare(QStringLiteral(
        "INSERT INTO tasks (title, due_at, quadrant, completed, rule_score, ai_rank, ai_reason, "
        "external_source, external_id, created_at, updated_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));
    q.addBindValue(task->title.trimmed());
    q.addBindValue(task->dueAt.isValid() ? task->dueAt.toUTC().toString(Qt::ISODate) : QVariant());
    q.addBindValue(static_cast<int>(task->quadrant));
    q.addBindValue(task->completed ? 1 : 0);
    q.addBindValue(task->ruleScore);
    q.addBindValue(task->aiRank);
    q.addBindValue(task->aiReason);
    q.addBindValue(task->externalSource.isEmpty() ? QVariant() : task->externalSource);
    q.addBindValue(task->externalId.isEmpty() ? QVariant() : task->externalId);
    q.addBindValue(task->createdAt.toString(Qt::ISODate));
    q.addBindValue(task->updatedAt.toString(Qt::ISODate));

    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        AppLogger::error("DB", QStringLiteral("添加任务失败: %1").arg(q.lastError().text()));
        return false;
    }

    task->id = q.lastInsertId().toLongLong();
    AppLogger::info("DB",
                    QStringLiteral("添加任务 id=%1 title=%2 Q%3 due=%4")
                        .arg(task->id)
                        .arg(task->title)
                        .arg(static_cast<int>(task->quadrant))
                        .arg(task->dueAt.isValid() ? task->dueAt.toString(Qt::ISODate) : QStringLiteral("(无)")));
    emit tasksChanged();
    return true;
}

bool TaskRepository::updateTask(const TaskItem &task, QString *errorMsg)
{
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral(
        "UPDATE tasks SET title=?, due_at=?, quadrant=?, completed=?, rule_score=?, ai_rank=?, ai_reason=?, "
        "external_source=?, external_id=?, updated_at=? WHERE id=?"));
    q.addBindValue(task.title);
    q.addBindValue(task.dueAt.isValid() ? task.dueAt.toUTC().toString(Qt::ISODate) : QVariant());
    q.addBindValue(static_cast<int>(task.quadrant));
    q.addBindValue(task.completed ? 1 : 0);
    q.addBindValue(task.ruleScore);
    q.addBindValue(task.aiRank);
    q.addBindValue(task.aiReason);
    q.addBindValue(task.externalSource.isEmpty() ? QVariant() : task.externalSource);
    q.addBindValue(task.externalId.isEmpty() ? QVariant() : task.externalId);
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    q.addBindValue(task.id);

    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        AppLogger::error("DB", QStringLiteral("更新任务失败 id=%1: %2").arg(task.id).arg(q.lastError().text()));
        return false;
    }
    AppLogger::info("DB",
                    QStringLiteral("更新任务 id=%1 title=%2 Q%3 completed=%4")
                        .arg(task.id)
                        .arg(task.title)
                        .arg(static_cast<int>(task.quadrant))
                        .arg(task.completed));
    emit tasksChanged();
    return true;
}

bool TaskRepository::updateTaskQuadrant(qint64 id, EisenhowerQuadrant quadrant, QString *errorMsg)
{
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral("UPDATE tasks SET quadrant=?, updated_at=? WHERE id=?"));
    q.addBindValue(static_cast<int>(quadrant));
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    q.addBindValue(id);
    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        AppLogger::error("DB", QStringLiteral("更新象限失败 id=%1: %2").arg(id).arg(q.lastError().text()));
        return false;
    }
    AppLogger::info("DB", QStringLiteral("更新象限 id=%1 -> Q%2").arg(id).arg(static_cast<int>(quadrant)));
    emit tasksChanged();
    return true;
}

bool TaskRepository::updateTaskCompleted(qint64 id, bool completed, QString *errorMsg)
{
    QSqlQuery q(d->db);
    const QDateTime now = QDateTime::currentDateTimeUtc();
    q.prepare(QStringLiteral("UPDATE tasks SET completed=?, completed_at=?, updated_at=? WHERE id=?"));
    q.addBindValue(completed ? 1 : 0);
    q.addBindValue(completed ? now.toString(Qt::ISODate) : QVariant());
    q.addBindValue(now.toString(Qt::ISODate));
    q.addBindValue(id);
    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        AppLogger::error("DB", QStringLiteral("更新完成状态失败 id=%1: %2").arg(id).arg(q.lastError().text()));
        return false;
    }
    AppLogger::info("DB",
                    QStringLiteral("更新完成状态 id=%1 completed=%2 at=%3")
                        .arg(id)
                        .arg(completed)
                        .arg(completed ? now.toString(Qt::ISODate) : QStringLiteral("(cleared)")));
    emit tasksChanged();
    return true;
}

bool TaskRepository::applyTaskCompletionGlobally(qint64 id, bool completed, QString *errorMsg)
{
    const QDateTime now = QDateTime::currentDateTimeUtc();
    const QDateTime completedAt = completed ? now : QDateTime();

    if (!TaskArchive::updateTaskCompletionInArchives(id, completed, completedAt, errorMsg))
        return false;

    QSqlQuery existsQ(d->db);
    existsQ.prepare(QStringLiteral("SELECT 1 FROM tasks WHERE id=? LIMIT 1"));
    existsQ.addBindValue(id);
    const bool inDb = existsQ.exec() && existsQ.next();

    if (inDb)
        return updateTaskCompleted(id, completed, errorMsg);

    emit tasksChanged();
    return true;
}

bool TaskRepository::deleteTask(qint64 id, QString *errorMsg)
{
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral("DELETE FROM tasks WHERE id=?"));
    q.addBindValue(id);
    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        AppLogger::error("DB", QStringLiteral("删除任务失败 id=%1: %2").arg(id).arg(q.lastError().text()));
        return false;
    }
    AppLogger::info("DB", QStringLiteral("删除任务 id=%1").arg(id));
    emit tasksChanged();
    return true;
}

bool TaskRepository::resetAllQuadrants(int *affectedCount, QString *errorMsg)
{
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral("UPDATE tasks SET quadrant=?, updated_at=?"));
    q.addBindValue(static_cast<int>(EisenhowerQuadrant::Unassigned));
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        AppLogger::error("DB", QStringLiteral("重置象限失败: %1").arg(q.lastError().text()));
        return false;
    }
    const int count = q.numRowsAffected();
    if (affectedCount)
        *affectedCount = count;
    AppLogger::info("DB", QStringLiteral("重置所有任务象限为不确定，影响 %1 条").arg(count));
    emit tasksChanged();
    return true;
}

bool TaskRepository::recordBehaviorEvent(BehaviorEventType type, qint64 taskId, EisenhowerQuadrant quadrant,
                                         const QDateTime &at, QString *errorMsg)
{
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral(
        "INSERT INTO behavior_events (event_type, task_id, quadrant, hour_of_day, created_at) "
        "VALUES (?, ?, ?, ?, ?)"));
    q.addBindValue(behaviorEventTypeName(type));
    q.addBindValue(taskId);
    q.addBindValue(static_cast<int>(quadrant));
    q.addBindValue(at.toLocalTime().time().hour());
    q.addBindValue(at.toString(Qt::ISODate));
    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        return false;
    }
    return true;
}

BehaviorStats TaskRepository::behaviorStatsLast30Days() const
{
    BehaviorStats stats;
    const QDateTime since = QDateTime::currentDateTimeUtc().addDays(-30);
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral("SELECT event_type, quadrant FROM behavior_events WHERE created_at >= ?"));
    q.addBindValue(since.toString(Qt::ISODate));
    if (!q.exec())
        return stats;

    while (q.next()) {
        const QString type = q.value(0).toString();
        const int quadrant = q.value(1).toInt();
        if (type == QStringLiteral("quadrant_changed"))
            stats.quadrantMoveCounts[quadrant]++;
        else if (type == QStringLiteral("task_completed"))
            stats.completionCount++;
        else if (type == QStringLiteral("ai_accepted"))
            stats.aiAcceptedCount++;
        else if (type == QStringLiteral("focus_completed"))
            stats.focusCompletedCount++;
    }
    return stats;
}

bool TaskRepository::insertFocusSession(qint64 taskId, int durationSec, int pomodoroIndex, qint64 *sessionId,
                                        QString *errorMsg)
{
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral(
        "INSERT INTO focus_sessions (task_id, started_at, duration_sec, pomodoro_index) "
        "VALUES (?, ?, ?, ?)"));
    q.addBindValue(taskId);
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    q.addBindValue(durationSec);
    q.addBindValue(pomodoroIndex);
    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        return false;
    }
    if (sessionId)
        *sessionId = q.lastInsertId().toLongLong();
    return true;
}

bool TaskRepository::finishFocusSession(qint64 sessionId, bool completed, bool abandoned, QString *errorMsg)
{
    if (sessionId <= 0)
        return false;

    QSqlQuery q(d->db);
    q.prepare(QStringLiteral(
        "UPDATE focus_sessions SET ended_at=?, completed=?, abandoned=? WHERE id=?"));
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    q.addBindValue(completed ? 1 : 0);
    q.addBindValue(abandoned ? 1 : 0);
    q.addBindValue(sessionId);
    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        return false;
    }
    return true;
}

static bool isLocalDate(const QDateTime &dt, const QDate &localDate)
{
    return dt.isValid() && dt.toLocalTime().date() == localDate;
}

bool TaskRepository::focusStatsForDate(const QDate &localDate, FocusDayStats *out) const
{
    if (!out || !localDate.isValid())
        return false;

    FocusDayStats stats;
    QSqlQuery q(d->db);
    if (!q.exec(QStringLiteral(
            "SELECT started_at, ended_at, duration_sec, completed, abandoned FROM focus_sessions")))
        return false;

    while (q.next()) {
        const QDateTime started = QDateTime::fromString(q.value(0).toString(), Qt::ISODate);
        if (!isLocalDate(started, localDate))
            continue;

        ++stats.sessionCount;
        if (q.value(3).toInt())
            ++stats.completedSessions;
        if (q.value(4).toInt())
            ++stats.abandonedSessions;

        if (!q.value(1).toString().isEmpty()) {
            const int minutes = q.value(2).toInt() / 60;
            stats.totalFocusMinutes += qMax(1, minutes);
        }
    }

    *out = stats;
    return true;
}

QVector<TaskItem> TaskRepository::tasksCompletedOnDate(const QDate &localDate) const
{
    QVector<TaskItem> out;
    if (!localDate.isValid())
        return out;

    QSqlQuery q(d->db);
    if (!q.exec(QStringLiteral("SELECT * FROM tasks WHERE completed=1")))
        return out;

    while (q.next()) {
        const TaskItem t = taskFromQuery(q);
        const QDateTime at = t.completedAt.isValid() ? t.completedAt : t.updatedAt;
        if (isLocalDate(at, localDate))
            out.append(t);
    }
    return out;
}

QVector<TaskItem> TaskRepository::tasksDueOnDate(const QDate &localDate) const
{
    QVector<TaskItem> out;
    if (!localDate.isValid())
        return out;

    for (const TaskItem &t : allTasks()) {
        if (t.dueAt.isValid() && t.dueAt.toLocalTime().date() == localDate)
            out.append(t);
    }

    QSet<qint64> ids;
    for (const TaskItem &t : out)
        ids.insert(t.id);
    for (const TaskItem &t : TaskArchive::loadExpiredTasks(localDate)) {
        if (!ids.contains(t.id)) {
            out.append(t);
            ids.insert(t.id);
        }
    }
    return out;
}

QDate TaskRepository::earliestTaskActivityDate() const
{
    QDate earliest;
    QSqlQuery q(d->db);
    if (q.exec(QStringLiteral("SELECT MIN(created_at) FROM tasks")) && q.next()) {
        const QDateTime created = QDateTime::fromString(q.value(0).toString(), Qt::ISODate);
        if (created.isValid()) {
            const QDate d = created.toLocalTime().date();
            if (!earliest.isValid() || d < earliest)
                earliest = d;
        }
    }
    QSqlQuery q2(d->db);
    if (q2.exec(QStringLiteral("SELECT MIN(completed_at) FROM tasks WHERE completed_at IS NOT NULL"))
        && q2.next()) {
        const QDateTime completed = QDateTime::fromString(q2.value(0).toString(), Qt::ISODate);
        if (completed.isValid()) {
            const QDate d = completed.toLocalTime().date();
            if (!earliest.isValid() || d < earliest)
                earliest = d;
        }
    }
    return earliest.isValid() ? earliest : QDate::currentDate();
}

bool TaskRepository::saveDailyEvaluation(const DailyEvaluation &eval, QString *errorMsg)
{
    if (!eval.evalDate.isValid()) {
        if (errorMsg)
            *errorMsg = QStringLiteral("评估日期无效");
        return false;
    }

    const QDateTime now = QDateTime::currentDateTimeUtc();
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral(
        "INSERT OR REPLACE INTO daily_evaluations "
        "(eval_date, tasks_completed, tasks_due, tasks_pending, summary, ai_feedback, task_plan_review, used_llm, llm_provider, llm_model, created_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));
    q.addBindValue(eval.evalDate.toString(QStringLiteral("yyyy-MM-dd")));
    q.addBindValue(eval.tasksCompleted);
    q.addBindValue(eval.tasksDue);
    q.addBindValue(eval.tasksPending);
    q.addBindValue(eval.summary);
    q.addBindValue(eval.aiFeedback);
    q.addBindValue(eval.taskPlanReview);
    q.addBindValue(eval.usedLlm ? 1 : 0);
    q.addBindValue(static_cast<int>(eval.llmProvider));
    q.addBindValue(eval.llmModel);
    q.addBindValue((eval.createdAt.isValid() ? eval.createdAt : now).toString(Qt::ISODate));

    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        AppLogger::error("DB", QStringLiteral("保存每日评估失败: %1").arg(q.lastError().text()));
        return false;
    }
    AppLogger::info("DB",
                    QStringLiteral("保存每日评估 date=%1 completed=%2 due=%3 pending=%4 llm=%5")
                        .arg(eval.evalDate.toString(Qt::ISODate))
                        .arg(eval.tasksCompleted)
                        .arg(eval.tasksDue)
                        .arg(eval.tasksPending)
                        .arg(eval.usedLlm));
    return true;
}

static DailyEvaluation evaluationFromQuery(const QSqlQuery &q)
{
    DailyEvaluation e;
    e.id = q.value(QStringLiteral("id")).toLongLong();
    e.evalDate = QDate::fromString(q.value(QStringLiteral("eval_date")).toString(), QStringLiteral("yyyy-MM-dd"));
    e.tasksCompleted = q.value(QStringLiteral("tasks_completed")).toInt();
    e.tasksDue = q.value(QStringLiteral("tasks_due")).toInt();
    e.tasksPending = q.value(QStringLiteral("tasks_pending")).toInt();
    e.summary = q.value(QStringLiteral("summary")).toString();
    e.aiFeedback = q.value(QStringLiteral("ai_feedback")).toString();
    e.taskPlanReview = q.value(QStringLiteral("task_plan_review")).toString();
    e.usedLlm = q.value(QStringLiteral("used_llm")).toInt() != 0;
    e.llmProvider = static_cast<LlmProviderType>(q.value(QStringLiteral("llm_provider")).toInt());
    e.llmModel = q.value(QStringLiteral("llm_model")).toString();
    e.createdAt = QDateTime::fromString(q.value(QStringLiteral("created_at")).toString(), Qt::ISODate);
    return e;
}

DailyEvaluation TaskRepository::dailyEvaluationForDate(const QDate &date) const
{
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral("SELECT * FROM daily_evaluations WHERE eval_date=?"));
    q.addBindValue(date.toString(QStringLiteral("yyyy-MM-dd")));
    if (q.exec() && q.next())
        return evaluationFromQuery(q);
    return {};
}

QVector<QDate> TaskRepository::dailyEvaluationDates() const
{
    QVector<QDate> dates;
    for (const DailyEvaluation &e : allDailyEvaluations())
        dates.append(e.evalDate);
    return dates;
}

QVector<DailyEvaluation> TaskRepository::allDailyEvaluations() const
{
    QVector<DailyEvaluation> list;
    QSqlQuery q(d->db);
    if (!q.exec(QStringLiteral("SELECT * FROM daily_evaluations ORDER BY eval_date DESC")))
        return list;

    while (q.next())
        list.append(evaluationFromQuery(q));
    return list;
}

bool TaskRepository::deleteDailyEvaluation(const QDate &date, QString *errorMsg)
{
    if (!date.isValid()) {
        if (errorMsg)
            *errorMsg = QStringLiteral("评估日期无效");
        return false;
    }

    QSqlQuery q(d->db);
    q.prepare(QStringLiteral("DELETE FROM daily_evaluations WHERE eval_date=?"));
    q.addBindValue(date.toString(QStringLiteral("yyyy-MM-dd")));
    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        AppLogger::error("DB", QStringLiteral("删除每日评估失败: %1").arg(q.lastError().text()));
        return false;
    }
    AppLogger::info("DB", QStringLiteral("已删除每日评估 date=%1").arg(date.toString(Qt::ISODate)));
    return true;
}

bool TaskRepository::hasDailyEvaluation(const QDate &date) const
{
    return dailyEvaluationForDate(date).evalDate.isValid();
}

static WeeklyReportRecord weeklyReportFromQuery(const QSqlQuery &q)
{
    WeeklyReportRecord r;
    r.id = q.value(QStringLiteral("id")).toLongLong();
    r.weekStart = QDate::fromString(q.value(QStringLiteral("week_start")).toString(), QStringLiteral("yyyy-MM-dd"));
    r.weekEnd = QDate::fromString(q.value(QStringLiteral("week_end")).toString(), QStringLiteral("yyyy-MM-dd"));
    r.markdown = q.value(QStringLiteral("markdown")).toString();
    r.usedLlm = q.value(QStringLiteral("used_llm")).toInt() != 0;
    r.llmProvider = static_cast<LlmProviderType>(q.value(QStringLiteral("llm_provider")).toInt());
    r.llmModel = q.value(QStringLiteral("llm_model")).toString();
    r.selectedTaskIdsJson = q.value(QStringLiteral("selected_task_ids")).toString();
    r.createdAt = QDateTime::fromString(q.value(QStringLiteral("created_at")).toString(), Qt::ISODate);
    return r;
}

bool TaskRepository::saveWeeklyReport(const WeeklyReportRecord &record, QString *errorMsg)
{
    if (!record.weekStart.isValid() || !record.weekEnd.isValid()) {
        if (errorMsg)
            *errorMsg = QStringLiteral("周报周期无效");
        return false;
    }

    const QDateTime now = QDateTime::currentDateTimeUtc();
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral(
        "INSERT OR REPLACE INTO weekly_reports "
        "(week_start, week_end, markdown, used_llm, llm_provider, llm_model, selected_task_ids, created_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"));
    q.addBindValue(record.weekStart.toString(QStringLiteral("yyyy-MM-dd")));
    q.addBindValue(record.weekEnd.toString(QStringLiteral("yyyy-MM-dd")));
    q.addBindValue(record.markdown);
    q.addBindValue(record.usedLlm ? 1 : 0);
    q.addBindValue(static_cast<int>(record.llmProvider));
    q.addBindValue(record.llmModel);
    q.addBindValue(record.selectedTaskIdsJson);
    q.addBindValue((record.createdAt.isValid() ? record.createdAt : now).toString(Qt::ISODate));

    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        AppLogger::error("DB", QStringLiteral("保存周报失败: %1").arg(q.lastError().text()));
        return false;
    }
    AppLogger::info("DB",
                    QStringLiteral("保存周报 week=%1~%2 llm=%3 model=%4")
                        .arg(record.weekStart.toString(Qt::ISODate), record.weekEnd.toString(Qt::ISODate))
                        .arg(record.usedLlm)
                        .arg(record.llmModel));
    return true;
}

WeeklyReportRecord TaskRepository::weeklyReportForWeek(const QDate &weekStart, const QDate &weekEnd) const
{
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral("SELECT * FROM weekly_reports WHERE week_start=? AND week_end=?"));
    q.addBindValue(weekStart.toString(QStringLiteral("yyyy-MM-dd")));
    q.addBindValue(weekEnd.toString(QStringLiteral("yyyy-MM-dd")));
    if (q.exec() && q.next())
        return weeklyReportFromQuery(q);
    return {};
}

QVector<WeeklyReportRecord> TaskRepository::allWeeklyReports() const
{
    QVector<WeeklyReportRecord> list;
    QSqlQuery q(d->db);
    if (!q.exec(QStringLiteral("SELECT * FROM weekly_reports ORDER BY created_at DESC")))
        return list;
    while (q.next())
        list.append(weeklyReportFromQuery(q));
    return list;
}

WeeklyReportRecord TaskRepository::latestWeeklyReport() const
{
    QSqlQuery q(d->db);
    if (!q.exec(QStringLiteral("SELECT * FROM weekly_reports ORDER BY created_at DESC LIMIT 1")))
        return {};
    if (q.next())
        return weeklyReportFromQuery(q);
    return {};
}

bool TaskRepository::deleteWeeklyReport(qint64 id, QString *errorMsg)
{
    if (id <= 0) {
        if (errorMsg)
            *errorMsg = QStringLiteral("周报记录无效");
        return false;
    }
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral("DELETE FROM weekly_reports WHERE id=?"));
    q.addBindValue(id);
    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        AppLogger::error("DB", QStringLiteral("删除周报失败: %1").arg(q.lastError().text()));
        return false;
    }
    AppLogger::info("DB", QStringLiteral("已删除周报 id=%1").arg(id));
    return true;
}
