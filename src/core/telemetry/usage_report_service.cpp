#include "usage_report_service.h"

#include "usage_report_config.h"
#include "../app_settings.h"
#include "../../utils/app_logger.h"
#include "../../utils/app_version.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSysInfo>
#include <QUrl>

UsageReportService::UsageReportService(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
{
}

bool UsageReportService::isActive() const
{
    return AppSettings::usageStatisticsEnabled() && UsageReportConfigStore::load().isUsable();
}

QString UsageReportService::eventName(Event event) const
{
    switch (event) {
    case Event::AppStart: return QStringLiteral("app_start");
    case Event::Heartbeat: return QStringLiteral("heartbeat");
    case Event::AppExit: return QStringLiteral("app_exit");
    case Event::UpgradeSuccess: return QStringLiteral("upgrade_success");
    }
    return QStringLiteral("unknown");
}

void UsageReportService::report(Event event, const QString &extraJson)
{
    if (!AppSettings::usageStatisticsEnabled())
        return;

    const UsageEndpointConfig config = UsageReportConfigStore::load();
    if (!config.isUsable())
        return;

    QJsonObject payload;
    payload.insert(QStringLiteral("event"), eventName(event));
    payload.insert(QStringLiteral("install_id"), AppSettings::installId());
    payload.insert(QStringLiteral("version"), AppVersion::versionString());
    payload.insert(QStringLiteral("build"), AppVersion::buildNumber());
    payload.insert(QStringLiteral("os"), QSysInfo::prettyProductName());
    payload.insert(QStringLiteral("locale"), QLocale::system().name());
    payload.insert(QStringLiteral("channel"), QStringLiteral("stable"));
    payload.insert(QStringLiteral("ts"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

    if (!extraJson.isEmpty()) {
        const QJsonDocument extra = QJsonDocument::fromJson(extraJson.toUtf8());
        if (extra.isObject()) {
            const QJsonObject extraObj = extra.object();
            for (auto it = extraObj.begin(); it != extraObj.end(); ++it)
                payload.insert(it.key(), it.value());
        }
    }

    sendPayload(payload);

    if (event == Event::Heartbeat)
        AppSettings::setLastUsageHeartbeat(QDateTime::currentDateTimeUtc());
}

void UsageReportService::sendPayload(const QJsonObject &payload)
{
    const UsageEndpointConfig config = UsageReportConfigStore::load();
    if (!config.isUsable())
        return;

    const QUrl url(config.endpoint);
    if (!url.isValid()) {
        AppLogger::debug("USAGE", QStringLiteral("无效的上报地址: %1").arg(config.endpoint));
        return;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      QStringLiteral("ToDoList/%1").arg(AppVersion::displayString()));
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    request.setTransferTimeout(15000);
#endif

    const QByteArray body = QJsonDocument(payload).toJson(QJsonDocument::Compact);
    AppLogger::debug("USAGE",
                     QStringLiteral("上报 %1 -> %2").arg(payload.value(QStringLiteral("event")).toString(),
                                                         config.endpoint));

    QNetworkReply *reply = m_network->post(request, body);
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            AppLogger::debug("USAGE",
                             QStringLiteral("上报失败: %1").arg(reply->errorString()));
        }
        reply->deleteLater();
    });
}

void UsageReportService::reportAppStart()
{
    report(Event::AppStart);
}

void UsageReportService::reportHeartbeatIfDue()
{
    if (!AppSettings::usageStatisticsEnabled())
        return;

    const UsageEndpointConfig config = UsageReportConfigStore::load();
    if (!config.isUsable())
        return;

    const QDateTime last = AppSettings::lastUsageHeartbeat();
    if (last.isValid()) {
        const qint64 hours = last.secsTo(QDateTime::currentDateTimeUtc()) / 3600;
        if (hours < config.heartbeatHours)
            return;
    }

    report(Event::Heartbeat);
}

void UsageReportService::reportAppExit()
{
    report(Event::AppExit);
}

void UsageReportService::reportUpgradeSuccess(const QString &fromVersion)
{
    QJsonObject extra;
    extra.insert(QStringLiteral("from_version"), fromVersion);
    report(Event::UpgradeSuccess, QString::fromUtf8(QJsonDocument(extra).toJson(QJsonDocument::Compact)));
}
