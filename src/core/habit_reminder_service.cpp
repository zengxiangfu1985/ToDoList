#include "habit_reminder_service.h"

#include "app_settings.h"
#include "habit_reminder_repository.h"

#include "../utils/app_logger.h"

#include <QDateTime>
#include <QSet>
#include <QTime>

HabitReminderService::HabitReminderService(HabitReminderRepository *repo, QObject *parent)
    : QObject(parent)
    , m_repo(repo)
{
    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &HabitReminderService::onTick);
}

void HabitReminderService::ensureAllDayActiveHoursDefault()
{
    const QTime start = AppSettings::habitActiveStart();
    const QTime end = AppSettings::habitActiveEnd();
    // Migrate previous restricted defaults that pushed next fire to "tomorrow 09:00"
    // (shown as ~11h countdown in the evening).
    const bool oldDefault = (start == QTime(9, 0)
                             && (end == QTime(18, 0) || end == QTime(22, 0)));
    if (oldDefault) {
        AppSettings::setHabitActiveStart(QTime(0, 0));
        AppSettings::setHabitActiveEnd(QTime(23, 59));
        AppLogger::info("Habit", QStringLiteral("活跃时段已调整为全天候 00:00–23:59"));
    }
}

void HabitReminderService::start()
{
    ensureAllDayActiveHoursDefault();

    // Do not re-show popups left over from a previous run.
    const QList<qint64> leftover = m_awaitingAck.values();
    m_awaitingAck.clear();
    for (const qint64 id : leftover)
        emit reminderCancelled(id);

    // Exit/restart always starts a fresh cycle (do not resume remaining countdown).
    resetAllCyclesFromNow();
    m_timer.start();
    AppLogger::info("Habit", QStringLiteral("健康提醒已启动：倒计时已按间隔重置（全天候）"));
}

void HabitReminderService::reload()
{
    if (!m_repo || !m_repo->isOpen())
        return;

    const QDate today = QDate::currentDate();
    for (auto it = m_disabledForDay.begin(); it != m_disabledForDay.end();) {
        if (it.value() < today)
            it = m_disabledForDay.erase(it);
        else
            ++it;
    }

    deferOverdueToNextCycle();
}

void HabitReminderService::deferOverdueToNextCycle()
{
    if (!m_repo || !m_repo->isOpen())
        return;

    const QDate today = QDate::currentDate();
    const QDateTime now = QDateTime::currentDateTimeUtc();

    for (HabitReminder habit : m_repo->allHabits()) {
        if (!habit.enabled) {
            const bool wasAwaiting = m_awaitingAck.remove(habit.id);
            if (habit.nextTriggerAt.isValid()) {
                habit.nextTriggerAt = QDateTime();
                QString err;
                m_repo->updateHabit(habit, &err);
            }
            if (wasAwaiting)
                emit reminderCancelled(habit.id);
            continue;
        }

        if (m_awaitingAck.contains(habit.id))
            continue;
        if (m_disabledForDay.value(habit.id) == today)
            continue;

        const bool overdue = !habit.nextTriggerAt.isValid()
            || habit.nextTriggerAt.toUTC() <= now;

        if (overdue) {
            // Wait one full cycle from now (clamped into active hours).
            scheduleHabit(habit, computeNextTrigger(habit, now));
            continue;
        }

        // Future schedule: keep if still inside active hours / weekday rules.
        const QDateTime localNext = habit.nextTriggerAt.toLocalTime();
        const bool weekdayOk = !AppSettings::habitWeekdaysOnly()
            || (localNext.date().dayOfWeek() >= 1 && localNext.date().dayOfWeek() <= 5);
        if (weekdayOk && isTimeWithinActiveHours(localNext.time()))
            continue;

        scheduleHabit(habit, computeNextTrigger(habit, now));
    }
}

void HabitReminderService::resetAllCyclesFromNow()
{
    if (!m_repo || !m_repo->isOpen())
        return;

    const QDate today = QDate::currentDate();
    const QDateTime now = QDateTime::currentDateTimeUtc();

    for (HabitReminder habit : m_repo->allHabits()) {
        if (!habit.enabled) {
            const bool wasAwaiting = m_awaitingAck.remove(habit.id);
            if (habit.nextTriggerAt.isValid()) {
                habit.nextTriggerAt = QDateTime();
                QString err;
                m_repo->updateHabit(habit, &err);
            }
            if (wasAwaiting)
                emit reminderCancelled(habit.id);
            continue;
        }

        if (m_disabledForDay.value(habit.id) == today)
            continue;

        // Fresh full interval from process start (ignores leftover countdown in DB).
        scheduleHabit(habit, computeNextTrigger(habit, now));
    }
}

