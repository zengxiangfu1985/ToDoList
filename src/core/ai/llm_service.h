#ifndef LLM_SERVICE_H
#define LLM_SERVICE_H

#include "../task_types.h"

#include <QObject>

class LlmService : public QObject
{
    Q_OBJECT
public:
    explicit LlmService(QObject *parent = nullptr);
    ~LlmService() override;

    void setConfig(const LlmConfig &config);
    LlmConfig config() const;
    void setScoringWeights(const ScoringWeights &weights);
    ScoringWeights scoringWeights() const;
    bool isBusy() const;

public slots:
    void analyzePriorities(const QVector<TaskItem> &tasks);

signals:
    void analysisStarted();
    void analysisFinished(const PriorityAnalysisResult &result);
    void providerChanged(LlmProviderType type);

private:
    class Private;
    Private *d;
};

#endif // LLM_SERVICE_H
