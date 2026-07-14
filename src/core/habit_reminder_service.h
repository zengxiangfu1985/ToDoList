#ifndef HABIT_REMINDER_SERVICE_H
#define HABIT_REMINDER_SERVICE_H

#include "task_types.h"

#include <QDate>
#include <QHash>
#include <QObject>
#include <QSet>
#include <QTimer>

class HabitReminderRepository;

class HabitReminderService : public QObject
{
    Q_OBJECT
public:
    explicit HabitReminderService(HabitReminderRepository *repo, QObject *parent = nullptr);

    void start();
    void reload();
    void setFocusActive(bool active);
    void setAppLocked(bool locked);

    QString countdownText(const HabitReminder &habit) const;
    QString pauseReason() const;
    bool isAwaitingAck(qint64 habitId) const;

public slots:
    void acknowledge(qint64 habitId);
    void snooze(qint64 habitId, int minutes = 5);
    void disableForToday(qint64 habitId);
    void setHabitEnabled(qint64 habitId, bool enabled);

signals:
    void reminderDue(const HabitReminder &habit);
    void statusTick();
    /** Emitted when a habit is turned off or cancelled — UI should drop queued/open popups. */
    void reminderCancelled(qint64 habitId);

private:
    void onTick();
    bool canTriggerNow() const;
    bool isWithinActiveHours() const;
    bool isTimeWithinActiveHours(const QTime &time) const;
    bool isAllDayActiveHours() const;
    bool isAllowedWeekday() const;
    bool isDue(const HabitReminder &habit) const;
    QDateTime computeNextTrigger(const HabitReminder &habit, const QDateTime &from) const;
    QDateTime startOfNextAllowedDay(const QDate &after) const;
    void scheduleHabit(HabitReminder &habit, const QDateTime &nextTrigger,
                       const QDateTime &lastTriggered = QDateTime());
    void dispatchDueReminders();
    /** Schedule overdue / invalid habits to now + one full interval (never fire immediately). */
    void deferOverdueToNextCycle();
    /** On each app launch: reset every enabled habit to a fresh full interval from now. */
    void resetAllCyclesFromNow();
    static void ensureAllDayActiveHoursDefault();

    HabitReminderRepository *m_repo = nullptr;
    QTimer m_timer;
    bool m_focusActive = false;
    bool m_appLocked = false;
    QHash<qint64, QDate> m_disabledForDay;
    QSet<qint64> m_awaitingAck;
};

#endif // HABIT_REMINDER_SERVICE_H
