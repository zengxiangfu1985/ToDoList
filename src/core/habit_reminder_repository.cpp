#include "habit_reminder_repository.h"

#include "../utils/app_logger.h"

#include <QDate>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTime>
#include <QVariant>

class HabitReminderRepository::Private
{
public:
    QSqlDatabase db;
    QString connectionName;
};

static HabitReminder habitFromQuery(const QSqlQuery &q)
{
    HabitReminder h;
    h.id = q.value(QStringLiteral("id")).toLongLong();
    h.title = q.value(QStringLiteral("title")).toString();
    h.message = q.value(QStringLiteral("message")).toString();
    h.kind = static_cast<HabitKind>(q.value(QStringLiteral("kind")).toInt());
    h.enabled = q.value(QStringLiteral("enabled")).toInt() != 0;
    h.intervalMinutes = q.value(QStringLiteral("interval_minutes")).toInt();
    const QString lastTriggered = q.value(QStringLiteral("last_triggered_at")).toString();
    if (!lastTriggered.isEmpty())
        h.lastTriggeredAt = QDateTime::fromString(lastTriggered, Qt::ISODate);
    const QString nextTrigger = q.value(QStringLiteral("next_trigger_at")).toString();
    if (!nextTrigger.isEmpty())
        h.nextTriggerAt = QDateTime::fromString(nextTrigger, Qt::ISODate);
    h.sortOrder = q.value(QStringLiteral("sort_order")).toInt();
    return h;
}

static QVector<HabitReminder> defaultHabits()
{
    QVector<HabitReminder> habits;

    HabitReminder stand;
    stand.title = QStringLiteral("起身活动");
    stand.message = QStringLiteral("站起来走动 3～5 分钟，伸展肩颈");
    stand.kind = HabitKind::StandUp;
    stand.enabled = true;
    stand.intervalMinutes = 45;
    stand.sortOrder = 1;
    habits.append(stand);

    HabitReminder eye;
    eye.title = QStringLiteral("护眼休息");
    eye.message = QStringLiteral("20-20-20：看 6 米外 20 秒，放松眼睛");
    eye.kind = HabitKind::EyeRest;
    eye.enabled = true;
    eye.intervalMinutes = 20;
    eye.sortOrder = 2;
    habits.append(eye);

    HabitReminder water;
    water.title = QStringLiteral("喝水提醒");
    water.message = QStringLiteral("喝杯水，休息一下");
    water.kind = HabitKind::DrinkWater;
    water.enabled = false;
    water.intervalMinutes = 60;
    water.sortOrder = 3;
    habits.append(water);

    return habits;
}

HabitReminderRepository::HabitReminderRepository(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    d->connectionName =
        QStringLiteral("habit_conn_%1").arg(reinterpret_cast<quintptr>(this));
}

HabitReminderRepository::~HabitReminderRepository()
{
    if (d->db.isOpen())
        d->db.close();
    QSqlDatabase::removeDatabase(d->connectionName);
    delete d;
}

bool HabitReminderRepository::open(const QString &dbPath, QString *errorMsg)
{
    if (d->db.isOpen())
        d->db.close();

    d->db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), d->connectionName);
    d->db.setDatabaseName(dbPath);
    if (!d->db.open()) {
        if (errorMsg)
            *errorMsg = d->db.lastError().text();
        return false;
    }

    return ensureDefaults(errorMsg);
}

bool HabitReminderRepository::isOpen() const
{
    return d->db.isOpen();
}

QVector<HabitReminder> HabitReminderRepository::allHabits() const
{
    QVector<HabitReminder> habits;
    if (!d->db.isOpen())
        return habits;

    QSqlQuery q(d->db);
    if (!q.exec(QStringLiteral("SELECT * FROM habit_reminders ORDER BY sort_order, id")))
        return habits;

    while (q.next())
        habits.append(habitFromQuery(q));
    return habits;
}

HabitReminder HabitReminderRepository::habitByKind(HabitKind kind) const
{
    if (!d->db.isOpen())
        return {};

    QSqlQuery q(d->db);
    q.prepare(QStringLiteral("SELECT * FROM habit_reminders WHERE kind=? LIMIT 1"));
    q.addBindValue(static_cast<int>(kind));
    if (q.exec() && q.next())
        return habitFromQuery(q);
    return {};
}

