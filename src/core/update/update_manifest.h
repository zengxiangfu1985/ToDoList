#ifndef UPDATE_MANIFEST_H
#define UPDATE_MANIFEST_H

#include <QString>
#include <QStringList>

struct UpdatePackageInfo
{
    QString version;
    int build = 0;
    QString minAppVersion;
    QString releaseNotes;
    bool mandatory = false;
    QString fileName;
    qint64 size = 0;
    QString sha256;
    QString url;
    QStringList mirrorUrls;
    int dbSchema = 0;
    bool valid = false;
};

struct UpdatePendingInfo
{
    QString installDir;
    QString zipPath;
    QString targetVersion;
    int targetBuild = 0;
    qint64 mainPid = 0;
    bool restart = true;
    bool valid = false;
};

class UpdateManifest
{
public:
    static UpdatePackageInfo parseRemoteJson(const QByteArray &bytes, QString *error = nullptr);
    static UpdatePackageInfo parsePackageJson(const QByteArray &bytes, QString *error = nullptr);
    static UpdatePackageInfo readPackageJsonFile(const QString &path, QString *error = nullptr);

    static bool writePendingFile(const UpdatePendingInfo &info, QString *error = nullptr);
    static UpdatePendingInfo readPendingFile(QString *error = nullptr);
    static QString pendingFilePath();

    static QStringList readFileManifest(const QString &path, QString *error = nullptr);
    static QStringList readFileManifestFromZipRoot(const QString &extractedRoot, QString *error = nullptr);

    static QString sha256HexOfFile(const QString &path, QString *error = nullptr);
};

#endif // UPDATE_MANIFEST_H
