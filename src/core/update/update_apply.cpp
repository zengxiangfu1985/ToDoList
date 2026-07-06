#include "update_apply.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDeadlineTimer>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QThread>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace {

void writeLog(const UpdateApplyEngine::LogFn &log, const QString &message)
{
    if (log)
        log(message);
}

QString normalizedRelativePath(const QString &path)
{
    return QDir::fromNativeSeparators(path);
}

bool removePathRecursive(const QString &path)
{
    if (path.isEmpty())
        return true;
    QFileInfo info(path);
    if (!info.exists())
        return true;
    if (info.isDir())
        return QDir(path).removeRecursively();
    return QFile::remove(path);
}

} // namespace

bool UpdateApplyEngine::shouldSkipRelativePath(const QString &relativePath)
{
    const QString rel = normalizedRelativePath(relativePath);
    if (rel.isEmpty())
        return true;
    if (rel.startsWith(QStringLiteral("data/"), Qt::CaseInsensitive))
        return true;
    if (rel.startsWith(QStringLiteral("._update_")))
        return true;
    if (rel == QStringLiteral("update.pending.json"))
        return true;
    if (rel == QStringLiteral("update-config.json"))
        return true;
    return false;
}

QString UpdateApplyEngine::locatePackageRoot(const QString &extractedRoot)
{
    if (QFile::exists(QDir(extractedRoot).filePath(QStringLiteral("ToDoList.exe"))))
        return extractedRoot;

    QDir dir(extractedRoot);
    const QStringList subs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &sub : subs) {
        const QString candidate = dir.filePath(sub);
        if (QFile::exists(QDir(candidate).filePath(QStringLiteral("ToDoList.exe"))))
            return candidate;
    }
    return extractedRoot;
}

bool UpdateApplyEngine::waitForProcess(qint64 pid, int timeoutMs, LogFn log)
{
    if (pid <= 0)
        return true;

#ifdef Q_OS_WIN
    HANDLE handle = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                static_cast<DWORD>(pid));
    if (!handle) {
        writeLog(log, QStringLiteral("主进程已退出 (pid=%1)").arg(pid));
        return true;
    }
    writeLog(log, QStringLiteral("等待主进程退出 (pid=%1)...").arg(pid));
    const DWORD result = WaitForSingleObject(handle, static_cast<DWORD>(timeoutMs));
    CloseHandle(handle);
    if (result == WAIT_TIMEOUT) {
        writeLog(log, QStringLiteral("等待主进程超时"));
        return false;
    }
    return true;
#else
    Q_UNUSED(timeoutMs)
    writeLog(log, QStringLiteral("非 Windows 平台跳过进程等待 (pid=%1)").arg(pid));
    return true;
#endif
}

bool UpdateApplyEngine::waitUntilExecutableIdle(const QString &exePath, int timeoutMs, LogFn log)
{
#ifdef Q_OS_WIN
    const QString escapedPath = QString(exePath).replace(QLatin1Char('\''), QStringLiteral("''"));
    const QString psScript = QStringLiteral(
                                 "$p = Get-Process ToDoList -ErrorAction SilentlyContinue | "
                                 "Where-Object { $_.Path -eq '%1' }; "
                                 "if ($p) { exit 1 } else { exit 0 }")
                                 .arg(escapedPath);
    const QDeadlineTimer deadline(timeoutMs);
    while (!deadline.hasExpired()) {
        QProcess process;
        process.start(QStringLiteral("powershell"),
                      {QStringLiteral("-NoProfile"), QStringLiteral("-Command"), psScript});
        if (!process.waitForFinished(15000)) {
            process.kill();
            writeLog(log, QStringLiteral("检测进程状态超时"));
            return false;
        }
        if (process.exitCode() == 0)
            return true;
        writeLog(log, QStringLiteral("等待 %1 完全退出...").arg(QFileInfo(exePath).fileName()));
        QThread::msleep(500);
    }
    writeLog(log, QStringLiteral("等待程序退出超时: %1").arg(exePath));
    return false;
#else
    Q_UNUSED(exePath)
    Q_UNUSED(timeoutMs)
    Q_UNUSED(log)
    return true;
#endif
}

