#ifndef UPDATE_CONFIG_H
#define UPDATE_CONFIG_H

#include <QString>
#include <QVector>

struct UpdateSource
{
    QString name;
    QString type;
    QString url;
};

struct UpdateConfig
{
    QVector<UpdateSource> sources;
    QString activeSource;
    bool checkOnStartup = true;
    int checkIntervalHours = 168;
};

class UpdateConfigStore
{
public:
    static UpdateConfig load();
    static QString configFilePath();
    static QString activeManifestUrl();
};

#endif // UPDATE_CONFIG_H
