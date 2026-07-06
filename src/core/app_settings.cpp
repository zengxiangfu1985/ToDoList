#include "app_settings.h"

#include "task_archive.h"

#include "../utils/portable_settings.h"

#include <QCryptographicHash>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QSettings>

namespace {

constexpr auto kGroup = "security";
constexpr auto kPasswordHash = "passwordHash";
constexpr auto kPasswordSalt = "passwordSalt";
constexpr auto kLockAfterSystemLock = "lockAfterSystemLockMinutes";
constexpr auto kLockOnIdle = "lockOnIdleMinutes";
constexpr auto kLockOnStartup = "lockOnStartup";
constexpr auto kMinimizeToTray = "minimizeToTray";
constexpr auto kHotkeyGroup = "hotkeys";
constexpr auto kTodayTasksHotkey = "todayTasks";
constexpr auto kTop3PopupHotkey = "top3Popup";
constexpr auto kUiLanguage = "uiLanguage";

QSettings settings()
{
    return PortableSettings::open();
}

QKeySequence defaultTodayTasksHotkey()
{
    return QKeySequence(QStringLiteral("Alt+Shift+J"));
}

QKeySequence defaultTop3PopupHotkey()
{
    return QKeySequence(QStringLiteral("Alt+Shift+3"));
}

QKeySequence readHotkey(const char *key, const QKeySequence &fallback)
{
    const QString stored = settings().value(QStringLiteral("%1/%2").arg(kHotkeyGroup, key)).toString();
    if (stored.isEmpty())
        return fallback;
    return QKeySequence(stored);
}

void writeHotkey(const char *key, const QKeySequence &sequence)
{
    QSettings s;
    s.beginGroup(kHotkeyGroup);
    s.setValue(key, sequence.toString(QKeySequence::PortableText));
    s.endGroup();
}

QString passwordFilePath()
{
    return TaskArchive::defaultDataDirectory() + QStringLiteral("/password.json");
}

struct PasswordFileData
{
    bool exists = false;
    bool enabled = false;
    QString password;
};

PasswordFileData readPasswordFile()
{
    PasswordFileData data;
    const QString path = passwordFilePath();
    QFile file(path);
    if (!file.exists())
        return data;

    if (!file.open(QIODevice::ReadOnly)) {
        data.exists = true;
        return data;
    }

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    file.close();
    data.exists = true;

    if (!doc.isObject())
        return data;

    const QJsonObject obj = doc.object();
    if (obj.contains(QStringLiteral("enabled")))
        data.enabled = obj.value(QStringLiteral("enabled")).toBool(true);
    else
        data.enabled = true;
    data.password = obj.value(QStringLiteral("password")).toString();
    return data;
}

bool writePasswordFile(const QString &password)
{
    TaskArchive::ensureDataDirectory();

    QJsonObject obj;
    obj.insert(QStringLiteral("_说明"),
               QStringLiteral("修改 password 字段可更改密码（至少 4 位）；将 enabled 设为 false 或清空 password 可清除密码。"
                              "保存后重新解锁或重启程序生效。"));
    obj.insert(QStringLiteral("enabled"), true);
    obj.insert(QStringLiteral("password"), password);

    QFile file(passwordFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

void removePasswordFile()
{
    QFile::remove(passwordFilePath());
}

void clearLegacyPasswordSettings()
{
    QSettings s;
    s.beginGroup(kGroup);
    s.remove(kPasswordHash);
    s.remove(kPasswordSalt);
    s.endGroup();
}

bool hasLegacyPassword()
{
    return !settings().value(QStringLiteral("%1/%2").arg(kGroup, kPasswordHash)).toString().isEmpty();
}

} // namespace

bool AppSettings::isPasswordEnabled()
{
    return hasPassword();
}

bool AppSettings::hasPassword()
{
    const PasswordFileData fileData = readPasswordFile();
    if (fileData.exists)
        return fileData.enabled && !fileData.password.isEmpty();

    return hasLegacyPassword();
}

QByteArray AppSettings::passwordSalt()
{
    const QByteArray hex = settings()
                               .value(QStringLiteral("%1/%2").arg(kGroup, kPasswordSalt))
                               .toByteArray();
    return QByteArray::fromHex(hex);
}

QString AppSettings::hashPassword(const QString &password, const QByteArray &salt)
{
    const QByteArray data = salt + password.toUtf8();
    return QString::fromLatin1(QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex());
}

bool AppSettings::verifyPassword(const QString &password)
{
    const PasswordFileData fileData = readPasswordFile();
    if (fileData.exists) {
        if (!fileData.enabled || fileData.password.isEmpty())
            return true;
        return fileData.password == password;
    }

    if (!hasLegacyPassword())
        return true;

    const QString stored = settings().value(QStringLiteral("%1/%2").arg(kGroup, kPasswordHash)).toString();
    return hashPassword(password, passwordSalt()) == stored;
}

bool AppSettings::setPassword(const QString &password, QString *error)
{
    if (password.length() < 4) {
        if (error)
            *error = QStringLiteral("密码至少 4 位");
        return false;
    }

    if (!writePasswordFile(password)) {
        if (error)
            *error = QStringLiteral("无法写入密码文件");
        return false;
    }

    clearLegacyPasswordSettings();

    QSettings s;
    s.beginGroup(kGroup);
    s.setValue(kLockOnStartup, true);
    s.endGroup();
    return true;
}

void AppSettings::clearPassword()
{
    removePasswordFile();
    clearLegacyPasswordSettings();
}

int AppSettings::lockAfterSystemLockMinutes()
{
    return settings().value(QStringLiteral("%1/%2").arg(kGroup, kLockAfterSystemLock), 5).toInt();
}

void AppSettings::setLockAfterSystemLockMinutes(int minutes)
{
    QSettings s;
    s.beginGroup(kGroup);
    s.setValue(kLockAfterSystemLock, qMax(0, minutes));
    s.endGroup();
}

int AppSettings::lockOnIdleMinutes()
{
    return settings().value(QStringLiteral("%1/%2").arg(kGroup, kLockOnIdle), 0).toInt();
}

void AppSettings::setLockOnIdleMinutes(int minutes)
{
    QSettings s;
    s.beginGroup(kGroup);
    s.setValue(kLockOnIdle, qMax(0, minutes));
    s.endGroup();
}

bool AppSettings::lockOnStartup()
{
    return settings().value(QStringLiteral("%1/%2").arg(kGroup, kLockOnStartup), true).toBool();
}

void AppSettings::setLockOnStartup(bool enabled)
{
    QSettings s;
    s.beginGroup(kGroup);
    s.setValue(kLockOnStartup, enabled);
    s.endGroup();
}

bool AppSettings::minimizeToTray()
{
    return settings().value(QStringLiteral("%1/%2").arg(kGroup, kMinimizeToTray), true).toBool();
}

void AppSettings::setMinimizeToTray(bool enabled)
{
    QSettings s;
    s.beginGroup(kGroup);
    s.setValue(kMinimizeToTray, enabled);
    s.endGroup();
}

QKeySequence AppSettings::todayTasksHotkey()
{
    return readHotkey(kTodayTasksHotkey, defaultTodayTasksHotkey());
}

void AppSettings::setTodayTasksHotkey(const QKeySequence &sequence)
{
    writeHotkey(kTodayTasksHotkey, sequence);
}

QKeySequence AppSettings::top3PopupHotkey()
{
    return readHotkey(kTop3PopupHotkey, defaultTop3PopupHotkey());
}

void AppSettings::setTop3PopupHotkey(const QKeySequence &sequence)
{
    writeHotkey(kTop3PopupHotkey, sequence);
}

QString AppSettings::uiLanguage()
{
    const QString stored = settings().value(QStringLiteral("general/%1").arg(kUiLanguage)).toString();
    if (stored == QStringLiteral("en"))
        return QStringLiteral("en");
    return QStringLiteral("zh");
}

void AppSettings::setUiLanguage(const QString &code)
{
    QSettings s;
    s.beginGroup(QStringLiteral("general"));
    s.setValue(kUiLanguage, code == QStringLiteral("en") ? QStringLiteral("en") : QStringLiteral("zh"));
    s.endGroup();
}

QString AppSettings::passwordFilePath()
{
    return ::passwordFilePath();
}
