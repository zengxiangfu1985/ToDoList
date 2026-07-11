#include "update_service.h"

#include "update_apply.h"
#include "update_config.h"
#include "../../utils/app_logger.h"
#include "../../utils/app_version.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>
#include <QUrlQuery>

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

QString friendlyNetworkError(QNetworkReply *reply, bool forDownload)
{
    if (!reply)
        return forDownload ? QStringLiteral("下载更新包失败，请稍后重试。")
                           : QStringLiteral("无法检查更新，请稍后重试。");

    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status == 429)
        return QStringLiteral("访问更新服务器过于频繁，请稍后再试或使用「导入离线更新包」。");
    if (status == 403)
        return QStringLiteral("更新服务器拒绝访问，请稍后再试或使用「导入离线更新包」。");
    if (status >= 500)
        return QStringLiteral("更新服务器暂时不可用，请稍后再试。");

    switch (reply->error()) {
    case QNetworkReply::TimeoutError:
    case QNetworkReply::OperationCanceledError:
        return forDownload ? QStringLiteral("下载更新包超时，请检查网络后重试。")
                           : QStringLiteral("连接更新服务器超时，请检查网络后重试。");
    case QNetworkReply::HostNotFoundError:
    case QNetworkReply::ConnectionRefusedError:
        return QStringLiteral("无法连接更新服务器，请检查网络后重试。");
    default:
        break;
    }

    return forDownload ? QStringLiteral("下载更新包失败，请稍后重试或使用「导入离线更新包」。")
                     : QStringLiteral("无法检查更新，请检查网络或稍后重试。");
}

bool isNewerPackage(const UpdatePackageInfo &candidate, const UpdatePackageInfo &baseline)
{
    if (!candidate.valid)
        return false;
    if (!baseline.valid)
        return true;

    const int cmp = AppVersion::compareVersion(candidate.version, baseline.version);
    if (cmp > 0)
        return true;
    if (cmp < 0)
        return false;
    return candidate.build > baseline.build;
}

void rememberBestRemote(UpdatePackageInfo *best, const UpdatePackageInfo &candidate)
{
    if (!candidate.valid)
        return;
    if (!best->valid || isNewerPackage(candidate, *best))
        *best = candidate;
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
    if (m_state == State::Downloading) {
        AppLogger::info("UPDATE", QStringLiteral("正在下载更新包，忽略重复检查"));
        return;
    }

    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    setState(State::Checking);
    m_lastError.clear();
    m_bestRemote = {};
    m_pendingManifestUrls = UpdateConfigStore::manifestUrls();
    startNextManifestRequest();
}

void UpdateService::startNextManifestRequest()
{
    if (m_pendingManifestUrls.isEmpty()) {
        const QString message = m_lastError.isEmpty()
                                    ? QStringLiteral("无法连接更新服务器，请检查网络或稍后重试")
                                    : m_lastError;
        setError(message);
        emit checkFinished(false);
        return;
    }

    const QString url = m_pendingManifestUrls.takeFirst();
    QUrl manifestUrl(url);
    if (!manifestUrl.hasQuery()) {
        QUrlQuery query;
        query.addQueryItem(QStringLiteral("_"),
                           QString::number(QDateTime::currentSecsSinceEpoch()));
        manifestUrl.setQuery(query);
    }
    AppLogger::info("UPDATE", QStringLiteral("检查更新: %1").arg(manifestUrl.toString()));

    QNetworkRequest netRequest(manifestUrl);
    netRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
                            QNetworkRequest::AlwaysNetwork);
    netRequest.setRawHeader("Cache-Control", "no-cache");
    netRequest.setHeader(QNetworkRequest::UserAgentHeader,
                         QStringLiteral("ToDoList/%1").arg(AppVersion::displayString()));
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    netRequest.setTransferTimeout(60000);
#endif
    m_reply = m_network->get(netRequest);
    connect(m_reply, &QNetworkReply::finished, this, &UpdateService::onCheckFinished);
}

void UpdateService::finishCheckNoUpdate()
{
    if (m_bestRemote.valid)
        m_latest = m_bestRemote;

    setState(State::Idle);
    m_lastError.clear();
    AppLogger::info("UPDATE",
                    QStringLiteral("无可用更新 (当前 %1 build %2, 远端 %3 build %4)")
                        .arg(AppVersion::versionString())
                        .arg(AppVersion::buildNumber())
                        .arg(m_latest.version)
                        .arg(m_latest.build));
    emit checkFinished(false);
}

void UpdateService::finishCheckWithUpdate()
{
    setState(State::Ready);
    AppLogger::info("UPDATE",
                    QStringLiteral("发现新版本: %1 (build %2)")
                        .arg(m_latest.version)
                        .arg(m_latest.build));
    emit checkFinished(true);
}

