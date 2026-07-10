#include "llm_service.h"
#include "llm_provider.h"
#include "ai_prompts.h"
#include "../priority_engine.h"
#include "../../utils/ai_analysis_trace.h"
#include "../../utils/app_logger.h"
#include "../../utils/llm_json_utils.h"

#include <QFutureWatcher>
#include <QtConcurrent>
#include <algorithm>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>

class LlmService::Private
{
public:
    LlmConfig config = LlmProviderFactory::loadFromSettings();
    ScoringWeights weights;
    bool busy = false;
    QFutureWatcher<PriorityAnalysisResult> *watcher = nullptr;
};

static QString providerName(LlmProviderType type)
{
    switch (type) {
    case LlmProviderType::Ollama: return QStringLiteral("Ollama");
    case LlmProviderType::DeepSeek: return QStringLiteral("DeepSeek");
    case LlmProviderType::Kimi: return QStringLiteral("Kimi");
    case LlmProviderType::CustomOpenAI: return QStringLiteral("CustomOpenAI");
    }
    return QStringLiteral("Unknown");
}

static QVector<QuadrantAssignment> ruleFallbackAssignments(const QVector<TaskItem> &pending, const QString &reason,
                                                           AiAnalysisTrace &trace)
{
    QVector<QuadrantAssignment> out;
    trace.warn("AI", QStringLiteral("启用规则推断象限（原因: %1）").arg(reason));
    for (const TaskItem &t : pending) {
        QuadrantAssignment a;
        a.taskId = t.id;
        a.quadrant = PriorityEngine::guessQuadrantForUnassigned(t);
        a.reason = QStringLiteral("规则推断(%1)：根据标题与截止时间分配到 Q%2")
                       .arg(reason)
                       .arg(static_cast<int>(a.quadrant));
        out.append(a);
        trace.info("CALC",
                   QStringLiteral("规则象限 id=%1 title=%2 -> Q%3")
                       .arg(t.id)
                       .arg(t.title)
                       .arg(static_cast<int>(a.quadrant)),
                   t.dueAt.isValid() ? t.dueAt.toString(Qt::ISODate) : QStringLiteral("(无截止日期)"));
    }
    return out;
}

static QVector<QuadrantAssignment> parseQuadrantResponse(const LlmResponse &llm, const QVector<TaskItem> &chunk,
                                                         AiAnalysisTrace &trace, QString *errorMsg)
{
    if (!llm.success) {
        if (errorMsg)
            *errorMsg = llm.errorMessage;
        return {};
    }

    const QByteArray jsonBytes = LlmJsonUtils::extractPayload(llm.content);
    QString parseError;
    const QJsonArray arr = LlmJsonUtils::parseArray(jsonBytes, &parseError);
    if (arr.isEmpty()) {
        if (errorMsg)
            *errorMsg = QStringLiteral("JSON 解析失败: %1").arg(parseError);
        trace.warn("AI", *errorMsg, QString::fromUtf8(jsonBytes.left(2000)));
        return {};
    }

    QVector<QuadrantAssignment> out;
    QSet<qint64> assignedIds;
    for (const QJsonValue &v : arr) {
        const QJsonObject o = v.toObject();
        const int q = o.value(QStringLiteral("quadrant")).toInt();
        if (q < 1 || q > 4)
            continue;
        QuadrantAssignment a;
        a.taskId = static_cast<qint64>(o.value(QStringLiteral("id")).toDouble());
        if (a.taskId <= 0)
            continue;
        a.quadrant = static_cast<EisenhowerQuadrant>(q);
        a.reason = o.value(QStringLiteral("reason")).toString();
        if (a.reason.size() < 5)
            a.reason = QStringLiteral("AI 划分到 Q%1").arg(q);
        out.append(a);
        assignedIds.insert(a.taskId);
        trace.info("AI", QStringLiteral("LLM 象限 id=%1 -> Q%2").arg(a.taskId).arg(q), a.reason);
    }

    for (const TaskItem &t : chunk) {
        if (assignedIds.contains(t.id))
            continue;
        QuadrantAssignment a;
        a.taskId = t.id;
        a.quadrant = PriorityEngine::guessQuadrantForUnassigned(t);
        a.reason = QStringLiteral("规则补全：AI 未返回此项，按标题+截止时间推断 Q%1")
                       .arg(static_cast<int>(a.quadrant));
        out.append(a);
        trace.warn("AI",
                   QStringLiteral("任务 id=%1 未在 LLM 响应中，规则补全 -> Q%2")
                       .arg(t.id)
                       .arg(static_cast<int>(a.quadrant)),
                   a.reason);
    }
    return out;
}

