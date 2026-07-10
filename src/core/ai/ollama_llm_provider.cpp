#include "ollama_llm_provider.h"

#include "../../utils/app_logger.h"

#include <QElapsedTimer>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>
#include <QRegularExpression>

namespace {

QString stripThinkingFromContent(const QString &text)
{
    QString out = text;
    static const QRegularExpression closedThink(
        QStringLiteral("<think(?:ing)?>[\\s\\S]*?</think(?:ing)?>"),
        QRegularExpression::CaseInsensitiveOption);
    out.remove(closedThink);
    static const QRegularExpression openThink(
        QStringLiteral("<think(?:ing)?>[\\s\\S]*$"),
        QRegularExpression::CaseInsensitiveOption);
    out.remove(openThink);
    return out.trimmed();
}

QString withQwenNoThinkPrefix(const QString &content, const LlmConfig &config)
{
    if (!config.model.contains(QStringLiteral("qwen3"), Qt::CaseInsensitive))
        return content;
    if (content.startsWith(QStringLiteral("/no_think")))
        return content;
    return QStringLiteral("/no_think\n") + content;
}

} // namespace

OllamaLlmProvider::OllamaLlmProvider(QObject *parent)
    : LlmProvider(parent)
{
}

LlmProviderType OllamaLlmProvider::providerType() const
{
    return LlmProviderType::Ollama;
}

QString OllamaLlmProvider::displayName() const
{
    return QStringLiteral("Ollama (本地 Qwen)");
}

bool OllamaLlmProvider::isConfigured() const
{
    return LlmProvider::isConfigured();
}

