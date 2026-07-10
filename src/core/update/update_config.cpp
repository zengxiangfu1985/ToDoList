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
        "https://raw.githubusercontent.com/zengxiangfu1985/ToDoList/main/dist/update.json");
}

QString fallbackManifestUrl()
{
    return QStringLiteral(
        "https://cdn.jsdelivr.net/gh/zengxiangfu1985/ToDoList@main/dist/update.json");
}

QString ghProxyManifestUrl()
{
    return QStringLiteral(
        "https://ghproxy.net/https://raw.githubusercontent.com/zengxiangfu1985/ToDoList/main/dist/update.json");
}

QString ghProxyMirrorManifestUrl()
{
    return QStringLiteral(
        "https://mirror.ghproxy.com/https://raw.githubusercontent.com/zengxiangfu1985/ToDoList/main/dist/update.json");
}

QString releaseLatestManifestUrl()
{
    return QStringLiteral(
        "https://github.com/zengxiangfu1985/ToDoList/releases/latest/download/update.json");
}

UpdateConfig defaultConfig()
{
    UpdateConfig config;
    UpdateSource ghProxy;
    ghProxy.name = QStringLiteral("ghproxy");
    ghProxy.type = QStringLiteral("static_json");
    ghProxy.url = ghProxyManifestUrl();
    config.sources.append(ghProxy);

    UpdateSource release;
    release.name = QStringLiteral("Release");
    release.type = QStringLiteral("static_json");
    release.url = releaseLatestManifestUrl();
    config.sources.append(release);

    UpdateSource jsDelivr;
    jsDelivr.name = QStringLiteral("jsDelivr");
    jsDelivr.type = QStringLiteral("static_json");
    jsDelivr.url = fallbackManifestUrl();
    config.sources.append(jsDelivr);

    UpdateSource github;
    github.name = QStringLiteral("GitHub");
    github.type = QStringLiteral("static_json");
    github.url = defaultManifestUrl();
    config.sources.append(github);

    config.activeSource = release.name;
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
    const QStringList urls = manifestUrls();
    return urls.isEmpty() ? defaultManifestUrl() : urls.first();
}

QStringList UpdateConfigStore::manifestUrls()
{
    const UpdateConfig config = load();
    QStringList urls;
    auto appendUnique = [&](const QString &url) {
        if (!url.isEmpty() && !urls.contains(url))
            urls.append(url);
    };
    auto isJsDelivr = [](const QString &url) {
        return url.contains(QStringLiteral("cdn.jsdelivr.net"));
    };

    appendUnique(releaseLatestManifestUrl());
    appendUnique(ghProxyManifestUrl());
    appendUnique(ghProxyMirrorManifestUrl());
    appendUnique(defaultManifestUrl());
    appendUnique(fallbackManifestUrl());
    for (const UpdateSource &source : config.sources) {
        if (isJsDelivr(source.url))
            appendUnique(source.url);
    }
    for (const UpdateSource &source : config.sources) {
        if (source.name == config.activeSource)
            appendUnique(source.url);
    }
    for (const UpdateSource &source : config.sources)
        appendUnique(source.url);

    return urls;
}