static QVector<QuadrantAssignment> classifyPendingChunk(LlmProvider *provider, const LlmConfig &config,
                                                        const QVector<TaskItem> &chunk, int batchIndex,
                                                        int batchCount, AiAnalysisTrace &trace,
                                                        bool *usedLlm, QString *chunkMessage)
{
    if (usedLlm)
        *usedLlm = false;
    if (chunkMessage)
        chunkMessage->clear();

    LlmRequest req;
    req.systemPrompt = AiPrompts::quadrantClassificationSystemPrompt();
    req.userPrompt = PriorityEngine::buildQuadrantClassificationPrompt(chunk);
    req.temperature = 0.1;
    req.outputFormat = AiPrompts::quadrantOutputFormat(config.provider);
    req.maxTokens = AiPrompts::quadrantMaxTokens(config.provider, chunk.size());

    trace.info("LLM",
               QStringLiteral("发送象限分类请求 [%1/%2] provider=%3 model=%4 tasks=%5 maxTokens=%6 timeout=%7ms")
                   .arg(batchIndex + 1)
                   .arg(batchCount)
                   .arg(providerName(config.provider), config.model)
                   .arg(chunk.size())
                   .arg(req.maxTokens)
                   .arg(config.timeoutMs),
               QStringLiteral("System:\n%1\n\nUser:\n%2").arg(req.systemPrompt, req.userPrompt));

    const LlmResponse llm = provider->complete(req);
    trace.info("LLM",
               QStringLiteral("象限分类响应 [%1/%2] success=%3 contentLen=%4")
                   .arg(batchIndex + 1)
                   .arg(batchCount)
                   .arg(llm.success)
                   .arg(llm.content.size()),
               llm.success ? llm.content : llm.errorMessage);

    QString errorMsg;
    QVector<QuadrantAssignment> out = parseQuadrantResponse(llm, chunk, trace, &errorMsg);
    if (out.isEmpty()) {
        if (chunkMessage)
            *chunkMessage = errorMsg.isEmpty() ? QStringLiteral("LLM 返回空结果") : errorMsg;
        return ruleFallbackAssignments(chunk, QStringLiteral("批次失败"), trace);
    }

    if (usedLlm)
        *usedLlm = llm.success;
    if (chunkMessage)
        *chunkMessage = QStringLiteral("本批 %1 项").arg(out.size());
    return out;
}

