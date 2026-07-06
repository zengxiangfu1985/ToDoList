#include "m365_auth_service.h"
#include "../../utils/portable_settings.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QSettings>
#include <QUrlQuery>

M365AuthService::M365AuthService(QObject *parent)
    : QObject(parent)
    , m_config(loadFromSettings())
{
}

M365Config M365AuthService::config() const
{
    return m_config;
}

void M365AuthService::setConfig(const M365Config &config)
{
    m_config = config;
}

M365Config M365AuthService::loadFromSettings()
{
    QSettings s = PortableSettings::open();
    M365Config cfg;
    cfg.tenantId = s.value(QStringLiteral("m365/tenantId"), QStringLiteral("common")).toString();
    cfg.clientId = s.value(QStringLiteral("m365/clientId")).toString();
    cfg.accessToken = s.value(QStringLiteral("m365/accessToken")).toString();
    cfg.refreshToken = s.value(QStringLiteral("m365/refreshToken")).toString();
    cfg.tokenExpiry = s.value(QStringLiteral("m365/tokenExpiry")).toDateTime();
    return cfg;
}

void M365AuthService::saveToSettings(const M365Config &config)
{
    QSettings s = PortableSettings::open();
    s.setValue(QStringLiteral("m365/tenantId"), config.tenantId);
    s.setValue(QStringLiteral("m365/clientId"), config.clientId);
    s.setValue(QStringLiteral("m365/accessToken"), config.accessToken);
    s.setValue(QStringLiteral("m365/refreshToken"), config.refreshToken);
    s.setValue(QStringLiteral("m365/tokenExpiry"), config.tokenExpiry);
}

static QJsonObject postFormSync(const QUrl &url, const QUrlQuery &query, QString *errorMsg)
{
    QNetworkAccessManager nam;
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));

    QEventLoop loop;
    QNetworkReply *reply = nam.post(req, query.toString(QUrl::FullyEncoded).toUtf8());
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    const QByteArray body = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        if (errorMsg)
            *errorMsg = reply->errorString() + QStringLiteral(": ") + QString::fromUtf8(body);
        reply->deleteLater();
        return {};
    }
    reply->deleteLater();
    return QJsonDocument::fromJson(body).object();
}

bool M365AuthService::startDeviceCodeFlow(QString *errorMsg)
{
    if (m_config.clientId.trimmed().isEmpty()) {
        if (errorMsg)
            *errorMsg = QStringLiteral("请先填写 Azure 应用程序 Client ID");
        return false;
    }

    const QUrl url(QStringLiteral("https://login.microsoftonline.com/%1/oauth2/v2.0/devicecode")
                     .arg(m_config.tenantId));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("client_id"), m_config.clientId);
    query.addQueryItem(QStringLiteral("scope"), QStringLiteral("User.Read Mail.Read offline_access"));

    const QJsonObject obj = postFormSync(url, query, errorMsg);
    if (obj.isEmpty())
        return false;

    m_deviceCode.verificationUri = obj.value(QStringLiteral("verification_uri")).toString();
    m_deviceCode.userCode = obj.value(QStringLiteral("user_code")).toString();
    m_deviceCode.deviceCode = obj.value(QStringLiteral("device_code")).toString();
    m_deviceCode.expiresIn = obj.value(QStringLiteral("expires_in")).toInt();
    m_deviceCode.interval = obj.value(QStringLiteral("interval")).toInt(5);

    emit deviceCodeReady(m_deviceCode.verificationUri, m_deviceCode.userCode);
    return true;
}

bool M365AuthService::pollAccessToken(QString *errorMsg)
{
    if (m_deviceCode.deviceCode.isEmpty()) {
        if (errorMsg)
            *errorMsg = QStringLiteral("请先启动设备码登录");
        return false;
    }

    const QUrl url(QStringLiteral("https://login.microsoftonline.com/%1/oauth2/v2.0/token")
                     .arg(m_config.tenantId));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("grant_type"), QStringLiteral("urn:ietf:params:oauth:grant-type:device_code"));
    query.addQueryItem(QStringLiteral("client_id"), m_config.clientId);
    query.addQueryItem(QStringLiteral("device_code"), m_deviceCode.deviceCode);

    const QJsonObject obj = postFormSync(url, query, errorMsg);
    if (obj.contains(QStringLiteral("error"))) {
        const QString err = obj.value(QStringLiteral("error")).toString();
        if (err == QStringLiteral("authorization_pending"))
            return false;
        if (errorMsg)
            *errorMsg = obj.value(QStringLiteral("error_description")).toString();
        emit authFailed(errorMsg ? *errorMsg : err);
        return false;
    }

    m_config.accessToken = obj.value(QStringLiteral("access_token")).toString();
    m_config.refreshToken = obj.value(QStringLiteral("refresh_token")).toString();
    const int expiresIn = obj.value(QStringLiteral("expires_in")).toInt(3600);
    m_config.tokenExpiry = QDateTime::currentDateTimeUtc().addSecs(expiresIn);
    saveToSettings(m_config);
    emit authenticated();
    return true;
}

bool M365AuthService::refreshAccessToken(QString *errorMsg)
{
    if (m_config.refreshToken.isEmpty()) {
        if (errorMsg)
            *errorMsg = QStringLiteral("无 refresh_token，请重新登录");
        return false;
    }

    const QUrl url(QStringLiteral("https://login.microsoftonline.com/%1/oauth2/v2.0/token")
                     .arg(m_config.tenantId));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("grant_type"), QStringLiteral("refresh_token"));
    query.addQueryItem(QStringLiteral("client_id"), m_config.clientId);
    query.addQueryItem(QStringLiteral("refresh_token"), m_config.refreshToken);
    query.addQueryItem(QStringLiteral("scope"), QStringLiteral("User.Read Mail.Read offline_access"));

    const QJsonObject obj = postFormSync(url, query, errorMsg);
    if (obj.isEmpty() || !obj.contains(QStringLiteral("access_token")))
        return false;

    m_config.accessToken = obj.value(QStringLiteral("access_token")).toString();
    if (obj.contains(QStringLiteral("refresh_token")))
        m_config.refreshToken = obj.value(QStringLiteral("refresh_token")).toString();
    m_config.tokenExpiry = QDateTime::currentDateTimeUtc().addSecs(obj.value(QStringLiteral("expires_in")).toInt(3600));
    saveToSettings(m_config);
    return true;
}
