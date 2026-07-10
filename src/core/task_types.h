#ifndef TASK_TYPES_H
#define TASK_TYPES_H

#include <QString>
#include <QDateTime>
#include <QMetaType>
#include <QVector>

enum class EisenhowerQuadrant {
    Unassigned = 0,
    Q1_UrgentImportant = 1,
    Q2_NotUrgentImportant = 2,
    Q3_UrgentNotImportant = 3,
    Q4_NotUrgentNotImportant = 4
};

inline bool isQuadrantAssigned(EisenhowerQuadrant q)
{
    const int v = static_cast<int>(q);
    return v >= 1 && v <= 4;
}

enum class LlmProviderType {
    Ollama,
    DeepSeek,
    Kimi,
    CustomOpenAI
};

enum class BehaviorEventType {
    QuadrantChanged,
    TaskCompleted,
    AiRecommendationAccepted
};

struct ScoringWeights {
    double urgency = 0.30;
    double importance = 0.30;
    double business = 0.20;
    double effort = 0.20;

    void normalize();
};

struct TaskItem {
    qint64 id = 0;
    QString title;
    QDateTime dueAt;
    EisenhowerQuadrant quadrant = EisenhowerQuadrant::Q2_NotUrgentImportant;
    bool completed = false;
    QDateTime completedAt;
    double ruleScore = 0.0;
    int aiRank = 0;
    QString aiReason;
    QString externalSource;
    QString externalId;
    QDateTime createdAt;
    QDateTime updatedAt;
    bool archived = false;
    QDateTime archivedAt;
};

struct LlmConfig {
    LlmProviderType provider = LlmProviderType::Ollama;
    QString baseUrl = QStringLiteral("http://127.0.0.1:11434");
    QString apiKey;
    QString model = QStringLiteral("qwen2.5:3b");
    int timeoutMs = 30000;
};

struct M365Config {
    QString tenantId = QStringLiteral("common");
    QString clientId;
    QString accessToken;
    QString refreshToken;
    QDateTime tokenExpiry;
    bool isConfigured() const;
};

struct GraphMailItem {
    QString id;
    QString subject;
    QDateTime receivedAt;
    bool flagged = false;
};

struct M365SyncResult {
    bool success = false;
    int imported = 0;
    int skipped = 0;
    QString errorMessage;
};

enum class LlmOutputFormat {
    Default,
    JsonQuadrantArray,
    JsonTop3Array,
    JsonQuickCaptureArray
};

struct LlmRequest {
    QString systemPrompt;
    QString userPrompt;
    double temperature = 0.3;
    int maxTokens = 1024;
    LlmOutputFormat outputFormat = LlmOutputFormat::Default;
    int timeoutMsOverride = 0;
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

struct QuadrantAssignment {
    qint64 taskId = 0;
    EisenhowerQuadrant quadrant = EisenhowerQuadrant::Q2_NotUrgentImportant;
    QString reason;
};

struct AnalysisTraceEntry {
    QDateTime timestamp;
    QString level;
    QString category;
    QString message;
    QString detail;
};

struct PriorityAnalysisResult {
    bool success = false;
    bool usedLlm = false;
    bool usedLlmForQuadrants = false;
    QString errorMessage;
    QString quadrantClassifyMessage;
    QDateTime analysisStartedAt;
    QDateTime analysisFinishedAt;
    QVector<AnalysisTraceEntry> trace;
    QVector<PriorityRecommendation> top3;
    QVector<QuadrantAssignment> quadrantAssignments;
};

struct DailyEvaluation {
    qint64 id = 0;
    QDate evalDate;
    int tasksCompleted = 0;
    int tasksDue = 0;
    int tasksPending = 0;
    QString summary;
    QString aiFeedback;
    QString taskPlanReview;
    bool usedLlm = false;
    LlmProviderType llmProvider = LlmProviderType::Ollama;
    QString llmModel;
    QDateTime createdAt;
};

struct WeeklyReportRecord {
    qint64 id = 0;
    QDate weekStart;
    QDate weekEnd;
    QString markdown;
    bool usedLlm = false;
    LlmProviderType llmProvider = LlmProviderType::Ollama;
    QString llmModel;
    QString selectedTaskIdsJson;
    QDateTime createdAt;
};

struct SavedDailyTop3 {
    QDate date;
    LlmProviderType provider = LlmProviderType::Ollama;
    QString model;
    bool usedLlm = false;
    QVector<PriorityRecommendation> top3;
    QVector<AnalysisTraceEntry> trace;
    bool valid = false;
};

Q_DECLARE_METATYPE(SavedDailyTop3)
Q_DECLARE_METATYPE(WeeklyReportRecord)

Q_DECLARE_METATYPE(PriorityAnalysisResult)
Q_DECLARE_METATYPE(M365SyncResult)

#endif // TASK_TYPES_H
