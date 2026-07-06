#ifndef DAILY_EVALUATION_SERVICE_H
#define DAILY_EVALUATION_SERVICE_H

#include "task_types.h"

#include <QDate>
#include <QObject>
#include <QVector>

class TaskRepository;
class QTimer;

class DailyEvaluationService : public QObject
{
    Q_OBJECT
public:
    explicit DailyEvaluationService(TaskRepository *repo, QObject *parent = nullptr);
    ~DailyEvaluationService() override;

    void setLlmConfig(const LlmConfig &config);
    void start();

    bool isBusy() const;
    QVector<QDate> pendingCatchUpDates() const;

public slots:
    void runCatchUp();
    void evaluateDateAsync(const QDate &date);
    void regenerateDateAsync(const QDate &date);

signals:
    void evaluationStarted(const QDate &date);
    void evaluationFinished(const QDate &date, const DailyEvaluation &result);
    void catchUpFinished(int evaluatedCount);

private slots:
    void onScheduleTick();
    void onEvaluationFutureFinished();

private:
    void armScheduleTimer();
    QDate lastEvaluableDate() const;
    QVector<QDate> missedEvaluationDates() const;
    bool shouldEvaluateDate(const QDate &date) const;
    void enqueueCatchUpDates(const QVector<QDate> &dates);
    void processNextCatchUp();

    TaskRepository *m_repo = nullptr;
    LlmConfig m_config;
    QTimer *m_scheduleTimer = nullptr;
    QDate m_lastScheduledRunDate;
    bool m_busy = false;
    QVector<QDate> m_catchUpQueue;
    class Private;
    Private *d;
};

#endif // DAILY_EVALUATION_SERVICE_H
