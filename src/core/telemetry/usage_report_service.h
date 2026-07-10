#ifndef USAGE_REPORT_SERVICE_H
#define USAGE_REPORT_SERVICE_H

#include <QObject>
#include <QString>

class QNetworkAccessManager;

class UsageReportService : public QObject
{
    Q_OBJECT
public:
    enum class Event {
        AppStart,
        Heartbeat,
        AppExit,
        UpgradeSuccess
    };
    Q_ENUM(Event)

    explicit UsageReportService(QObject *parent = nullptr);

    void report(Event event, const QString &extraJson = QString());
    void reportAppStart();
    void reportHeartbeatIfDue();
    void reportAppExit();
    void reportUpgradeSuccess(const QString &fromVersion);

    bool isActive() const;

private:
    void sendPayload(const QJsonObject &payload);
    QString eventName(Event event) const;

    QNetworkAccessManager *m_network = nullptr;
};

#endif // USAGE_REPORT_SERVICE_H