static QVector<QuadrantAssignment> classifyUnassigned(LlmConfig config, const QVector<TaskItem> &tasks,
                                                        bool *usedLlm, QString *classifyMessage,
                                                        AiAnalysisTrace &trace)
{
    if (usedLlm)
        *usedLlm = false;
    if (classifyMessage)
        classifyMessage->clear();

    QVector<TaskItem> pending;
    for (const TaskItem &t : tasks) {
        if (!t.completed && !isQuadrantAssigned(t.quadrant))
            pending.append(t);
    }
    if (pending.isEmpty()) {
        trace.info("AI", QStringLiteral("无待分配象限的任务，跳过象限分类"));
        if (classifyMessage)
            *classifyMessage = QStringLiteral("无待分配任务");
        return {};
    }

    trace.info("AI", QStringLiteral("【阶段 1/2】象限分类 — 待分配 %1 条任务").arg(pending.size()));
    QString pendingList;
    for (const TaskItem &t : pending) {
        pendingList += QStringLiteral("  id=%1  %2  due=%3\n")
                           .arg(t.id)
                           .arg(t.title)
                           .arg(t.dueAt.isValid() ? t.dueAt.toString(Qt::ISODate) : QStringLiteral("(无)"));
    }
    trace.debug("AI", QStringLiteral("待分配任务列表"), pendingList.trimmed());

    auto provider = LlmProviderFactory::create(config);
    if (!provider || !provider->isConfigured()) {
        const QString msg = provider ? QStringLiteral("LLM 未配置，使用规则推断象限")
                                       : QStringLiteral("无法创建 LLM Provider，使用规则推断象限");
        if (classifyMessage)
            *classifyMessage = msg;
        return ruleFallbackAssignments(pending, QStringLiteral("未配置"), trace);
    }

    const bool isOllama = config.provider == LlmProviderType::Ollama;
    const int batchSize = AiPrompts::quadrantBatchSize(config.provider);
    const int effectiveBatchSize = isOllama ? batchSize : pending.size();
    const int batchCount = (pending.size() + effectiveBatchSize - 1) / effectiveBatchSize;

    if (isOllama && batchCount > 1)
        trace.info("AI", QStringLiteral("Ollama 分批象限分类：%1 批 × 最多 %2 条").arg(batchCount).arg(effectiveBatchSize));

    QVector<QuadrantAssignment> out;
    bool anyLlmSuccess = false;
    int llmBatchCount = 0;

    for (int i = 0; i < pending.size(); i += effectiveBatchSize) {
        const QVector<TaskItem> chunk = pending.mid(i, effectiveBatchSize);
        const int batchIndex = i / effectiveBatchSize;

        bool chunkUsedLlm = false;
        QString chunkMsg;
        const QVector<QuadrantAssignment> chunkResult =
            classifyPendingChunk(provider.get(), config, chunk, batchIndex, batchCount, trace,
                                 &chunkUsedLlm, &chunkMsg);

        if (chunkUsedLlm) {
            anyLlmSuccess = true;
            ++llmBatchCount;
        }
        out += chunkResult;
    }

    if (usedLlm)
        *usedLlm = anyLlmSuccess;
    if (classifyMessage) {
        if (anyLlmSuccess)
            *classifyMessage = batchCount > 1
                ? QStringLiteral("AI 成功划分 %1 项（%2/%3 批走 LLM）").arg(out.size()).arg(llmBatchCount).arg(batchCount)
                : QStringLiteral("AI 成功划分 %1 项").arg(out.size());
        else
            *classifyMessage = QStringLiteral("LLM 全部失败，已用规则推断 %1 项").arg(out.size());
    }
    trace.info("AI", *classifyMessage);
    return out;
}

