#ifndef HABIT_REMINDER_REPOSITORY_H
#define HABIT_REMINDER_REPOSITORY_H

#include "task_types.h"

#include <QObject>
#include <QVector>

class HabitReminderRepository : public QObject
{
    Q_OBJECT
public:
    explicit HabitReminderRepository(QObject *parent = nullptr);
    ~HabitReminderRepository() override;

    bool open(const QString &dbPath, QString *errorMsg = nullptr);
    bool isOpen() const;

    QVector<HabitReminder> allHabits() const;
    HabitReminder habitByKind(HabitKind kind) const;
    bool ensureDefaults(QString *errorMsg = nullptr);
    bool updateHabit(const HabitReminder &habit, QString *errorMsg = nullptr);
    bool updateSchedule(qint64 id, const QDateTime &lastTriggered, const QDateTime &nextTrigger,
                        QString *errorMsg = nullptr);
    bool insertLog(qint64 habitId, bool snoozed, QString *errorMsg = nullptr);
    int completedCountForDate(const QDate &date) const;

signals:
    void habitsChanged();

private:
    class Private;
    Private *d;
};

#endif // HABIT_REMINDER_REPOSITORY_H
