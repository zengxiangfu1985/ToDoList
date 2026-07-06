#ifndef M365_AUTH_SERVICE_H
#define M365_AUTH_SERVICE_H

#include "../task_types.h"

#include <QObject>

struct M365DeviceCodeInfo {
    QString verificationUri;
    QString userCode;
    QString deviceCode;
    int expiresIn = 0;
    int interval = 5;
};

// Microsoft 365 OAuth2 设备码流
class M365AuthService : public QObject
{
    Q_OBJECT
public:
    explicit M365AuthService(QObject *parent = nullptr);

    M365Config config() const;
    void setConfig(const M365Config &config);
    static M365Config loadFromSettings();
    static void saveToSettings(const M365Config &config);

    bool startDeviceCodeFlow(QString *errorMsg = nullptr);
    bool pollAccessToken(QString *errorMsg = nullptr);
    bool refreshAccessToken(QString *errorMsg = nullptr);

    M365DeviceCodeInfo lastDeviceCode() const { return m_deviceCode; }

signals:
    void deviceCodeReady(const QString &url, const QString &userCode);
    void authenticated();
    void authFailed(const QString &error);

private:
    M365Config m_config;
    M365DeviceCodeInfo m_deviceCode;
};

#endif // M365_AUTH_SERVICE_H
