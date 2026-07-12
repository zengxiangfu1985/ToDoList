#ifndef TASK_ARCHIVE_H
#define TASK_ARCHIVE_H

#include "task_types.h"

#include <QDate>
#include <QDateTime>
#include <QString>
#include <QVector>

class TaskArchive
{
public:
    static QString defaultDataDirectory();
    static QString databasePath();
    static QString snapshotPath(const QDate &date);

    static bool ensureDataDirectory(QString *errorMsg = nullptr);
    static bool exportDailySnapshot(const QVector<TaskItem> &tasks, const QDate &date = QDate::currentDate(),
                                    QString *errorMsg = nullptr);
    static QStringList availableHistoryDates();
    static QVector<TaskItem> loadSnapshot(const QDate &date, QString *errorMsg = nullptr);
    static QVector<TaskItem> loadHistoryForDate(const QDate &date, QString *errorMsg = nullptr);

    static QString expiredPath(const QDate &dueDate);
    static bool appendExpiredTasks(const QVector<TaskItem> &tasks, const QDate &dueDate,
                                   QString *errorMsg = nullptr);
    static QVector<TaskItem> loadExpiredTasks(const QDate &dueDate, QString *errorMsg = nullptr);
    static bool updateTaskCompletionInArchives(qint64 taskId, bool completed, const QDateTime &completedAt,
                                               QString *errorMsg = nullptr);

    static QString dailyTop3Path(const QDate &date = QDate::currentDate());
    static QString top3ModelKey(const LlmConfig &config);
    static SavedDailyTop3 loadDailyTop3(const QDate &date = QDate::currentDate());
    static QVector<PriorityRecommendation> hydrateTop3Recommendations(
        const QVector<PriorityRecommendation> &saved, const QVector<TaskItem> &activeTasks);
    static bool saveDailyTop3(const QDate &date, const LlmConfig &config,
                              const PriorityAnalysisResult &result, QString *errorMsg = nullptr);
    static bool clearDailyTop3(const QDate &date = QDate::currentDate());
};

#endif // TASK_ARCHIVE_H
