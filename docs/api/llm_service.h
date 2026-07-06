#ifndef LLM_SERVICE_H
#define LLM_SERVICE_H

#include "task_types.h"

#include <QObject>

class LlmProvider;

// Scenario: S02 Step 3-5 — 异步 AI 分析门面
class LlmService : public QObject
{
    Q_OBJECT
public:
    explicit LlmService(QObject *parent = nullptr);
    ~LlmService() override;

    void setConfig(const LlmConfig &config);
    LlmConfig config() const;
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
