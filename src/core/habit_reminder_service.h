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

public slots:
    void acknowledge(qint64 habitId);
    void snooze(qint64 habitId, int minutes = 5);
    void disableForToday(qint64 habitId);
    void setHabitEnabled(qint64 habitId, bool enabled);

signals:
    void reminderDue(const HabitReminder &habit);

private:
    void onTick();
    bool canTriggerNow() const;
    bool isWithinActiveHours() const;
    bool isAllowedWeekday() const;
    bool isDue(const HabitReminder &habit) const;
    QDateTime computeNextTrigger(const HabitReminder &habit, const QDateTime &from) const;
    QDateTime startOfNextAllowedDay(const QDate &after) const;
    void scheduleHabit(HabitReminder &habit, const QDateTime &nextTrigger,
                       const QDateTime &lastTriggered = QDateTime());
    void dispatchDueReminders();

    HabitReminderRepository *m_repo = nullptr;
    QTimer m_timer;
    bool m_focusActive = false;
    bool m_appLocked = false;
    QHash<qint64, QDate> m_disabledForDay;
    QSet<qint64> m_awaitingAck;
};

#endif // HABIT_REMINDER_SERVICE_H
