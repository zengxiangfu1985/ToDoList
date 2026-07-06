#include "update_service.h"

#include "update_apply.h"
#include "update_config.h"
#include "../../utils/app_version.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace {

QString updaterExecutablePath()
{
    return QDir(QCoreApplication::applicationDirPath())
        .filePath(QStringLiteral("ToDoListUpdater.exe"));
}

qint64 currentProcessId()
{
#ifdef Q_OS_WIN
    return static_cast<qint64>(GetCurrentProcessId());
#else
    return static_cast<qint64>(QCoreApplication::applicationPid());
#endif
}

} // namespace

UpdateService::UpdateService(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
{
}

UpdateService::State UpdateService::state() const
{
    return m_state;
}

UpdatePackageInfo UpdateService::latestPackage() const
{
    return m_latest;
}

QString UpdateService::lastError() const
{
    return m_lastError;
}

int UpdateService::downloadProgress() const
{
    return m_downloadProgress;
}

void UpdateService::setState(State state)
{
    if (m_state == state)
        return;
    m_state = state;
    emit stateChanged(state);
}

void UpdateService::setError(const QString &message)
{
    m_lastError = message;
    setState(State::Error);
    emit errorOccurred(message);
}

void UpdateService::checkForUpdates()
{
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    setState(State::Checking);
    m_lastError.clear();

    QNetworkRequest request(QUrl(UpdateConfigStore::activeManifestUrl()));
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      QStringLiteral("ToDoList/%1").arg(AppVersion::displayString()));
    m_reply = m_network->get(request);
    connect(m_reply, &QNetworkReply::finished, this, &UpdateService::onCheckFinished);
}

void UpdateService::onCheckFinished()
{
    if (!m_reply)
        return;

    if (m_reply->error() != QNetworkReply::NoError) {
        setError(m_reply->errorString());
        m_reply->deleteLater();
        m_reply = nullptr;
        emit checkFinished(false);
        return;
    }

    QString parseError;
    m_latest = UpdateManifest::parseRemoteJson(m_reply->readAll(), &parseError);
    m_reply->deleteLater();
    m_reply = nullptr;

    if (!m_latest.valid) {
        setError(parseError.isEmpty() ? QStringLiteral("无法解析更新清单") : parseError);
        emit checkFinished(false);
        return;
    }

    if (!AppVersion::isNewerThanCurrent(m_latest.version, m_latest.build)) {
        setState(State::Idle);
        emit checkFinished(false);
        return;
    }

    if (!m_latest.minAppVersion.isEmpty()
        && AppVersion::compareVersion(AppVersion::versionString(), m_latest.minAppVersion) < 0) {
        setError(QStringLiteral("当前版本过低，请先手动升级到 %1").arg(m_latest.minAppVersion));
        emit checkFinished(false);
        return;
    }

    setState(State::Ready);
    emit checkFinished(true);
}

QString UpdateService::downloadCachePath(const UpdatePackageInfo &package) const
{
    const QString cacheRoot = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                              + QStringLiteral("/ToDoList/updates");
    QDir().mkpath(cacheRoot);
    const QString fileName = package.fileName.isEmpty()
                                 ? QStringLiteral("ToDoList-Portable-%1.zip").arg(package.version)
                                 : package.fileName;
    return QDir(cacheRoot).filePath(fileName);
}

void UpdateService::downloadUpdate(const UpdatePackageInfo &package)
{
    if (package.url.isEmpty()) {
        setError(QStringLiteral("更新包缺少下载地址"));
        emit downloadFinished(false);
        return;
    }

    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    m_latest = package;
    m_downloadPath = downloadCachePath(package);
    if (QFile::exists(m_downloadPath))
        QFile::remove(m_downloadPath);

    setState(State::Downloading);
    m_downloadProgress = 0;
    m_downloadReceived = 0;
    m_downloadTotal = package.size;
    emit downloadProgressChanged(0);

    QNetworkRequest request(QUrl(package.url));
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      QStringLiteral("ToDoList/%1").arg(AppVersion::displayString()));
    m_reply = m_network->get(request);
    connect(m_reply, &QNetworkReply::readyRead, this, &UpdateService::onDownloadReadyRead);
    connect(m_reply, &QNetworkReply::downloadProgress, this,
            [this](qint64 received, qint64 total) {
                m_downloadReceived = received;
                m_downloadTotal = total > 0 ? total : m_downloadTotal;
                const int percent = m_downloadTotal > 0
                                        ? static_cast<int>((received * 100) / m_downloadTotal)
                                        : 0;
                m_downloadProgress = percent;
                emit downloadProgressChanged(percent);
            });
    connect(m_reply, &QNetworkReply::finished, this, &UpdateService::onDownloadFinished);
}