static PriorityAnalysisResult runAnalysis(LlmConfig config, ScoringWeights weights, QVector<TaskItem> tasks)
{
    AiAnalysisTrace trace;
    PriorityAnalysisResult result;
    result.analysisStartedAt = QDateTime::currentDateTime();

    trace.info("AI", QStringLiteral("========== AI 分析开始 =========="));
    trace.info("AI", QStringLiteral("任务总数 %1").arg(tasks.size()));
    trace.info("LLM",
               QStringLiteral("Provider=%1 Model=%2 URL=%3")
                   .arg(providerName(config.provider), config.model, config.baseUrl));

    result.quadrantAssignments = classifyUnassigned(config, tasks, &result.usedLlmForQuadrants,
                                                  &result.quadrantClassifyMessage, trace);
    for (const QuadrantAssignment &a : result.quadrantAssignments) {
        for (TaskItem &t : tasks) {
            if (t.id == a.taskId)
                t.quadrant = a.quadrant;
        }
    }

    const QVector<TaskItem> scored = PriorityEngine::applyRuleScores(tasks, weights);
    QString scoreLines;
    for (const TaskItem &t : scored) {
        if (t.completed)
            continue;
        scoreLines += QStringLiteral("  id=%1 Q%2 score=%3  %4\n")
                          .arg(t.id)
                          .arg(static_cast<int>(t.quadrant))
                          .arg(t.ruleScore, 0, 'f', 2)
                          .arg(t.title);
    }
    trace.debug("CALC", QStringLiteral("规则得分计算完成"), scoreLines.trimmed());

    const QVector<PriorityRecommendation> ruleTop = PriorityEngine::top3FromRules(scored, weights);
    trace.info("CALC", QStringLiteral("规则层 Top3 候选 %1 条").arg(ruleTop.size()));
    QString ruleTopLines;
    for (const PriorityRecommendation &rec : ruleTop) {
        ruleTopLines += QStringLiteral("  #%1 id=%2 score=%3 %4\n")
                            .arg(rec.rank)
                            .arg(rec.taskId)
                            .arg(rec.score, 0, 'f', 1)
                            .arg(rec.title);
    }
    if (!ruleTopLines.isEmpty())
        trace.debug("CALC", QStringLiteral("规则 Top3 列表"), ruleTopLines.trimmed());

    trace.info("AI", QStringLiteral("【阶段 2/2】Top 3 优先级推荐"));

    auto provider = LlmProviderFactory::create(config);
    if (!provider || !provider->isConfigured()) {
        result.success = true;
        result.usedLlm = false;
        result.top3 = ruleTop;
        result.errorMessage = provider ? QStringLiteral("LLM 未配置，已使用规则层 Top 3")
                                       : QStringLiteral("无法创建 LLM Provider");
        trace.warn("AI", result.errorMessage);
        trace.info("AI", QStringLiteral("========== AI 分析结束 (仅规则层) =========="));
        result.analysisFinishedAt = QDateTime::currentDateTime();
        result.trace = trace.takeEntries();
        AiAnalysisTrace::saveToFile(result);
        return result;
    }

    LlmRequest req;
    req.systemPrompt = AiPrompts::top3SystemPrompt();
    req.userPrompt = PriorityEngine::buildLlmPrompt(scored, 8);
    req.temperature = 0.1;
    req.outputFormat = AiPrompts::top3OutputFormat(config.provider);
    req.maxTokens = AiPrompts::top3MaxTokens(config.provider);

    trace.info("LLM",
               QStringLiteral("发送 Top3 请求 provider=%1 model=%2 maxTokens=%3")
                   .arg(providerName(config.provider), config.model)
                   .arg(req.maxTokens),
               QStringLiteral("System:\n%1\n\nUser:\n%2").arg(req.systemPrompt, req.userPrompt));

    const LlmResponse llm = provider->complete(req);
    trace.info("LLM",
               QStringLiteral("Top3 响应 success=%1 contentLen=%2")
                   .arg(llm.success)
                   .arg(llm.content.size()),
               llm.success ? llm.content : llm.errorMessage);

    if (!llm.success) {
        result.success = true;
        result.usedLlm = false;
        result.top3 = ruleTop;
        result.errorMessage = llm.errorMessage;
        trace.warn("AI", QStringLiteral("Top3 LLM 失败，使用规则层"), llm.errorMessage);
        trace.info("AI", QStringLiteral("========== AI 分析结束 (Top3 规则降级) =========="));
        result.analysisFinishedAt = QDateTime::currentDateTime();
        result.trace = trace.takeEntries();
        AiAnalysisTrace::saveToFile(result);
        return result;
    }

    QHash<qint64, TaskItem> byId;
    for (const TaskItem &t : scored)
        byId.insert(t.id, t);

    QString parseError;
    const QJsonArray arr = LlmJsonUtils::parseArray(LlmJsonUtils::extractPayload(llm.content), &parseError);

    QVector<PriorityRecommendation> llmTop;
    for (const QJsonValue &v : arr) {
        const QJsonObject o = v.toObject();
        const qint64 id = static_cast<qint64>(o.value(QStringLiteral("id")).toDouble());
        if (!byId.contains(id))
            continue;
        PriorityRecommendation rec;
        rec.taskId = id;
        rec.title = byId.value(id).title;
        rec.rank = o.value(QStringLiteral("rank")).toInt();
        rec.score = byId.value(id).ruleScore;
        rec.reason = o.value(QStringLiteral("reason")).toString();
        rec.reason = AiPrompts::sanitizeTop3Reason(rec.reason);
        if (rec.reason.size() < 8)
            rec.reason = QStringLiteral("建议优先处理：") + rec.title;
        llmTop.append(rec);
        trace.info("AI",
                   QStringLiteral("LLM Top3 #%1 id=%2 %3").arg(rec.rank).arg(id).arg(rec.title),
                   rec.reason);
    }

    std::sort(llmTop.begin(), llmTop.end(), [](const PriorityRecommendation &a, const PriorityRecommendation &b) {
        return a.rank < b.rank;
    });

    if (llmTop.isEmpty()) {
        result.success = true;
        result.usedLlm = false;
        result.top3 = ruleTop;
        result.errorMessage = QStringLiteral("LLM Top3 响应无法解析 (%1)，已降级为规则层").arg(parseError);
        trace.warn("AI", result.errorMessage);
        trace.info("AI", QStringLiteral("========== AI 分析结束 (Top3 解析降级) =========="));
        result.analysisFinishedAt = QDateTime::currentDateTime();
        result.trace = trace.takeEntries();
        AiAnalysisTrace::saveToFile(result);
        return result;
    }

    while (llmTop.size() > 3)
        llmTop.removeLast();

    result.success = true;
    result.usedLlm = true;
    result.top3 = llmTop;
    trace.info("AI", QStringLiteral("========== AI 分析结束 (LLM Top3 成功) =========="));
    result.analysisFinishedAt = QDateTime::currentDateTime();
    result.trace = trace.takeEntries();
    AiAnalysisTrace::saveToFile(result);
    return result;
}

