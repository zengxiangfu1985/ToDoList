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
    m_timer.setInterval(30000);
    connect(&m_timer, &QTimer::timeout, this, &HabitReminderService::onTick);
}

void HabitReminderService::start()
{
    reload();
    m_timer.start();
    QTimer::singleShot(2000, this, &HabitReminderService::onTick);
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

    const QDateTime now = QDateTime::currentDateTimeUtc();
    for (HabitReminder habit : m_repo->allHabits()) {
        if (!habit.enabled) {
            if (habit.nextTriggerAt.isValid()) {
                habit.nextTriggerAt = QDateTime();
                QString err;
                m_repo->updateHabit(habit, &err);
            }
            continue;
        }
        if (!habit.nextTriggerAt.isValid())
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
        QString err;
        if (!m_repo->updateHabit(habit, &err))
            return;
        if (enabled) {
            const QDateTime now = QDateTime::currentDateTimeUtc();
            scheduleHabit(habit, computeNextTrigger(habit, now));
        }
        return;
    }
}

bool HabitReminderService::canTriggerNow() const
{
    if (m_appLocked)
        return false;
    if (m_focusActive && AppSettings::habitPauseDuringFocus())
        return false;
    if (!isWithinActiveHours())
        return false;
    if (!isAllowedWeekday())
        return false;
    return true;
}

bool HabitReminderService::isWithinActiveHours() const
{
    const QTime now = QTime::currentTime();
    const QTime start = AppSettings::habitActiveStart();
    const QTime end = AppSettings::habitActiveEnd();
    if (!start.isValid() || !end.isValid())
        return true;
    if (start <= end)
        return now >= start && now <= end;
    return now >= start || now <= end;
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
    QDateTime candidate = from.toUTC().addSecs(qMax(1, habit.intervalMinutes) * 60);

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

        if (start.isValid() && end.isValid()) {
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
    if (!canTriggerNow())
        return;
    dispatchDueReminders();
}