void UpdateService::onDownloadReadyRead()
{
    if (!m_reply)
        return;

    QFile file(m_downloadPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        setError(QStringLiteral("无法写入下载缓存"));
        m_reply->abort();
        return;
    }
    file.write(m_reply->readAll());
}

void UpdateService::onDownloadFinished()
{
    if (!m_reply)
        return;

    if (m_reply->error() != QNetworkReply::NoError) {
        setError(m_reply->errorString());
        m_reply->deleteLater();
        m_reply = nullptr;
        emit downloadFinished(false);
        return;
    }

    onDownloadReadyRead();
    m_reply->deleteLater();
    m_reply = nullptr;

    QString validateError;
    if (!validateDownloadedPackage(m_latest, m_downloadPath, &validateError)) {
        setError(validateError);
        emit downloadFinished(false);
        return;
    }

    setState(State::Ready);
    emit downloadFinished(true);
}

bool UpdateService::validateDownloadedPackage(const UpdatePackageInfo &package,
                                              const QString &path, QString *error) const
{
    if (!QFileInfo::exists(path)) {
        if (error)
            *error = QStringLiteral("下载文件不存在");
        return false;
    }

    const qint64 size = QFileInfo(path).size();
    if (package.size > 0 && size != package.size) {
        if (error)
            *error = QStringLiteral("下载大小不匹配");
        return false;
    }

    const QString sha256 = UpdateManifest::sha256HexOfFile(path, error);
    if (sha256.isEmpty())
        return false;
    if (!package.sha256.isEmpty() && sha256.compare(package.sha256, Qt::CaseInsensitive) != 0) {
        if (error)
            *error = QStringLiteral("SHA256 校验失败");
        return false;
    }
    return true;
}

bool UpdateService::writePendingAndLaunchUpdater(const UpdatePackageInfo &package,
                                                 const QString &zipPath, QString *error)
{
    UpdatePendingInfo pending;
    pending.installDir = QCoreApplication::applicationDirPath();
    pending.zipPath = zipPath;
    pending.targetVersion = package.version;
    pending.targetBuild = package.build;
    pending.mainPid = currentProcessId();
    pending.restart = true;
    pending.valid = true;

    if (!UpdateManifest::writePendingFile(pending, error))
        return false;

    const QString updaterPath = updaterExecutablePath();
    if (!QFileInfo::exists(updaterPath)) {
        if (error)
            *error = QStringLiteral("找不到 ToDoListUpdater.exe");
        return false;
    }

    if (!QProcess::startDetached(updaterPath, {QStringLiteral("--apply")})) {
        if (error)
            *error = QStringLiteral("无法启动 Updater");
        return false;
    }
    return true;
}

bool UpdateService::launchUpdaterAndExit(const UpdatePackageInfo &package, QString *error)
{
    const QString zipPath = m_downloadPath.isEmpty() ? downloadCachePath(package) : m_downloadPath;
    QString validateError;
    if (!validateDownloadedPackage(package, zipPath, &validateError)) {
        if (error)
            *error = validateError;
        return false;
    }
    return writePendingAndLaunchUpdater(package, zipPath, error);
}

bool UpdateService::prepareOfflineUpdate(const QString &zipPath, QString *error)
{
    if (!QFileInfo::exists(zipPath)) {
        if (error)
            *error = QStringLiteral("离线更新包不存在");
        return false;
    }

    const QString stagingRoot = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                                + QStringLiteral("/ToDoList/offline-manifest");
    QString extractError;
    if (!UpdateApplyEngine::extractZip(zipPath, stagingRoot, &extractError))
        return false;

    UpdatePackageInfo package = UpdateManifest::readPackageJsonFile(
        QDir(UpdateApplyEngine::locatePackageRoot(stagingRoot))
            .filePath(QStringLiteral("update.manifest.json")),
        error);
    QDir(stagingRoot).removeRecursively();

    if (!package.valid) {
        if (error && error->isEmpty())
            *error = QStringLiteral("离线包缺少 update.manifest.json");
        return false;
    }

    if (AppVersion::isOlderThanCurrent(package.version, package.build)) {
        if (error)
            *error = QStringLiteral("离线包版本不高于当前版本");
        return false;
    }

    m_latest = package;
    m_downloadPath = zipPath;
    setState(State::Ready);
    return true;
}
