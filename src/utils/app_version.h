#ifndef APP_VERSION_H
#define APP_VERSION_H

#include <QString>

class AppVersion
{
public:
    static QString versionString();
    static int buildNumber();
    static QString displayString();
    static QString buildDate();

    static int compareVersion(const QString &left, const QString &right);
    static bool isNewerThanCurrent(const QString &version, int build);
    static bool isOlderThanCurrent(const QString &version, int build);
};

#endif // APP_VERSION_H
