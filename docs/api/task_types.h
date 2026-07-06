#ifndef TASK_TYPES_H
#define TASK_TYPES_H

#include <QString>
#include <QDateTime>
#include <QMetaType>
#include <QVector>

enum class EisenhowerQuadrant {
    Q1_UrgentImportant = 1,
    Q2_NotUrgentImportant = 2,
    Q3_UrgentNotImportant = 3,
    Q4_NotUrgentNotImportant = 4
};

enum class LlmProviderType {
    Ollama,
    DeepSeek,
    Kimi,
    CustomOpenAI
};

struct TaskItem {
    qint64 id = 0;
    QString title;
    QDateTime dueAt;
    EisenhowerQuadrant quadrant = EisenhowerQuadrant::Q2_NotUrgentImportant;
    bool completed = false;
    double ruleScore = 0.0;
    int aiRank = 0;
    QString aiReason;
    QDateTime createdAt;
    QDateTime updatedAt;
};

struct LlmConfig {
    LlmProviderType provider = LlmProviderType::Ollama;
    QString baseUrl = QStringLiteral("http://127.0.0.1:11434");
    QString apiKey;
    QString model = QStringLiteral("qwen2.5:3b");
    int timeoutMs = 30000;
};

struct LlmRequest {
    QString systemPrompt;
    QString userPrompt;
    double temperature = 0.3;
    int maxTokens = 1024;
};

struct LlmResponse {
    bool success = false;
    QString content;
    QString errorMessage;
    LlmProviderType provider = LlmProviderType::Ollama;
};

struct PriorityRecommendation {
    qint64 taskId = 0;
    QString title;
    int rank = 0;
    double score = 0.0;
    QString reason;
};

struct PriorityAnalysisResult {
    bool success = false;
    bool usedLlm = false;
    QString errorMessage;
    QVector<PriorityRecommendation> top3;
};

Q_DECLARE_METATYPE(PriorityAnalysisResult)

#endif // TASK_TYPES_H
