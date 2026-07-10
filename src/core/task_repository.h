#ifndef TASK_REPOSITORY_H
#define TASK_REPOSITORY_H

#include "task_types.h"

#include <QHash>
#include <QObject>
#include <QVector>

struct BehaviorStats {
    QHash<int, int> quadrantMoveCounts;
    int completionCount = 0;
    int aiAcceptedCount = 0;
    int focusCompletedCount = 0;
};

struct FocusDayStats {
    int sessionCount = 0;
    int completedSessions = 0;
    int abandonedSessions = 0;
    int totalFocusMinutes = 0;
};

class TaskRepository : public QObject
{
    Q_OBJECT
public:
    explicit TaskRepository(QObject *parent = nullptr);
    ~TaskRepository() override;

    bool open(const QString &dbPath, QString *errorMsg = nullptr);
    bool isOpen() const;
    QVector<TaskItem> allTasks() const;
    QVector<TaskItem> activeTasks() const;
    QVector<TaskItem> activeTasksDueOnDate(const QDate &localDate) const;
    int archiveOverdueTasks(QString *errorMsg = nullptr);
    QVector<TaskItem> yesterdayUnfinishedTasks() const;
    bool addTask(TaskItem *task, QString *errorMsg = nullptr);
    bool updateTask(const TaskItem &task, QString *errorMsg = nullptr);
    bool updateTaskQuadrant(qint64 id, EisenhowerQuadrant quadrant, QString *errorMsg = nullptr);
    bool updateTaskCompleted(qint64 id, bool completed, QString *errorMsg = nullptr);
    bool deleteTask(qint64 id, QString *errorMsg = nullptr);
    bool resetAllQuadrants(int *affectedCount = nullptr, QString *errorMsg = nullptr);
    TaskItem findByExternal(const QString &source, const QString &externalId) const;

    QVector<TaskItem> tasksCompletedOnDate(const QDate &localDate) const;
    QVector<TaskItem> tasksDueOnDate(const QDate &localDate) const;
    QDate earliestTaskActivityDate() const;

    bool saveDailyEvaluation(const DailyEvaluation &eval, QString *errorMsg = nullptr);
    DailyEvaluation dailyEvaluationForDate(const QDate &date) const;
    QVector<DailyEvaluation> allDailyEvaluations() const;
    QVector<QDate> dailyEvaluationDates() const;
    bool hasDailyEvaluation(const QDate &date) const;
    bool deleteDailyEvaluation(const QDate &date, QString *errorMsg = nullptr);

    bool saveWeeklyReport(const WeeklyReportRecord &record, QString *errorMsg = nullptr);
    WeeklyReportRecord weeklyReportForWeek(const QDate &weekStart, const QDate &weekEnd) const;
    QVector<WeeklyReportRecord> allWeeklyReports() const;
    bool deleteWeeklyReport(qint64 id, QString *errorMsg = nullptr);
    WeeklyReportRecord latestWeeklyReport() const;

    bool recordBehaviorEvent(BehaviorEventType type, qint64 taskId, EisenhowerQuadrant quadrant,
                             const QDateTime &at, QString *errorMsg = nullptr);
    BehaviorStats behaviorStatsLast30Days() const;

    bool insertFocusSession(qint64 taskId, int durationSec, int pomodoroIndex, qint64 *sessionId,
                            QString *errorMsg = nullptr);
    bool finishFocusSession(qint64 sessionId, bool completed, bool abandoned, QString *errorMsg = nullptr);
    bool focusStatsForDate(const QDate &localDate, FocusDayStats *out) const;

signals:
    void tasksChanged();

private:
    bool ensureSchema(QString *errorMsg);
    class Private;
    Private *d;
};

#endif // TASK_REPOSITORY_H