bool UpdateApplyEngine::replaceFile(const QString &src, const QString &dst, QString *error)
{
    QDir().mkpath(QFileInfo(dst).absolutePath());

    if (QFile::exists(dst)) {
        const QString backupPath = dst + QStringLiteral(".old");
        QFile::remove(backupPath);
        if (!QFile::rename(dst, backupPath)) {
            if (error)
                *error = QStringLiteral("无法替换文件: %1").arg(dst);
            return false;
        }
        if (!QFile::copy(src, dst)) {
            QFile::remove(dst);
            QFile::rename(backupPath, dst);
            if (error)
                *error = QStringLiteral("复制失败: %1").arg(dst);
            return false;
        }
        QFile::remove(backupPath);
        return true;
    }

    if (!QFile::copy(src, dst)) {
        if (error)
            *error = QStringLiteral("复制失败: %1").arg(dst);
        return false;
    }
    return true;
}

bool UpdateApplyEngine::extractZip(const QString &zipPath, const QString &destDir, QString *error,
                                   LogFn log)
{
    QDir().mkpath(destDir);
    removePathRecursive(destDir);

#ifdef Q_OS_WIN
    const QString psScript = QStringLiteral(
                                 "Expand-Archive -LiteralPath '%1' -DestinationPath '%2' -Force")
                                 .arg(QString(zipPath).replace(QLatin1Char('\''), QStringLiteral("''")),
                                      QString(destDir).replace(QLatin1Char('\''), QStringLiteral("''")));
    writeLog(log, QStringLiteral("解压更新包..."));
    QProcess process;
    process.start(QStringLiteral("powershell"),
                  {QStringLiteral("-NoProfile"), QStringLiteral("-Command"), psScript});
    if (!process.waitForStarted(10000)) {
        if (error)
            *error = QStringLiteral("无法启动 PowerShell 解压");
        return false;
    }
    if (!process.waitForFinished(600000)) {
        process.kill();
        if (error)
            *error = QStringLiteral("解压超时");
        return false;
    }
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        if (error)
            *error = QStringLiteral("解压失败: %1")
                         .arg(QString::fromLocal8Bit(process.readAllStandardError()));
        return false;
    }
    return true;
#else
    Q_UNUSED(log)
    if (error)
        *error = QStringLiteral("当前仅支持 Windows 便携版升级");
    return false;
#endif
}

bool UpdateApplyEngine::backupInstallDir(const QString &installDir, const QString &backupDir,
                                         const QStringList &files, QString *error, LogFn log)
{
    QDir().mkpath(backupDir);
    writeLog(log, QStringLiteral("备份当前程序文件到 %1").arg(backupDir));

    for (const QString &relative : files) {
        if (shouldSkipRelativePath(relative))
            continue;
        const QString src = QDir(installDir).filePath(relative);
        const QString dst = QDir(backupDir).filePath(relative);
        if (!QFileInfo::exists(src))
            continue;
        QDir().mkpath(QFileInfo(dst).absolutePath());
        if (QFile::exists(dst))
            QFile::remove(dst);
        if (!QFile::copy(src, dst)) {
            if (error)
                *error = QStringLiteral("备份失败: %1").arg(relative);
            return false;
        }
    }
    return true;
}

bool UpdateApplyEngine::restoreBackup(const QString &installDir, const QString &backupDir,
                                      const QStringList &files, QString *error, LogFn log)
{
    writeLog(log, QStringLiteral("回滚到备份版本..."));
    for (const QString &relative : files) {
        if (shouldSkipRelativePath(relative))
            continue;
        const QString src = QDir(backupDir).filePath(relative);
        const QString dst = QDir(installDir).filePath(relative);
        if (!QFileInfo::exists(src))
            continue;
        QDir().mkpath(QFileInfo(dst).absolutePath());
        if (QFile::exists(dst))
            QFile::remove(dst);
        if (!QFile::copy(src, dst)) {
            if (error)
                *error = QStringLiteral("回滚失败: %1").arg(relative);
            return false;
        }
    }
    return true;
}

