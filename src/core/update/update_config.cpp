#include "update_config.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace {

QString defaultManifestUrl()
{
    return QStringLiteral(
        "https://raw.githubusercontent.com/example/ToDoList/main/dist/update.json");
}

UpdateConfig defaultConfig()
{
    UpdateConfig config;
    UpdateSource source;
    source.name = QStringLiteral("GitHub");
    source.type = QStringLiteral("static_json");
    source.url = defaultManifestUrl();
    config.sources.append(source);
    config.activeSource = source.name;
    config.checkOnStartup = true;
    config.checkIntervalHours = 168;
    return config;
}

UpdateConfig parseConfigObject(const QJsonObject &obj)
{
    UpdateConfig config;
    config.activeSource = obj.value(QStringLiteral("active_source")).toString();
    config.checkOnStartup = obj.value(QStringLiteral("check_on_startup")).toBool(true);
    config.checkIntervalHours = obj.value(QStringLiteral("check_interval_hours")).toInt(168);

    const QJsonArray sources = obj.value(QStringLiteral("sources")).toArray();
    for (const QJsonValue &value : sources) {
        const QJsonObject sourceObj = value.toObject();
        UpdateSource source;
        source.name = sourceObj.value(QStringLiteral("name")).toString();
        source.type = sourceObj.value(QStringLiteral("type")).toString();
        source.url = sourceObj.value(QStringLiteral("url")).toString();
        if (!source.name.isEmpty() && !source.url.isEmpty())
            config.sources.append(source);
    }
    if (config.sources.isEmpty())
        return defaultConfig();
    if (config.activeSource.isEmpty())
        config.activeSource = config.sources.first().name;
    return config;
}

} // namespace

QString UpdateConfigStore::configFilePath()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString dataConfig = QDir(appDir).filePath(QStringLiteral("data/update-config.json"));
    if (QFile::exists(dataConfig))
        return dataConfig;
    return QDir(appDir).filePath(QStringLiteral("update-config.json"));
}

UpdateConfig UpdateConfigStore::load()
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

QString UpdateConfigStore::activeManifestUrl()
{
    const UpdateConfig config = load();
    for (const UpdateSource &source : config.sources) {
        if (source.name == config.activeSource)
            return source.url;
    }
    if (!config.sources.isEmpty())
        return config.sources.first().url;
    return defaultManifestUrl();
}