void UpdateService::onCheckFinished()
{
    if (!m_reply)
        return;

    const QByteArray body = m_reply->readAll();
    if (m_reply->error() != QNetworkReply::NoError) {
        const QString detail = QStringLiteral("%1 (%2)")
                                   .arg(m_reply->errorString(), m_reply->url().toString());
        AppLogger::warn("UPDATE", QStringLiteral("拉取失败: %1").arg(detail));
        m_lastError = friendlyNetworkError(m_reply, false);
        m_reply->deleteLater();
        m_reply = nullptr;
        startNextManifestRequest();
        return;
    }

    m_reply->deleteLater();
    m_reply = nullptr;

    QString parseError;
    m_latest = UpdateManifest::parseRemoteJson(body, &parseError);
    if (!m_latest.valid) {
        const QString err = parseError.isEmpty() ? QStringLiteral("无法解析更新清单") : parseError;
        AppLogger::warn("UPDATE", err);
        m_lastError = err;
        startNextManifestRequest();
        return;
    }

    rememberBestRemote(&m_bestRemote, m_latest);

    if (!AppVersion::isNewerThanCurrent(m_latest.version, m_latest.build)) {
        if (!m_pendingManifestUrls.isEmpty()) {
            AppLogger::info("UPDATE",
                            QStringLiteral("清单版本 %1 (build %2) 不高于当前，尝试下一更新源")
                                .arg(m_latest.version)
                                .arg(m_latest.build));
            startNextManifestRequest();
            return;
        }
        finishCheckNoUpdate();
        return;
    }

    if (!m_latest.minAppVersion.isEmpty()
        && AppVersion::compareVersion(AppVersion::versionString(), m_latest.minAppVersion) < 0) {
        setError(QStringLiteral("当前版本过低，请先手动升级到 %1").arg(m_latest.minAppVersion));
        emit checkFinished(false);
        return;
    }

    finishCheckWithUpdate();
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

    m_latest = package;
    m_downloadPath = downloadCachePath(package);

    QString cacheError;
    if (validateDownloadedPackage(package, m_downloadPath, &cacheError)) {
        AppLogger::info("UPDATE",
                        QStringLiteral("复用已下载的更新包: %1").arg(m_downloadPath));
        m_lastError.clear();
        m_downloadProgress = 100;
        setState(State::Ready);
        emit downloadProgressChanged(100);
        emit downloadFinished(true);
        return;
    }

    if (m_state == State::Downloading && m_latest.version == package.version
        && m_latest.build == package.build) {
        AppLogger::info("UPDATE", QStringLiteral("更新包正在下载中，忽略重复请求"));
        return;
    }

    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    m_pendingDownloadUrls = buildDownloadUrls(package);
    m_downloadProgress = 0;
    m_downloadReceived = 0;
    m_downloadTotal = package.size;
    m_lastError.clear();

    setState(State::Downloading);
    emit downloadProgressChanged(0);
    startNextDownloadRequest();
}

bool UpdateService::hasValidCachedDownload(const UpdatePackageInfo &package) const
{
    const QString path = downloadCachePath(package);
    QString error;
    return validateDownloadedPackage(package, path, &error);
}

QStringList UpdateService::buildDownloadUrls(const UpdatePackageInfo &package) const
{
    QStringList urls;
    auto appendUnique = [&](const QString &url) {
        if (!url.isEmpty() && !urls.contains(url))
            urls.append(url);
    };
    appendUnique(package.url);
    for (const QString &mirrorUrl : package.mirrorUrls)
        appendUnique(mirrorUrl);
    return urls;
}

void UpdateService::startNextDownloadRequest()
{
    if (m_pendingDownloadUrls.isEmpty()) {
        const QString message = m_lastError.isEmpty()
                                    ? QStringLiteral("所有下载源均失败，请检查网络或使用「导入离线更新包」")
                                    : m_lastError;
        setError(message);
        emit downloadFinished(false);
        return;
    }

    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    if (QFile::exists(m_downloadPath))
        QFile::remove(m_downloadPath);

    const QString url = m_pendingDownloadUrls.takeFirst();
    AppLogger::info("UPDATE", QStringLiteral("下载更新包: %1").arg(url));

    const QUrl downloadUrl(url);
    QNetworkRequest request(downloadUrl);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    request.setTransferTimeout(600000);
#endif
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
        const QString detail = QStringLiteral("下载更新包失败: %1 (%2)")
                                   .arg(m_reply->errorString(), m_reply->url().toString());
        AppLogger::warn("UPDATE", detail);
        m_lastError = friendlyNetworkError(m_reply, true);
        m_reply->deleteLater();
        m_reply = nullptr;
        if (!m_pendingDownloadUrls.isEmpty())
            AppLogger::info("UPDATE", QStringLiteral("切换备用下载源重试"));
        startNextDownloadRequest();
        return;
    }

    onDownloadReadyRead();
    m_reply->deleteLater();
    m_reply = nullptr;

    QString validateError;
    if (!validateDownloadedPackage(m_latest, m_downloadPath, &validateError)) {
        AppLogger::warn("UPDATE", validateError);
        m_lastError = validateError;
        if (!m_pendingDownloadUrls.isEmpty())
            AppLogger::info("UPDATE", QStringLiteral("校验失败，切换备用下载源重试"));
        startNextDownloadRequest();
        return;
    }

    m_lastError.clear();
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
    if (size == 0) {
        if (error)
            *error = QStringLiteral("下载文件为空");
        return false;
    }

    if (package.size > 1024 * 1024 && size < 64 * 1024) {
        if (error) {
            *error = QStringLiteral("下载未完成（实际 %1 字节，期望约 %2 字节），请检查网络")
                         .arg(size)
                         .arg(package.size);
        }
        return false;
    }

    if (!package.sha256.isEmpty()) {
        const QString sha256 = UpdateManifest::sha256HexOfFile(path, error);
        if (sha256.isEmpty())
            return false;
        if (sha256.compare(package.sha256, Qt::CaseInsensitive) == 0)
            return true;
        if (error)
            *error = QStringLiteral("SHA256 校验失败");
        return false;
    }

    if (package.size > 0 && size != package.size) {
        if (error) {
            *error = QStringLiteral("下载大小不匹配（期望 %1，实际 %2 字节）")
                         .arg(package.size)
                         .arg(size);
        }
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
