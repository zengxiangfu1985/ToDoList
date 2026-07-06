#include "portable_settings.h"

#include "../core/task_archive.h"
#include "../utils/app_logger.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace {

QString g_testStorageDirectory;

QString storageDirectory()
{
    if (!g_testStorageDirectory.isEmpty())
        return g_testStorageDirectory;
    return TaskArchive::defaultDataDirectory();
}

} // namespace

QString PortableSettings::settingsFilePath()
{
    return QDir(storageDirectory()).filePath(QStringLiteral("settings.ini"));
}

void PortableSettings::setStorageDirectoryForTests(const QString &directory)
{
    g_testStorageDirectory = directory;
}

void PortableSettings::resetTestOverrides()
{
    g_testStorageDirectory.clear();
}

void PortableSettings::migrateLegacyRegistryIfNeeded(QSettings &ini)
{
    if (ini.contains(QStringLiteral("meta/settingsInitialized")))
        return;

#ifdef Q_OS_WIN
    const QSettings legacy(QSettings::NativeFormat, QSettings::UserScope,
                           QCoreApplication::organizationName(),
                           QCoreApplication::applicationName());
    const QStringList legacyKeys = legacy.allKeys();
    if (!legacyKeys.isEmpty()) {
        AppLogger::info("APP",
                        QStringLiteral("正在从系统注册表迁移 %1 项设置到 %2")
                            .arg(legacyKeys.size())
                            .arg(settingsFilePath()));
        for (const QString &key : legacyKeys)
            ini.setValue(key, legacy.value(key));
    }
#else
    Q_UNUSED(ini)
#endif

    ini.setValue(QStringLiteral("meta/settingsInitialized"), true);
    ini.sync();
}

void PortableSettings::initialize()
{
    TaskArchive::ensureDataDirectory();

    const QString path = settingsFilePath();
    QDir().mkpath(QFileInfo(path).absolutePath());

    QSettings ini(path, QSettings::IniFormat);
    migrateLegacyRegistryIfNeeded(ini);

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, storageDirectory());

    AppLogger::info("APP", QStringLiteral("设置文件: %1").arg(path));
}

QSettings PortableSettings::open()
{
    return QSettings(settingsFilePath(), QSettings::IniFormat);
}