LlmResponse OllamaLlmProvider::complete(const LlmRequest &request)
{
    LlmResponse response;
    response.provider = LlmProviderType::Ollama;

    if (!isConfigured()) {
        response.errorMessage = QStringLiteral("Ollama 未配置：请检查 baseUrl 与 model");
        AppLogger::warn("LLM", response.errorMessage);
        return response;
    }

    QString userContent = withQwenNoThinkPrefix(request.userPrompt, m_config);

    QJsonArray messages;
    if (!request.systemPrompt.isEmpty()) {
        QJsonObject sys;
        sys.insert(QStringLiteral("role"), QStringLiteral("system"));
        sys.insert(QStringLiteral("content"), withQwenNoThinkPrefix(request.systemPrompt, m_config));
        messages.append(sys);
    }
    QJsonObject user;
    user.insert(QStringLiteral("role"), QStringLiteral("user"));
    user.insert(QStringLiteral("content"), userContent);
    messages.append(user);

    QJsonObject body;
    body.insert(QStringLiteral("model"), m_config.model);
    body.insert(QStringLiteral("messages"), messages);
    body.insert(QStringLiteral("stream"), false);
    body.insert(QStringLiteral("think"), false);
    body.insert(QStringLiteral("keep_alive"), QStringLiteral("10m"));
    if (request.outputFormat == LlmOutputFormat::JsonQuadrantArray) {
        body.insert(QStringLiteral("format"), QJsonObject{
            {QStringLiteral("type"), QStringLiteral("array")},
            {QStringLiteral("items"), QJsonObject{
                {QStringLiteral("type"), QStringLiteral("object")},
                {QStringLiteral("properties"), QJsonObject{
                    {QStringLiteral("id"), QJsonObject{{QStringLiteral("type"), QStringLiteral("integer")}}},
                    {QStringLiteral("quadrant"), QJsonObject{{QStringLiteral("type"), QStringLiteral("integer")}}},
                    {QStringLiteral("reason"), QJsonObject{{QStringLiteral("type"), QStringLiteral("string")}}}
                }},
                {QStringLiteral("required"), QJsonArray{
                    QStringLiteral("id"), QStringLiteral("quadrant"), QStringLiteral("reason")
                }}
            }}
        });
    } else if (request.outputFormat == LlmOutputFormat::JsonTop3Array) {
        body.insert(QStringLiteral("format"), QJsonObject{
            {QStringLiteral("type"), QStringLiteral("array")},
            {QStringLiteral("items"), QJsonObject{
                {QStringLiteral("type"), QStringLiteral("object")},
                {QStringLiteral("properties"), QJsonObject{
                    {QStringLiteral("id"), QJsonObject{{QStringLiteral("type"), QStringLiteral("integer")}}},
                    {QStringLiteral("rank"), QJsonObject{{QStringLiteral("type"), QStringLiteral("integer")}}},
                    {QStringLiteral("reason"), QJsonObject{{QStringLiteral("type"), QStringLiteral("string")}}}
                }},
                {QStringLiteral("required"), QJsonArray{
                    QStringLiteral("id"), QStringLiteral("rank"), QStringLiteral("reason")
                }}
            }}
        });
    } else if (request.outputFormat == LlmOutputFormat::JsonQuickCaptureArray) {
        body.insert(QStringLiteral("format"), QJsonObject{
            {QStringLiteral("type"), QStringLiteral("array")},
            {QStringLiteral("items"), QJsonObject{
                {QStringLiteral("type"), QStringLiteral("object")},
                {QStringLiteral("properties"), QJsonObject{
                    {QStringLiteral("title"), QJsonObject{{QStringLiteral("type"), QStringLiteral("string")}}},
                    {QStringLiteral("due"), QJsonObject{{QStringLiteral("type"), QStringLiteral("string")}}},
                    {QStringLiteral("quadrant"), QJsonObject{{QStringLiteral("type"), QStringLiteral("integer")}}},
                    {QStringLiteral("notes"), QJsonObject{{QStringLiteral("type"), QStringLiteral("string")}}}
                }},
                {QStringLiteral("required"), QJsonArray{
                    QStringLiteral("title"), QStringLiteral("due"), QStringLiteral("quadrant"),
                    QStringLiteral("notes")
                }}
            }}
        });
    }
    body.insert(QStringLiteral("options"), QJsonObject{
        {QStringLiteral("temperature"), request.temperature},
        {QStringLiteral("num_predict"), request.maxTokens}
    });

    const QUrl url(m_config.baseUrl.endsWith('/') ? m_config.baseUrl + QStringLiteral("api/chat")
                                                  : m_config.baseUrl + QStringLiteral("/api/chat"));
    QNetworkRequest netReq(url);
    netReq.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    QNetworkAccessManager nam;
    QEventLoop loop;
    QNetworkReply *reply = nam.post(netReq, QJsonDocument(body).toJson(QJsonDocument::Compact));
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    bool timedOut = false;
    QTimer timer;
    const int timeoutMs = request.timeoutMsOverride > 0 ? request.timeoutMsOverride : m_config.timeoutMs;
    if (timeoutMs > 0) {
        timer.setSingleShot(true);
        QObject::connect(&timer, &QTimer::timeout, &loop, [&]() {
            timedOut = true;
            reply->abort();
        });
        timer.start(timeoutMs);
    }

    QElapsedTimer elapsed;
    elapsed.start();
    loop.exec();

    if (timedOut) {
        response.errorMessage = QStringLiteral("Ollama 请求超时 (%1 ms)，请检查模型是否过大或改用 qwen2.5:3b")
                                  .arg(timeoutMs);
        AppLogger::error("LLM", response.errorMessage);
        reply->deleteLater();
        return response;
    }

    if (reply->error() != QNetworkReply::NoError) {
        response.errorMessage = QStringLiteral("Ollama 连接失败 (%1:%2): %3")
                                    .arg(m_config.baseUrl)
                                    .arg(url.port() > 0 ? url.port() : 11434)
                                    .arg(reply->errorString());
        AppLogger::error("LLM", response.errorMessage);
        reply->deleteLater();
        return response;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();

    const QJsonObject obj = doc.object();
    if (obj.contains(QStringLiteral("error"))) {
        response.errorMessage = QStringLiteral("Ollama 错误: %1")
                                    .arg(obj.value(QStringLiteral("error")).toString());
        AppLogger::error("LLM", response.errorMessage);
        return response;
    }

    const QJsonObject message = obj.value(QStringLiteral("message")).toObject();
    response.content = stripThinkingFromContent(message.value(QStringLiteral("content")).toString().trimmed());
    if (response.content.isEmpty()) {
        const QString thinking = message.value(QStringLiteral("thinking")).toString().trimmed();
        if (!thinking.isEmpty()) {
            response.errorMessage = QStringLiteral(
                "Ollama 返回空内容（模型将 token 用于思考链）。"
                "请更新 ToDoList 至最新版本，或换用 qwen2.5 系列模型。");
        } else {
            const QString reason = obj.value(QStringLiteral("done_reason")).toString();
            response.errorMessage = reason == QStringLiteral("length")
                ? QStringLiteral("Ollama 返回空内容（输出 token 不足，请增大 maxTokens 或换更小模型）")
                : QStringLiteral("Ollama 返回空内容");
        }
        AppLogger::warn("LLM", response.errorMessage);
        return response;
    }

    response.success = true;
    AppLogger::debug("LLM",
                     QStringLiteral("Ollama OK, %1 chars, %2 ms, format=%3")
                         .arg(response.content.size())
                         .arg(elapsed.elapsed())
                         .arg(static_cast<int>(request.outputFormat)));
    return response;
}
