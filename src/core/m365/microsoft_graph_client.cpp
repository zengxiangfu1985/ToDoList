#include "microsoft_graph_client.h"
#include "m365_auth_service.h"

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

MicrosoftGraphClient::MicrosoftGraphClient(M365AuthService *auth, QObject *parent)
    : QObject(parent)
    , m_auth(auth)
{
}

bool MicrosoftGraphClient::ensureToken(QString *errorMsg)
{
    if (!m_auth)
        return false;

    M365Config cfg = m_auth->config();
    if (!cfg.isConfigured()) {
        if (errorMsg)
            *errorMsg = QStringLiteral("Microsoft 365 未登录");
        return false;
    }

    if (cfg.tokenExpiry.isValid() && cfg.tokenExpiry <= QDateTime::currentDateTimeUtc().addSecs(60)) {
        if (!m_auth->refreshAccessToken(errorMsg))
            return false;
    }
    return true;
}

QVector<GraphMailItem> MicrosoftGraphClient::fetchFlaggedMessages(QString *errorMsg)
{
    QVector<GraphMailItem> items;
    if (!ensureToken(errorMsg))
        return items;

    const M365Config cfg = m_auth->config();
    const QUrl url(QStringLiteral("https://graph.microsoft.com/v1.0/me/messages?"
                                  "$filter=flag/flagStatus eq 'flagged'&$top=25&"
                                  "$select=id,subject,receivedDateTime,flag"));

    QNetworkRequest req(url);
    req.setRawHeader("Authorization", QByteArray("Bearer ") + cfg.accessToken.toUtf8());

    QNetworkAccessManager nam;
    QEventLoop loop;
    QNetworkReply *reply = nam.get(req);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    const QByteArray body = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        if (errorMsg)
            *errorMsg = reply->errorString() + QStringLiteral(": ") + QString::fromUtf8(body);
        reply->deleteLater();
        return items;
    }
    reply->deleteLater();

    const QJsonArray values = QJsonDocument::fromJson(body).object().value(QStringLiteral("value")).toArray();
    for (const QJsonValue &v : values) {
        const QJsonObject o = v.toObject();
        GraphMailItem mail;
        mail.id = o.value(QStringLiteral("id")).toString();
        mail.subject = o.value(QStringLiteral("subject")).toString();
        mail.receivedAt = QDateTime::fromString(o.value(QStringLiteral("receivedDateTime")).toString(), Qt::ISODate);
        mail.flagged = o.value(QStringLiteral("flag")).toObject().value(QStringLiteral("flagStatus")).toString()
                       == QStringLiteral("flagged");
        if (!mail.subject.isEmpty())
            items.append(mail);
    }
    return items;
}
