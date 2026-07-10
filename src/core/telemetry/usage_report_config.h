#ifndef USAGE_REPORT_CONFIG_H
#define USAGE_REPORT_CONFIG_H

#include <QString>

struct UsageEndpointConfig
{
    bool enabled = true;
    QString endpoint;
    int heartbeatHours = 24;

    bool isUsable() const;
};

class UsageReportConfigStore
{
public:
    static UsageEndpointConfig load();
    static QString configFilePath();
};

#endif // USAGE_REPORT_CONFIG_H
