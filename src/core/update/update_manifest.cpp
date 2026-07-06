#include "update_manifest.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

namespace {

QString appRootDir()
{
    return QCoreApplication::applicationDirPath();
}

UpdatePackageInfo parseLatestObject(const QJsonObject &latest, QString *error,
                                    bool requireSha256)
{
    UpdatePackageInfo info;
    if (latest.isEmpty()) {
        if (error)
            *error = QStringLiteral("更新清单缺少 latest 节点");
        return info;
    }

    info.version = latest.value(QStringLiteral("version")).toString();
    info.build = latest.value(QStringLiteral("build")).toInt();
    info.minAppVersion = latest.value(QStringLiteral("min_app_version")).toString();
    info.releaseNotes = latest.value(QStringLiteral("release_notes")).toString();
    info.mandatory = latest.value(QStringLiteral("mandatory")).toBool(false);
    info.dbSchema = latest.value(QStringLiteral("db_schema")).toInt();

    const QJsonObject package = latest.value(QStringLiteral("package")).toObject();
    info.fileName = package.value(QStringLiteral("file_name")).toString();
    info.size = package.value(QStringLiteral("size")).toVariant().toLongLong();
    info.sha256 = package.value(QStringLiteral("sha256")).toString().toLower();
    info.url = package.value(QStringLiteral("url")).toString();
    const QJsonArray mirrors = package.value(QStringLiteral("mirrors")).toArray();
    for (const QJsonValue &mirrorValue : mirrors) {
        const QString mirrorUrl = mirrorValue.toString();
        if (!mirrorUrl.isEmpty() && !info.mirrorUrls.contains(mirrorUrl))
            info.mirrorUrls.append(mirrorUrl);
    }

    info.valid = !info.version.isEmpty() && info.build > 0
                 && (!requireSha256 || !info.sha256.isEmpty());
    if (!info.valid && error)
        *error = QStringLiteral("更新清单字段不完整");
    return info;
}

} // namespace

UpdatePackageInfo UpdateManifest::parseRemoteJson(const QByteArray &bytes, QString *error)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject()) {
        if (error)
            *error = QStringLiteral("更新清单不是有效 JSON");
        return {};
    }
    return parseLatestObject(doc.object().value(QStringLiteral("latest")).toObject(), error,
                             true);
}

UpdatePackageInfo UpdateManifest::parsePackageJson(const QByteArray &bytes, QString *error)
{
    const QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isObject()) {
        if (error)
            *error = QStringLiteral("离线包 manifest 不是有效 JSON");
        return {};
    }
    return parseLatestObject(doc.object(), error, false);
}

UpdatePackageInfo UpdateManifest::readPackageJsonFile(const QString &path, QString *error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error)
            *error = QStringLiteral("无法读取 manifest: %1").arg(path);
        return {};
    }
    return parsePackageJson(file.readAll(), error);
}

QString UpdateManifest::pendingFilePath()
{
    return QDir(appRootDir()).filePath(QStringLiteral("update.pending.json"));
}

bool UpdateManifest::writePendingFile(const UpdatePendingInfo &info, QString *error)
{
    QJsonObject obj;
    obj.insert(QStringLiteral("install_dir"), info.installDir);
    obj.insert(QStringLiteral("zip_path"), info.zipPath);
    obj.insert(QStringLiteral("target_version"), info.targetVersion);
    obj.insert(QStringLiteral("target_build"), info.targetBuild);
    obj.insert(QStringLiteral("main_pid"), static_cast<double>(info.mainPid));
    obj.insert(QStringLiteral("restart"), info.restart);

    QFile file(pendingFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error)
            *error = QStringLiteral("无法写入 pending 文件");
        return false;
    }
    file.write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    return true;
}

UpdatePendingInfo UpdateManifest::readPendingFile(QString *error)
{
    UpdatePendingInfo info;
    QFile file(pendingFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        if (error)
            *error = QStringLiteral("找不到 pending 文件");
        return info;
    }

    const QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
    info.installDir = obj.value(QStringLiteral("install_dir")).toString();
    info.zipPath = obj.value(QStringLiteral("zip_path")).toString();
    info.targetVersion = obj.value(QStringLiteral("target_version")).toString();
    info.targetBuild = obj.value(QStringLiteral("target_build")).toInt();
    info.mainPid = static_cast<qint64>(obj.value(QStringLiteral("main_pid")).toDouble());
    info.restart = obj.value(QStringLiteral("restart")).toBool(true);
    info.valid = !info.installDir.isEmpty() && !info.zipPath.isEmpty();
    if (!info.valid && error)
        *error = QStringLiteral("pending 文件内容无效");
    return info;
}

QStringList UpdateManifest::readFileManifest(const QString &path, QString *error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error)
            *error = QStringLiteral("无法读取 file-manifest: %1").arg(path);
        return {};
    }

    QStringList lines;
    while (!file.atEnd()) {
        const QString line = QString::fromUtf8(file.readLine()).trimmed();
        if (line.isEmpty() || line.startsWith(QLatin1Char('#')))
            continue;
        lines.append(line);
    }
    return lines;
}

QStringList UpdateManifest::readFileManifestFromZipRoot(const QString &extractedRoot, QString *error)
{
    const QString manifestPath = QDir(extractedRoot).filePath(QStringLiteral("file-manifest.txt"));
    if (QFile::exists(manifestPath))
        return readFileManifest(manifestPath, error);

    QStringList fallback;
    QDirIterator it(extractedRoot, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        const QString rel = QDir(extractedRoot).relativeFilePath(it.filePath());
        if (rel.startsWith(QStringLiteral("data/"), Qt::CaseInsensitive)
            || rel.startsWith(QStringLiteral("data\\"), Qt::CaseInsensitive))
            continue;
        fallback.append(rel);
    }
    return fallback;
}

QString UpdateManifest::sha256HexOfFile(const QString &path, QString *error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error)
            *error = QStringLiteral("无法读取文件: %1").arg(path);
        return {};
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);
    if (!hash.addData(&file)) {
        if (error)
            *error = QStringLiteral("计算 SHA256 失败: %1").arg(path);
        return {};
    }
    return QString::fromLatin1(hash.result().toHex());
}