bool UpdateApplyEngine::copyWhitelistedFiles(const QString &stagingRoot, const QString &installDir,
                                               QString *error, LogFn log)
{
    QString manifestError;
    QStringList files = UpdateManifest::readFileManifestFromZipRoot(stagingRoot, &manifestError);
    if (files.isEmpty()) {
        if (error)
            *error = manifestError.isEmpty() ? QStringLiteral("更新包缺少 file-manifest.txt")
                                             : manifestError;
        return false;
    }

    writeLog(log, QStringLiteral("复制 %1 个文件到安装目录").arg(files.size()));
    for (const QString &relative : files) {
        if (shouldSkipRelativePath(relative))
            continue;
        const QString src = QDir(stagingRoot).filePath(relative);
        const QString dst = QDir(installDir).filePath(relative);
        if (!QFileInfo::exists(src))
            continue;
        if (!replaceFile(src, dst, error))
            return false;
    }
    return true;
}

bool UpdateApplyEngine::applyPackage(const UpdatePendingInfo &pending, QString *error, LogFn log)
{
    if (!pending.valid) {
        if (error)
            *error = QStringLiteral("pending 信息无效");
        return false;
    }

    const QString installDir = QDir::fromNativeSeparators(pending.installDir);
    const QString zipPath = QDir::fromNativeSeparators(pending.zipPath);
    if (!QFileInfo::exists(zipPath)) {
        if (error)
            *error = QStringLiteral("更新包不存在: %1").arg(zipPath);
        return false;
    }

    const QString stagingRoot = QDir(installDir).filePath(QStringLiteral("._update_staging"));
    const QString backupRoot = QDir(installDir).filePath(
        QStringLiteral("._update_backup/") + QDateTime::currentDateTime().toString(
                                                  QStringLiteral("yyyyMMdd-HHmmss")));

    if (!waitForProcess(pending.mainPid, 120000, log)) {
        if (error)
            *error = QStringLiteral("主程序未退出，无法升级");
        return false;
    }

    const QString exePath = QDir(installDir).filePath(QStringLiteral("ToDoList.exe"));
    if (!waitUntilExecutableIdle(exePath, 120000, log)) {
        if (error)
            *error = QStringLiteral("ToDoList 仍在运行，请完全退出后重试");
        return false;
    }

    if (!extractZip(zipPath, stagingRoot, error, log))
        return false;

    const QString packageRoot = locatePackageRoot(stagingRoot);

    const QString manifestPath = QDir(packageRoot).filePath(QStringLiteral("update.manifest.json"));
    if (QFile::exists(manifestPath)) {
        const UpdatePackageInfo package = UpdateManifest::readPackageJsonFile(manifestPath, error);
        if (!package.version.isEmpty() && package.version != pending.targetVersion) {
            if (error)
                *error = QStringLiteral("更新包版本不匹配: %1 != %2")
                             .arg(package.version, pending.targetVersion);
            return false;
        }
    }

    QStringList files = UpdateManifest::readFileManifestFromZipRoot(packageRoot, error);
    if (files.isEmpty())
        return false;

    if (!backupInstallDir(installDir, backupRoot, files, error, log))
        return false;

    if (!copyWhitelistedFiles(packageRoot, installDir, error, log)) {
        restoreBackup(installDir, backupRoot, files, error, log);
        return false;
    }

    removePathRecursive(stagingRoot);
    QFile::remove(UpdateManifest::pendingFilePath());
    writeLog(log, QStringLiteral("升级完成: %1 (build %2)")
                        .arg(pending.targetVersion)
                        .arg(pending.targetBuild));
    return true;
}
