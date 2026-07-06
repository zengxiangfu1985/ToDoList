#ifndef PORTABLE_SETTINGS_H
#define PORTABLE_SETTINGS_H

#include <QSettings>
#include <QString>

class PortableSettings
{
public:
    static void initialize();
    static QString settingsFilePath();
    static QSettings open();

    static void setStorageDirectoryForTests(const QString &directory);
    static void resetTestOverrides();

private:
    static void migrateLegacyRegistryIfNeeded(QSettings &ini);
};

#endif // PORTABLE_SETTINGS_H
