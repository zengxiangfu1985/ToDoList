#ifndef UPDATE_SERVICE_H
#define UPDATE_SERVICE_H

#include "update_manifest.h"

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class UpdateService : public QObject
{
    Q_OBJECT
public:
    enum class State {
        Idle,
        Checking,
        Downloading,
        Ready,
        Error
    };
    Q_ENUM(State)

    explicit UpdateService(QObject *parent = nullptr);

    State state() const;
    UpdatePackageInfo latestPackage() const;
    QString lastError() const;
    int downloadProgress() const;

    void checkForUpdates();
    void downloadUpdate(const UpdatePackageInfo &package);
    bool hasValidCachedDownload(const UpdatePackageInfo &package) const;
    bool launchUpdaterAndExit(const UpdatePackageInfo &package, QString *error = nullptr);
    bool prepareOfflineUpdate(const QString &zipPath, QString *error = nullptr);

signals:
    void stateChanged(UpdateService::State state);
    void checkFinished(bool hasUpdate);
    void downloadProgressChanged(int percent);
    void downloadFinished(bool success);
    void errorOccurred(const QString &message);

private slots:
    void onCheckFinished();
    void onDownloadReadyRead();
    void onDownloadFinished();

private:
    void setState(State state);
    void setError(const QString &message);
    void startNextManifestRequest();
    void finishCheckNoUpdate();
    void finishCheckWithUpdate();
    void startNextDownloadRequest();
    QStringList buildDownloadUrls(const UpdatePackageInfo &package) const;
    QString downloadCachePath(const UpdatePackageInfo &package) const;
    bool validateDownloadedPackage(const UpdatePackageInfo &package, const QString &path,
                                     QString *error) const;
    bool writePendingAndLaunchUpdater(const UpdatePackageInfo &package, const QString &zipPath,
                                      QString *error);

    QNetworkAccessManager *m_network = nullptr;
    QNetworkReply *m_reply = nullptr;
    State m_state = State::Idle;
    UpdatePackageInfo m_latest;
    QString m_lastError;
    QStringList m_pendingManifestUrls;
    QStringList m_pendingDownloadUrls;
    QString m_downloadPath;
    qint64 m_downloadTotal = 0;
    qint64 m_downloadReceived = 0;
    int m_downloadProgress = 0;
};

#endif // UPDATE_SERVICE_H
