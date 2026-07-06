#ifndef WEEKLY_REPORT_SERVICE_H
#define WEEKLY_REPORT_SERVICE_H

#include "task_types.h"

#include <QDate>
#include <QObject>
#include <QVector>

class TaskRepository;

struct WeeklyReportResult
{
    bool success = false;
    bool usedLlm = false;
    QString markdown;
    QString errorMessage;
    QDate weekStart;
    QDate weekEnd;
    LlmProviderType llmProvider = LlmProviderType::Ollama;
    QString llmModel;
};

QVector<TaskItem> collectWeekCandidateTasks(TaskRepository *repo, const QDate &weekStart,
                                            const QDate &weekEnd);
WeeklyReportResult generateWeeklyReport(const LlmConfig &config, const QVector<TaskItem> &selectedTasks,
                                        const QDate &weekStart, const QDate &weekEnd);

class WeeklyReportService : public QObject
{
    Q_OBJECT
public:
    explicit WeeklyReportService(TaskRepository *repo, QObject *parent = nullptr);
    ~WeeklyReportService() override;

    void setLlmConfig(const LlmConfig &config);
    bool isBusy() const;
    QDate pendingWeekStart() const;
    QDate pendingWeekEnd() const;

public slots:
    void generateAsync(const QVector<TaskItem> &selectedTasks, const QDate &weekStart, const QDate &weekEnd);

signals:
    void generationStarted(const QDate &weekStart, const QDate &weekEnd);
    void generationFinished(const WeeklyReportRecord &record, const WeeklyReportResult &result);

private slots:
    void onFutureFinished();

private:
    TaskRepository *m_repo = nullptr;
    LlmConfig m_config;
    bool m_busy = false;
    QDate m_pendingWeekStart;
    QDate m_pendingWeekEnd;
    QString m_pendingTaskIdsJson;
    class Private;
    Private *d;
};

#endif // WEEKLY_REPORT_SERVICE_H