bool HabitReminderRepository::ensureDefaults(QString *errorMsg)
{
    QSqlQuery countQ(d->db);
    if (!countQ.exec(QStringLiteral("SELECT COUNT(*) FROM habit_reminders"))
        || !countQ.next()) {
        if (errorMsg)
            *errorMsg = countQ.lastError().text();
        return false;
    }
    if (countQ.value(0).toInt() > 0)
        return true;

    const QDateTime now = QDateTime::currentDateTimeUtc();
    for (const HabitReminder &preset : defaultHabits()) {
        QSqlQuery q(d->db);
        q.prepare(QStringLiteral(
            "INSERT INTO habit_reminders "
            "(title, message, kind, enabled, interval_minutes, next_trigger_at, sort_order) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)"));
        q.addBindValue(preset.title);
        q.addBindValue(preset.message);
        q.addBindValue(static_cast<int>(preset.kind));
        q.addBindValue(preset.enabled ? 1 : 0);
        q.addBindValue(preset.intervalMinutes);
        q.addBindValue(now.toString(Qt::ISODate));
        q.addBindValue(preset.sortOrder);
        if (!q.exec()) {
            if (errorMsg)
                *errorMsg = q.lastError().text();
            return false;
        }
    }

    AppLogger::info("Habit", QStringLiteral("已初始化默认健康习惯提醒"));
    emit habitsChanged();
    return true;
}

bool HabitReminderRepository::updateHabit(const HabitReminder &habit, QString *errorMsg)
{
    if (habit.id <= 0) {
        if (errorMsg)
            *errorMsg = QStringLiteral("无效的习惯 ID");
        return false;
    }

    QSqlQuery q(d->db);
    q.prepare(QStringLiteral(
        "UPDATE habit_reminders SET title=?, message=?, enabled=?, interval_minutes=?, "
        "last_triggered_at=?, next_trigger_at=?, sort_order=? WHERE id=?"));
    q.addBindValue(habit.title);
    q.addBindValue(habit.message);
    q.addBindValue(habit.enabled ? 1 : 0);
    q.addBindValue(habit.intervalMinutes);
    q.addBindValue(habit.lastTriggeredAt.isValid() ? habit.lastTriggeredAt.toString(Qt::ISODate)
                                                     : QVariant());
    q.addBindValue(habit.nextTriggerAt.isValid() ? habit.nextTriggerAt.toString(Qt::ISODate)
                                                 : QVariant());
    q.addBindValue(habit.sortOrder);
    q.addBindValue(habit.id);
    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        return false;
    }

    emit habitsChanged();
    return true;
}

bool HabitReminderRepository::updateSchedule(qint64 id, const QDateTime &lastTriggered,
                                             const QDateTime &nextTrigger, QString *errorMsg)
{
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral(
        "UPDATE habit_reminders SET last_triggered_at=?, next_trigger_at=? WHERE id=?"));
    q.addBindValue(lastTriggered.isValid() ? lastTriggered.toString(Qt::ISODate) : QVariant());
    q.addBindValue(nextTrigger.isValid() ? nextTrigger.toString(Qt::ISODate) : QVariant());
    q.addBindValue(id);
    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        return false;
    }
    return true;
}

bool HabitReminderRepository::insertLog(qint64 habitId, bool snoozed, QString *errorMsg)
{
    const QDateTime now = QDateTime::currentDateTimeUtc();
    QSqlQuery q(d->db);
    q.prepare(QStringLiteral(
        "INSERT INTO habit_reminder_logs (habit_id, triggered_at, acknowledged_at, snoozed) "
        "VALUES (?, ?, ?, ?)"));
    q.addBindValue(habitId);
    q.addBindValue(now.toString(Qt::ISODate));
    q.addBindValue(snoozed ? QVariant() : now.toString(Qt::ISODate));
    q.addBindValue(snoozed ? 1 : 0);
    if (!q.exec()) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        return false;
    }
    return true;
}

int HabitReminderRepository::completedCountForDate(const QDate &date) const
{
    if (!date.isValid() || !d->db.isOpen())
        return 0;

    const QDateTime start = QDateTime(date, QTime(0, 0), Qt::LocalTime).toUTC();
    const QDateTime end = QDateTime(date.addDays(1), QTime(0, 0), Qt::LocalTime).toUTC();

    QSqlQuery q(d->db);
    q.prepare(QStringLiteral(
        "SELECT COUNT(*) FROM habit_reminder_logs "
        "WHERE snoozed=0 AND acknowledged_at IS NOT NULL "
        "AND triggered_at >= ? AND triggered_at < ?"));
    q.addBindValue(start.toString(Qt::ISODate));
    q.addBindValue(end.toString(Qt::ISODate));
    if (q.exec() && q.next())
        return q.value(0).toInt();
    return 0;
}