void HabitReminderService::setFocusActive(bool active)
{
    m_focusActive = active;
}

void HabitReminderService::setAppLocked(bool locked)
{
    m_appLocked = locked;
}

QString HabitReminderService::pauseReason() const
{
    if (m_appLocked)
        return QObject::tr("已锁定");
    if (m_focusActive && AppSettings::habitPauseDuringFocus())
        return QObject::tr("专注暂停");
    if (!isAllowedWeekday())
        return QObject::tr("仅工作日");
    if (!isWithinActiveHours()) {
        const QTime start = AppSettings::habitActiveStart();
        const QTime end = AppSettings::habitActiveEnd();
        return QObject::tr("时段外 %1–%2")
            .arg(start.toString(QStringLiteral("HH:mm")))
            .arg(end.toString(QStringLiteral("HH:mm")));
    }
    return QString();
}

QString HabitReminderService::countdownText(const HabitReminder &habit) const
{
    if (!habit.enabled)
        return QObject::tr("已关闭");
    if (m_disabledForDay.value(habit.id) == QDate::currentDate())
        return QObject::tr("今日不再");
    if (m_awaitingAck.contains(habit.id))
        return QObject::tr("待处理");

    if (!habit.nextTriggerAt.isValid()) {
        const QString paused = pauseReason();
        return paused.isEmpty() ? QStringLiteral("--:--") : paused;
    }

    const qint64 secs = QDateTime::currentDateTimeUtc().secsTo(habit.nextTriggerAt.toUTC());
    if (secs <= 0) {
        const QString paused = pauseReason();
        return paused.isEmpty() ? QObject::tr("即将提醒") : paused;
    }

    const int hours = static_cast<int>(secs / 3600);
    const int minutes = static_cast<int>((secs % 3600) / 60);
    const int seconds = static_cast<int>(secs % 60);
    if (hours > 0) {
        return QStringLiteral("%1:%2:%3")
            .arg(hours)
            .arg(minutes, 2, 10, QLatin1Char('0'))
            .arg(seconds, 2, 10, QLatin1Char('0'));
    }
    return QStringLiteral("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

void HabitReminderService::acknowledge(qint64 habitId)
{
    if (!m_repo)
        return;

    HabitReminder habit;
    for (const HabitReminder &h : m_repo->allHabits()) {
        if (h.id == habitId) {
            habit = h;
            break;
        }
    }
    if (habit.id <= 0)
        return;

    m_awaitingAck.remove(habitId);
    QString err;
    m_repo->insertLog(habitId, false, &err);
    const QDateTime now = QDateTime::currentDateTimeUtc();
    scheduleHabit(habit, computeNextTrigger(habit, now), now);
    AppLogger::info("Habit", QStringLiteral("习惯已完成 id=%1 title=%2").arg(habitId).arg(habit.title));
}

void HabitReminderService::snooze(qint64 habitId, int minutes)
{
    if (!m_repo || minutes <= 0)
        return;

    HabitReminder habit;
    for (const HabitReminder &h : m_repo->allHabits()) {
        if (h.id == habitId) {
            habit = h;
            break;
        }
    }
    if (habit.id <= 0)
        return;

    m_awaitingAck.remove(habitId);
    QString err;
    m_repo->insertLog(habitId, true, &err);
    const QDateTime now = QDateTime::currentDateTimeUtc();
    scheduleHabit(habit, now.addSecs(minutes * 60), now);
}

void HabitReminderService::disableForToday(qint64 habitId)
{
    m_awaitingAck.remove(habitId);
    m_disabledForDay.insert(habitId, QDate::currentDate());

    if (!m_repo)
        return;

    HabitReminder habit;
    for (const HabitReminder &h : m_repo->allHabits()) {
        if (h.id == habitId) {
            habit = h;
            break;
        }
    }
    if (habit.id <= 0)
        return;

    scheduleHabit(habit, startOfNextAllowedDay(QDate::currentDate()));
}

void HabitReminderService::setHabitEnabled(qint64 habitId, bool enabled)
{
    if (!m_repo)
        return;

    for (HabitReminder habit : m_repo->allHabits()) {
        if (habit.id != habitId)
            continue;
        habit.enabled = enabled;
        if (!enabled)
            habit.nextTriggerAt = QDateTime();
        QString err;
        if (!m_repo->updateHabit(habit, &err))
            return;
        if (enabled) {
            const QDateTime now = QDateTime::currentDateTimeUtc();
            // Newly enabled: first reminder after one full interval, not immediately.
            scheduleHabit(habit, computeNextTrigger(habit, now));
        } else {
            m_awaitingAck.remove(habitId);
            emit reminderCancelled(habitId);
        }
        return;
    }
}

bool HabitReminderService::isAwaitingAck(qint64 habitId) const
{
    return m_awaitingAck.contains(habitId);
}

bool HabitReminderService::canTriggerNow() const
{
    return pauseReason().isEmpty();
}

bool HabitReminderService::isWithinActiveHours() const
{
    return isTimeWithinActiveHours(QTime::currentTime());
}

bool HabitReminderService::isTimeWithinActiveHours(const QTime &time) const
{
    if (isAllDayActiveHours())
        return true;
    const QTime start = AppSettings::habitActiveStart();
    const QTime end = AppSettings::habitActiveEnd();
    if (!start.isValid() || !end.isValid())
        return true;
    if (start <= end)
        return time >= start && time <= end;
    return time >= start || time <= end;
}

bool HabitReminderService::isAllDayActiveHours() const
{
    const QTime start = AppSettings::habitActiveStart();
    const QTime end = AppSettings::habitActiveEnd();
    return start == QTime(0, 0) && end.hour() == 23 && end.minute() >= 59;
}

bool HabitReminderService::isAllowedWeekday() const
{
    if (!AppSettings::habitWeekdaysOnly())
        return true;
    const int day = QDate::currentDate().dayOfWeek();
    return day >= 1 && day <= 5;
}

bool HabitReminderService::isDue(const HabitReminder &habit) const
{
    if (!habit.enabled || habit.id <= 0)
        return false;
    if (m_disabledForDay.value(habit.id) == QDate::currentDate())
        return false;
    if (m_awaitingAck.contains(habit.id))
        return false;
    if (!habit.nextTriggerAt.isValid())
        return true;
    return habit.nextTriggerAt.toUTC() <= QDateTime::currentDateTimeUtc();
}

QDateTime HabitReminderService::computeNextTrigger(const HabitReminder &habit,
                                                   const QDateTime &from) const
{
    // Always: base next = from + interval (UTC). Avoid pushing into "tomorrow 09:00".
    QDateTime candidate = from.toUTC().addSecs(qMax(1, habit.intervalMinutes) * 60);

    if (isAllDayActiveHours() && !AppSettings::habitWeekdaysOnly())
        return candidate;

    for (int guard = 0; guard < 366; ++guard) {
        const QDate localDate = candidate.toLocalTime().date();
        const QTime localTime = candidate.toLocalTime().time();
        const QTime start = AppSettings::habitActiveStart();
        const QTime end = AppSettings::habitActiveEnd();

        if (AppSettings::habitWeekdaysOnly()) {
            const int day = localDate.dayOfWeek();
            if (day < 1 || day > 5) {
                candidate = startOfNextAllowedDay(localDate);
                continue;
            }
        }

        if (!isAllDayActiveHours() && start.isValid() && end.isValid()) {
            if (start <= end) {
                if (localTime < start) {
                    candidate = QDateTime(localDate, start, Qt::LocalTime).toUTC();
                    continue;
                }
                if (localTime > end) {
                    candidate = startOfNextAllowedDay(localDate);
                    continue;
                }
            } else if (localTime > end && localTime < start) {
                candidate = QDateTime(localDate, start, Qt::LocalTime).toUTC();
                continue;
            }
        }

        return candidate;
    }

    return candidate;
}

QDateTime HabitReminderService::startOfNextAllowedDay(const QDate &after) const
{
    QDate day = after.addDays(1);
    const QTime start = AppSettings::habitActiveStart();
    for (int i = 0; i < 8; ++i) {
        if (!AppSettings::habitWeekdaysOnly()
            || (day.dayOfWeek() >= 1 && day.dayOfWeek() <= 5)) {
            return QDateTime(day, start.isValid() ? start : QTime(0, 0), Qt::LocalTime).toUTC();
        }
        day = day.addDays(1);
    }
    return QDateTime(day, start.isValid() ? start : QTime(0, 0), Qt::LocalTime).toUTC();
}

void HabitReminderService::scheduleHabit(HabitReminder &habit, const QDateTime &nextTrigger,
                                         const QDateTime &lastTriggered)
{
    if (!m_repo)
        return;

    if (lastTriggered.isValid())
        habit.lastTriggeredAt = lastTriggered;
    habit.nextTriggerAt = nextTrigger;

    QString err;
    m_repo->updateHabit(habit, &err);
}

void HabitReminderService::dispatchDueReminders()
{
    if (!m_repo)
        return;

    const QVector<HabitReminder> habits = m_repo->allHabits();
    for (const HabitReminder &habit : habits) {
        if (!isDue(habit))
            continue;
        m_awaitingAck.insert(habit.id);
        emit reminderDue(habit);
    }
}

void HabitReminderService::onTick()
{
    emit statusTick();
    if (!canTriggerNow())
        return;
    dispatchDueReminders();
}
