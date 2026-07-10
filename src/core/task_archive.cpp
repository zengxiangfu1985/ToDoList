#include "task_archive.h"

#include "ai/ai_prompts.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QSet>
#include <algorithm>

static QJsonObject taskToJson(const TaskItem &t)
{
    QJsonObject o;
    o.insert(QStringLiteral("id"), static_cast<double>(t.id));
    o.insert(QStringLiteral("title"), t.title);
    o.insert(QStringLiteral("due_at"), t.dueAt.isValid() ? t.dueAt.toString(Qt::ISODate) : QString());
    o.insert(QStringLiteral("quadrant"), static_cast<int>(t.quadrant));
    o.insert(QStringLiteral("completed"), t.completed);
    if (t.completedAt.isValid())
        o.insert(QStringLiteral("completed_at"), t.completedAt.toString(Qt::ISODate));
    o.insert(QStringLiteral("archived"), t.archived);
    if (t.archivedAt.isValid())
        o.insert(QStringLiteral("archived_at"), t.archivedAt.toString(Qt::ISODate));
    if (t.createdAt.isValid())
        o.insert(QStringLiteral("created_at"), t.createdAt.toString(Qt::ISODate));
    o.insert(QStringLiteral("rule_score"), t.ruleScore);
    o.insert(QStringLiteral("ai_rank"), t.aiRank);
    o.insert(QStringLiteral("ai_reason"), t.aiReason);
    return o;
}

static TaskItem taskFromJson(const QJsonObject &o)
{
    TaskItem t;
    t.id = static_cast<qint64>(o.value(QStringLiteral("id")).toDouble());
    t.title = o.value(QStringLiteral("title")).toString();
    const QString due = o.value(QStringLiteral("due_at")).toString();
    if (!due.isEmpty())
        t.dueAt = QDateTime::fromString(due, Qt::ISODate);
    t.quadrant = static_cast<EisenhowerQuadrant>(o.value(QStringLiteral("quadrant")).toInt());
    t.completed = o.value(QStringLiteral("completed")).toBool();
    const QString completedAt = o.value(QStringLiteral("completed_at")).toString();
    if (!completedAt.isEmpty())
        t.completedAt = QDateTime::fromString(completedAt, Qt::ISODate);
    t.archived = o.value(QStringLiteral("archived")).toBool();
    const QString archivedAt = o.value(QStringLiteral("archived_at")).toString();
    if (!archivedAt.isEmpty())
        t.archivedAt = QDateTime::fromString(archivedAt, Qt::ISODate);
    const QString createdAt = o.value(QStringLiteral("created_at")).toString();
    if (!createdAt.isEmpty())
        t.createdAt = QDateTime::fromString(createdAt, Qt::ISODate);
    t.ruleScore = o.value(QStringLiteral("rule_score")).toDouble();
    t.aiRank = o.value(QStringLiteral("ai_rank")).toInt();
    t.aiReason = o.value(QStringLiteral("ai_reason")).toString();
    return t;
}

QString TaskArchive::defaultDataDirectory()
{
    return QCoreApplication::applicationDirPath() + QStringLiteral("/data");
}

QString TaskArchive::databasePath()
{
    return defaultDataDirectory() + QStringLiteral("/tasks.db");
}

QString TaskArchive::snapshotPath(const QDate &date)
{
    return defaultDataDirectory() + QStringLiteral("/tasks-%1.json")
        .arg(date.toString(QStringLiteral("yyyy-MM-dd")));
}

bool TaskArchive::ensureDataDirectory(QString *errorMsg)
{
    QDir dir;
    if (dir.mkpath(defaultDataDirectory()))
        return true;
    if (errorMsg)
        *errorMsg = QObject::tr("无法创建数据目录: %1").arg(defaultDataDirectory());
    return false;
}

