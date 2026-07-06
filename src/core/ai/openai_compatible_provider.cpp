#include "openai_compatible_provider.h"

#include "../../utils/app_logger.h"
#include "../../utils/ssl_support.h"

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>

OpenAiCompatibleProvider::OpenAiCompatibleProvider(QObject *parent)
    : LlmProvider(parent)
{
}

LlmProviderType OpenAiCompatibleProvider::providerType() const
{
    return m_config.provider;
}

QString OpenAiCompatibleProvider::displayName() const
{
    switch (m_config.provider) {
    case LlmProviderType::DeepSeek: return QStringLiteral("DeepSeek");
    case LlmProviderType::Kimi: return QStringLiteral("Kimi (Moonshot)");
    case LlmProviderType::CustomOpenAI: return QStringLiteral("Custom OpenAI");
    default: return QStringLiteral("OpenAI Compatible");
    }
}

bool OpenAiCompatibleProvider::isConfigured() const
{
    return LlmProvider::isConfigured() && !m_config.apiKey.trimmed().isEmpty();
}

LlmResponse OpenAiCompatibleProvider::complete(const LlmRequest &request)
{
    LlmResponse response;
    response.provider = m_config.provider;

    if (!isConfigured()) {
        response.errorMessage = QStringLiteral("%1 未配置 API Key").arg(displayName());
        AppLogger::warn("LLM", response.errorMessage);
        return response;
    }

    if (!SslSupport::isAvailable()) {
        response.errorMessage = SslSupport::unavailableMessage();
        AppLogger::error("LLM", response.errorMessage);
        return response;
    }

    QJsonArray messages;
    if (!request.systemPrompt.isEmpty()) {
        QJsonObject sys;
        sys.insert(QStringLiteral("role"), QStringLiteral("system"));
        sys.insert(QStringLiteral("content"), request.systemPrompt);
        messages.append(sys);
    }
    QJsonObject user;
    user.insert(QStringLiteral("role"), QStringLiteral("user"));
    user.insert(QStringLiteral("content"), request.userPrompt);
    messages.append(user);

    QJsonObject body;
    body.insert(QStringLiteral("model"), m_config.model);
    body.insert(QStringLiteral("messages"), messages);
    body.insert(QStringLiteral("temperature"), request.temperature);
    body.insert(QStringLiteral("max_tokens"), request.maxTokens);

    QString base = m_config.baseUrl;
    while (base.endsWith('/'))
        base.chop(1);
    const QUrl url(base + QStringLiteral("/chat/completions"));

    QNetworkRequest netReq(url);
    netReq.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    netReq.setRawHeader("Authorization", QByteArray("Bearer ") + m_config.apiKey.toUtf8());

    QNetworkAccessManager nam;
    QEventLoop loop;
    QNetworkReply *reply = nam.post(netReq, QJsonDocument(body).toJson(QJsonDocument::Compact));
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(m_config.timeoutMs > 0 ? m_config.timeoutMs : 30000);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        response.errorMessage = QStringLiteral("%1 API 错误: %2").arg(displayName(), reply->errorString());
        AppLogger::error("LLM", response.errorMessage);
        reply->deleteLater();
        return response;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();

    const QJsonArray choices = doc.object().value(QStringLiteral("choices")).toArray();
    if (choices.isEmpty()) {
        response.errorMessage = QStringLiteral("%1 返回无 choices").arg(displayName());
        AppLogger::warn("LLM", response.errorMessage);
        return response;
    }
    const QJsonObject message = choices.first().toObject().value(QStringLiteral("message")).toObject();
    response.content = message.value(QStringLiteral("content")).toString();
    response.success = !response.content.isEmpty();
    if (!response.success) {
        response.errorMessage = QStringLiteral("%1 返回空内容").arg(displayName());
        AppLogger::warn("LLM", response.errorMessage);
    } else {
        AppLogger::debug("LLM", QStringLiteral("%1 OK, %2 chars").arg(displayName()).arg(response.content.size()));
    }
    return response;
}