LlmService::LlmService(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    qRegisterMetaType<PriorityAnalysisResult>("PriorityAnalysisResult");
    d->watcher = new QFutureWatcher<PriorityAnalysisResult>(this);
    connect(d->watcher, &QFutureWatcher<PriorityAnalysisResult>::finished, this, [this]() {
        d->busy = false;
        emit analysisFinished(d->watcher->result());
    });
}

LlmService::~LlmService()
{
    delete d;
}

void LlmService::setConfig(const LlmConfig &config)
{
    d->config = config;
    LlmProviderFactory::saveToSettings(config);
    emit providerChanged(config.provider);
}

LlmConfig LlmService::config() const
{
    return d->config;
}

void LlmService::setScoringWeights(const ScoringWeights &weights)
{
    d->weights = weights;
}

ScoringWeights LlmService::scoringWeights() const
{
    return d->weights;
}

bool LlmService::isBusy() const
{
    return d->busy;
}

void LlmService::analyzePriorities(const QVector<TaskItem> &tasks)
{
    if (d->busy) {
        AppLogger::warn("AI", QStringLiteral("AI 分析已在进行中，忽略重复请求"));
        return;
    }

    d->busy = true;
    emit analysisStarted();

    LlmConfig cfg = d->config;
    if (cfg.provider == LlmProviderType::Ollama && cfg.timeoutMs < 180000)
        cfg.timeoutMs = 180000;
    const ScoringWeights weights = d->weights;
    QFuture<PriorityAnalysisResult> future = QtConcurrent::run(runAnalysis, cfg, weights, tasks);
    d->watcher->setFuture(future);
}