bool TaskArchive::exportDailySnapshot(const QVector<TaskItem> &tasks, const QDate &date, QString *errorMsg)
{
    if (!ensureDataDirectory(errorMsg))
        return false;

    QJsonArray arr;
    for (const TaskItem &t : tasks)
        arr.append(taskToJson(t));

    QJsonObject root;
    root.insert(QStringLiteral("date"), date.toString(Qt::ISODate));
    root.insert(QStringLiteral("exported_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    root.insert(QStringLiteral("tasks"), arr);

    QFile file(snapshotPath(date));
    if (!file.open(QIODevice::WriteOnly)) {
        if (errorMsg)
            *errorMsg = file.errorString();
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

QStringList TaskArchive::availableHistoryDates()
{
    QSet<QString> dateSet;
    QDir dir(defaultDataDirectory());
    const QStringList snapFiles = dir.entryList({QStringLiteral("tasks-*.json")}, QDir::Files);
    for (const QString &name : snapFiles) {
        const QString d = name.mid(6, 10);
        if (d.size() == 10)
            dateSet.insert(d);
    }
    const QStringList expiredFiles = dir.entryList({QStringLiteral("expired-*.json")}, QDir::Files);
    for (const QString &name : expiredFiles) {
        const QString d = name.mid(8, 10);
        if (d.size() == 10)
            dateSet.insert(d);
    }
    QStringList dates = dateSet.values();
    std::sort(dates.begin(), dates.end(), std::greater<QString>());
    return dates;
}

QVector<TaskItem> TaskArchive::loadHistoryForDate(const QDate &date, QString *errorMsg)
{
    QVector<TaskItem> items = loadSnapshot(date, errorMsg);
    QSet<qint64> ids;
    for (const TaskItem &t : items)
        ids.insert(t.id);

    QFile file(expiredPath(date));
    if (file.open(QIODevice::ReadOnly)) {
        const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        const QJsonArray arr = doc.object().value(QStringLiteral("tasks")).toArray();
        for (const QJsonValue &v : arr) {
            const TaskItem t = taskFromJson(v.toObject());
            if (!ids.contains(t.id)) {
                items.append(t);
                ids.insert(t.id);
            }
        }
    }
    return items;
}

QString TaskArchive::expiredPath(const QDate &dueDate)
{
    return defaultDataDirectory() + QStringLiteral("/expired-%1.json")
        .arg(dueDate.toString(QStringLiteral("yyyy-MM-dd")));
}

bool TaskArchive::appendExpiredTasks(const QVector<TaskItem> &tasks, const QDate &dueDate, QString *errorMsg)
{
    if (tasks.isEmpty())
        return true;
    if (!ensureDataDirectory(errorMsg))
        return false;

    QSet<qint64> existingIds;
    QJsonArray arr;
    QFile existingFile(expiredPath(dueDate));
    if (existingFile.open(QIODevice::ReadOnly)) {
        const QJsonDocument doc = QJsonDocument::fromJson(existingFile.readAll());
        for (const QJsonValue &v : doc.object().value(QStringLiteral("tasks")).toArray()) {
            const TaskItem t = taskFromJson(v.toObject());
            existingIds.insert(t.id);
            arr.append(taskToJson(t));
        }
    }

    for (const TaskItem &t : tasks) {
        if (existingIds.contains(t.id))
            continue;
        arr.append(taskToJson(t));
    }

    QJsonObject root;
    root.insert(QStringLiteral("date"), dueDate.toString(Qt::ISODate));
    root.insert(QStringLiteral("archived_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    root.insert(QStringLiteral("tasks"), arr);

    QFile file(expiredPath(dueDate));
    if (!file.open(QIODevice::WriteOnly)) {
        if (errorMsg)
            *errorMsg = file.errorString();
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

QVector<TaskItem> TaskArchive::loadExpiredTasks(const QDate &dueDate, QString *errorMsg)
{
    QVector<TaskItem> items;
    QFile file(expiredPath(dueDate));
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMsg)
            *errorMsg = file.errorString();
        return items;
    }
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    for (const QJsonValue &v : doc.object().value(QStringLiteral("tasks")).toArray())
        items.append(taskFromJson(v.toObject()));
    return items;
}

QString TaskArchive::dailyTop3Path(const QDate &date)
{
    return defaultDataDirectory() + QStringLiteral("/top3-%1.json")
        .arg(date.toString(QStringLiteral("yyyy-MM-dd")));
}

QString TaskArchive::top3ModelKey(const LlmConfig &config)
{
    return QStringLiteral("%1|%2").arg(static_cast<int>(config.provider)).arg(config.model.trimmed());
}

SavedDailyTop3 TaskArchive::loadDailyTop3(const QDate &date)
{
    SavedDailyTop3 saved;
    saved.date = date;
    QFile file(dailyTop3Path(date));
    if (!file.open(QIODevice::ReadOnly))
        return saved;

    const QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
    saved.provider = static_cast<LlmProviderType>(root.value(QStringLiteral("provider")).toInt());
    saved.model = root.value(QStringLiteral("model")).toString();
    const QJsonArray arr = root.value(QStringLiteral("top3")).toArray();
    for (const QJsonValue &v : arr) {
        const QJsonObject o = v.toObject();
        PriorityRecommendation rec;
        rec.taskId = static_cast<qint64>(o.value(QStringLiteral("id")).toDouble());
        rec.title = o.value(QStringLiteral("title")).toString();
        rec.rank = o.value(QStringLiteral("rank")).toInt();
        rec.score = o.value(QStringLiteral("score")).toDouble();
        rec.reason = o.value(QStringLiteral("reason")).toString();
        rec.reason = AiPrompts::sanitizeTop3Reason(rec.reason);
        saved.top3.append(rec);
    }
    saved.usedLlm = root.value(QStringLiteral("used_llm")).toBool();
    const QJsonArray traceArr = root.value(QStringLiteral("trace")).toArray();
    for (const QJsonValue &v : traceArr) {
        const QJsonObject o = v.toObject();
        AnalysisTraceEntry entry;
        entry.timestamp = QDateTime::fromString(o.value(QStringLiteral("timestamp")).toString(), Qt::ISODate);
        entry.level = o.value(QStringLiteral("level")).toString();
        entry.category = o.value(QStringLiteral("category")).toString();
        entry.message = o.value(QStringLiteral("message")).toString();
        entry.detail = o.value(QStringLiteral("detail")).toString();
        saved.trace.append(entry);
    }
    saved.valid = !saved.top3.isEmpty() && !saved.model.isEmpty();
    return saved;
}

QVector<PriorityRecommendation> TaskArchive::hydrateTop3Recommendations(
    const QVector<PriorityRecommendation> &saved, const QVector<TaskItem> &activeTasks)
{
    QHash<qint64, TaskItem> byId;
    for (const TaskItem &t : activeTasks)
        byId.insert(t.id, t);

    QVector<PriorityRecommendation> result;
    result.reserve(saved.size());
    for (PriorityRecommendation rec : saved) {
        const auto it = byId.constFind(rec.taskId);
        if (it == byId.constEnd())
            continue;
        rec.title = it.value().title;
        rec.score = it.value().ruleScore;
        if (rec.reason.trimmed().isEmpty())
            rec.reason = QStringLiteral("建议优先处理：") + rec.title;
        else
            rec.reason = AiPrompts::sanitizeTop3Reason(rec.reason);
        result.append(rec);
    }

    std::sort(result.begin(), result.end(), [](const PriorityRecommendation &a, const PriorityRecommendation &b) {
        if (a.rank != b.rank)
            return a.rank < b.rank;
        return a.score > b.score;
    });
    for (int i = 0; i < result.size(); ++i)
        result[i].rank = i + 1;
    return result;
}

bool TaskArchive::saveDailyTop3(const QDate &date, const LlmConfig &config,
                                const PriorityAnalysisResult &result, QString *errorMsg)
{
    const QVector<PriorityRecommendation> &top3 = result.top3;
    if (top3.isEmpty())
        return false;
    if (!ensureDataDirectory(errorMsg))
        return false;

    QJsonArray arr;
    for (const PriorityRecommendation &rec : top3) {
        QJsonObject o;
        o.insert(QStringLiteral("id"), static_cast<double>(rec.taskId));
        o.insert(QStringLiteral("title"), rec.title);
        o.insert(QStringLiteral("rank"), rec.rank);
        o.insert(QStringLiteral("score"), rec.score);
        o.insert(QStringLiteral("reason"), rec.reason);
        arr.append(o);
    }

    QJsonObject root;
    root.insert(QStringLiteral("date"), date.toString(Qt::ISODate));
    root.insert(QStringLiteral("provider"), static_cast<int>(config.provider));
    root.insert(QStringLiteral("model"), config.model);
    root.insert(QStringLiteral("model_key"), top3ModelKey(config));
    root.insert(QStringLiteral("used_llm"), result.usedLlm);
    root.insert(QStringLiteral("saved_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    root.insert(QStringLiteral("top3"), arr);

    QJsonArray traceArr;
    for (const AnalysisTraceEntry &entry : result.trace) {
        QJsonObject o;
        o.insert(QStringLiteral("timestamp"), entry.timestamp.toString(Qt::ISODate));
        o.insert(QStringLiteral("level"), entry.level);
        o.insert(QStringLiteral("category"), entry.category);
        o.insert(QStringLiteral("message"), entry.message);
        o.insert(QStringLiteral("detail"), entry.detail);
        traceArr.append(o);
    }
    if (!traceArr.isEmpty())
        root.insert(QStringLiteral("trace"), traceArr);

    QFile file(dailyTop3Path(date));
    if (!file.open(QIODevice::WriteOnly)) {
        if (errorMsg)
            *errorMsg = file.errorString();
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

bool TaskArchive::clearDailyTop3(const QDate &date)
{
    return QFile::remove(dailyTop3Path(date));
}

QVector<TaskItem> TaskArchive::loadSnapshot(const QDate &date, QString *errorMsg)
{
    QVector<TaskItem> items;
    QFile file(snapshotPath(date));
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMsg)
            *errorMsg = file.errorString();
        return items;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    const QJsonArray arr = doc.object().value(QStringLiteral("tasks")).toArray();
    for (const QJsonValue &v : arr)
        items.append(taskFromJson(v.toObject()));
    return items;
}
