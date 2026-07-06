#ifndef UPDATE_APPLY_H
#define UPDATE_APPLY_H

#include "update_manifest.h"

#include <functional>

class UpdateApplyEngine
{
public:
    using LogFn = std::function<void(const QString &)>;

    static bool waitForProcess(qint64 pid, int timeoutMs, LogFn log = LogFn());
    static bool extractZip(const QString &zipPath, const QString &destDir, QString *error,
                           LogFn log = LogFn());
    static bool applyPackage(const UpdatePendingInfo &pending, QString *error,
                             LogFn log = LogFn());
    static QString locatePackageRoot(const QString &extractedRoot);

private:
    static bool copyWhitelistedFiles(const QString &stagingRoot, const QString &installDir,
                                     QString *error, LogFn log = LogFn());
    static bool backupInstallDir(const QString &installDir, const QString &backupDir,
                                 const QStringList &files, QString *error, LogFn log = LogFn());
    static bool restoreBackup(const QString &installDir, const QString &backupDir,
                              const QStringList &files, QString *error, LogFn log = LogFn());
    static bool shouldSkipRelativePath(const QString &relativePath);
};

#endif // UPDATE_APPLY_H
