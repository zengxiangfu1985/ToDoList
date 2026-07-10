#include "usage_report_config.h"

#include "../task_archive.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace {

UsageEndpointConfig defaultConfig()
{
    UsageEndpointConfig config;
    config.enabled = true;
    config.endpoint = QString();
    config.heartbeatHours = 24;
    return config;
}

UsageEndpointConfig parseConfigObject(const QJsonObject &obj)
{
    UsageEndpointConfig config = defaultConfig();
    if (obj.contains(QStringLiteral("enabled")))
        config.enabled = obj.value(QStringLiteral("enabled")).toBool(true);
    config.endpoint = obj.value(QStringLiteral("endpoint")).toString().trimmed();
    config.heartbeatHours = obj.value(QStringLiteral("heartbeat_hours")).toInt(24);
    if (config.heartbeatHours < 1)
        config.heartbeatHours = 24;
    return config;
}

} // namespace

bool UsageEndpointConfig::isUsable() const
{
    return enabled && !endpoint.isEmpty();
}

QString UsageReportConfigStore::configFilePath()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString dataConfig = QDir(appDir).filePath(QStringLiteral("data/usage-endpoint.json"));
    if (QFile::exists(dataConfig))
        return dataConfig;
    return QDir(appDir).filePath(QStringLiteral("usage-endpoint.json"));
}

UsageEndpointConfig UsageReportConfigStore::load()
{
    const QString path = configFilePath();
    if (!QFile::exists(path))
        return defaultConfig();

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return defaultConfig();

    const QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
    if (obj.isEmpty())
        return defaultConfig();
    return parseConfigObject(obj);
}
