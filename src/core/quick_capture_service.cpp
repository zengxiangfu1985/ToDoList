#include "quick_capture_service.h"

#include "ai/ai_prompts.h"
#include "ai/llm_provider.h"
#include "task_repository.h"
#include "../utils/app_logger.h"
#include "../utils/llm_json_utils.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QTime>

namespace {

EisenhowerQuadrant quadrantFromHint(int value)
{
    if (value >= 1 && value <= 4)
        return static_cast<EisenhowerQuadrant>(value);
    return EisenhowerQuadrant::Unassigned;
}

QDateTime defaultDueTodayEnd()
{
    return QDateTime(QDate::currentDate(), QTime(23, 59));
}

QDateTime parseDueValue(const QString &raw)
{
    const QString trimmed = raw.trimmed();
    if (trimmed.isEmpty())
        return defaultDueTodayEnd();

    QDateTime dt = QDateTime::fromString(trimmed, Qt::ISODateWithMs);
    if (!dt.isValid())
        dt = QDateTime::fromString(trimmed, Qt::ISODate);
    if (!dt.isValid()) {
        const QDate date = QDate::fromString(trimmed, Qt::ISODate);
        if (date.isValid())
            dt = QDateTime(date, QTime(23, 59));
    }
    if (!dt.isValid())
        return defaultDueTodayEnd();
    if (dt.time() == QTime(0, 0))
        dt.setTime(QTime(23, 59));
    return dt;
}

QStringList splitRuleLines(const QString &text)
{
    QString normalized = text;
    normalized.replace(QChar(0xFF1B), QLatin1Char(';'));
    normalized.replace(QChar(0xFF0C), QLatin1Char(','));
    normalized.replace(QChar(0x3001), QLatin1Char(','));
    normalized.replace(QLatin1Char('\r'), QLatin1Char('\n'));

    QStringList lines;
    const QStringList chunks = normalized.split(
        QRegularExpression(QStringLiteral("[\\n;,；，、]+")), Qt::SkipEmptyParts);
    for (const QString &chunk : chunks) {
        QString line = chunk.trimmed();
        if (line.isEmpty())
            continue;
        static const QRegularExpression stepRe(
            QStringLiteral("(?:之后|然后|接着|再|并|并且|以及)(?=[\\u4e00-\\u9fff])"));
        const QStringList subSteps = line.split(stepRe, Qt::SkipEmptyParts);
        if (subSteps.size() > 1) {
            for (const QString &step : subSteps) {
                const QString trimmed = step.trimmed();
                if (!trimmed.isEmpty())
                    lines.append(trimmed);
            }
        } else {
            lines.append(line);
        }
    }
    return lines;
}

} // namespace

QuickCaptureParseResult QuickCaptureService::parseWithRules(const QString &text)
{
    QuickCaptureParseResult result;
    result.success = true;
    result.usedLlm = false;

    const QStringList lines = splitRuleLines(text);
    if (lines.isEmpty()) {
        result.success = false;
        result.errorMessage = QStringLiteral("没有可解析的任务内容");
        return result;
    }

    const QDateTime dueDefault = defaultDueTodayEnd();
    for (const QString &line : lines) {
        QuickCaptureDraft draft;
        draft.title = line;
        draft.dueAt = dueDefault;
        draft.quadrant = EisenhowerQuadrant::Unassigned;
        result.tasks.append(draft);
    }
    return result;
}

QuickCaptureParseResult QuickCaptureService::parse(const QString &text, const LlmConfig &config)
{
    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) {
        QuickCaptureParseResult empty;
        empty.errorMessage = QStringLiteral("输入内容为空");
        return empty;
    }

    auto provider = LlmProviderFactory::create(config);
    if (!provider || !provider->isConfigured()) {
        AppLogger::debug("QUICK_CAPTURE", QStringLiteral("LLM 未配置，使用规则拆分"));
        return parseWithRules(trimmed);
    }

    LlmRequest request;
    request.systemPrompt = AiPrompts::quickCaptureSystemPrompt();
    request.userPrompt = AiPrompts::quickCaptureUserPrompt(trimmed);
    request.temperature = 0.2;
    request.maxTokens = AiPrompts::quickCaptureMaxTokens(config.provider);
    request.outputFormat = AiPrompts::quickCaptureOutputFormat(config.provider);
    request.timeoutMsOverride = AiPrompts::quickCaptureTimeoutMs(config.provider);

    AppLogger::debug("QUICK_CAPTURE",
                     QStringLiteral("发送 LLM 解析 provider=%1 model=%2 maxTokens=%3 timeout=%4ms")
                         .arg(static_cast<int>(config.provider))
                         .arg(config.model)
                         .arg(request.maxTokens)
                         .arg(request.timeoutMsOverride));

    const LlmResponse llm = provider->complete(request);
    if (!llm.success) {
        AppLogger::warn("QUICK_CAPTURE",
                        QStringLiteral("LLM 解析失败，降级规则拆分: %1").arg(llm.errorMessage));
        QuickCaptureParseResult fallback = parseWithRules(trimmed);
        if (!fallback.tasks.isEmpty())
            fallback.errorMessage = llm.errorMessage;
        return fallback;
    }

    AppLogger::debug("QUICK_CAPTURE",
                     QStringLiteral("LLM 响应 %1 chars: %2")
                         .arg(llm.content.size())
                         .arg(llm.content.left(500)));

    const QByteArray jsonBytes = LlmJsonUtils::extractPayload(llm.content);
    QString parseError;
    const QJsonArray arr = LlmJsonUtils::parseArray(jsonBytes, &parseError);
    if (arr.isEmpty()) {
        AppLogger::warn("QUICK_CAPTURE",
                        QStringLiteral("JSON 解析失败，降级规则拆分: %1 | raw=%2")
                            .arg(parseError, llm.content.left(300)));
        return parseWithRules(trimmed);
    }

    QuickCaptureParseResult result;
    result.success = true;
    result.usedLlm = true;

    for (const QJsonValue &value : arr) {
        const QJsonObject obj = value.toObject();
        const QString title = obj.value(QStringLiteral("title")).toString().trimmed();
        if (title.isEmpty())
            continue;

        QuickCaptureDraft draft;
        draft.title = title;
        draft.dueAt = parseDueValue(obj.value(QStringLiteral("due")).toString());
        draft.quadrant = quadrantFromHint(obj.value(QStringLiteral("quadrant")).toInt());
        draft.notes = obj.value(QStringLiteral("notes")).toString().trimmed();
        result.tasks.append(draft);
    }

    if (result.tasks.isEmpty())
        return parseWithRules(trimmed);

    return result;
}

int QuickCaptureService::saveTasks(TaskRepository *repo, const QVector<QuickCaptureDraft> &drafts,
                                   QString *errorMessage)
{
    if (!repo)
        return 0;

    int saved = 0;
    for (const QuickCaptureDraft &draft : drafts) {
        TaskItem task;
        task.title = draft.title;
        task.dueAt = draft.dueAt;
        task.quadrant = draft.quadrant;
        task.completed = false;

        QString err;
        if (!repo->addTask(&task, &err)) {
            if (errorMessage)
                *errorMessage = err;
            break;
        }
        ++saved;
    }

    if (saved > 1) {
        AppLogger::debug("QUICK_CAPTURE", QStringLiteral("批量保存 %1 条（多次 tasksChanged）").arg(saved));
    }
    return saved;
}
