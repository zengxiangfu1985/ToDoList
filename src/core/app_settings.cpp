#include "app_settings.h"

#include "task_archive.h"

#include "../utils/portable_settings.h"

#include <QCryptographicHash>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QTime>

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
constexpr auto kQuickCaptureHotkey = "quickCapture";
constexpr auto kFocus25Hotkey = "focus25";
constexpr auto kQuickCaptureGroup = "quickCapture";
constexpr auto kQuickCaptureAutoAnalyze = "autoAnalyze";
constexpr auto kFocusGroup = "focus";
constexpr auto kFocusDurationMinutes = "durationMinutes";
constexpr auto kFocusTrayCountdown = "trayCountdown";
constexpr auto kHabitGroup = "healthHabits";
constexpr auto kHabitPauseDuringFocus = "pauseDuringFocus";
constexpr auto kHabitWeekdaysOnly = "weekdaysOnly";
constexpr auto kHabitActiveStart = "activeStart";
constexpr auto kHabitActiveEnd = "activeEnd";
constexpr auto kUiLanguage = "uiLanguage";
constexpr auto kUsageGroup = "usage";
constexpr auto kUsageStatisticsEnabled = "statisticsEnabled";
constexpr auto kInstallId = "installId";
constexpr auto kLastUsageHeartbeat = "lastHeartbeatUtc";

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

QKeySequence defaultQuickCaptureHotkey()
{
    return QKeySequence(QStringLiteral("Alt+Shift+N"));
}

QKeySequence defaultFocus25Hotkey()
{
    return QKeySequence(QStringLiteral("Alt+Shift+F"));
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

QKeySequence AppSettings::quickCaptureHotkey()
{
    return readHotkey(kQuickCaptureHotkey, defaultQuickCaptureHotkey());
}

void AppSettings::setQuickCaptureHotkey(const QKeySequence &sequence)
{
    writeHotkey(kQuickCaptureHotkey, sequence);
}

QKeySequence AppSettings::focus25Hotkey()
{
    return readHotkey(kFocus25Hotkey, defaultFocus25Hotkey());
}

void AppSettings::setFocus25Hotkey(const QKeySequence &sequence)
{
    writeHotkey(kFocus25Hotkey, sequence);
}

bool AppSettings::quickCaptureAutoAnalyze()
{
    QSettings s;
    s.beginGroup(kQuickCaptureGroup);
    const bool enabled = s.value(kQuickCaptureAutoAnalyze, true).toBool();
    s.endGroup();
    return enabled;
}

void AppSettings::setQuickCaptureAutoAnalyze(bool enabled)
{
    QSettings s;
    s.beginGroup(kQuickCaptureGroup);
    s.setValue(kQuickCaptureAutoAnalyze, enabled);
    s.endGroup();
}

int AppSettings::focusDurationMinutes()
{
    QSettings s;
    s.beginGroup(kFocusGroup);
    const int minutes = s.value(kFocusDurationMinutes, 25).toInt();
    s.endGroup();
    if (minutes == 15 || minutes == 25 || minutes == 50)
        return minutes;
    return 25;
}

void AppSettings::setFocusDurationMinutes(int minutes)
{
    int value = 25;
    if (minutes == 15 || minutes == 25 || minutes == 50)
        value = minutes;
    QSettings s;
    s.beginGroup(kFocusGroup);
    s.setValue(kFocusDurationMinutes, value);
    s.endGroup();
}

bool AppSettings::focusTrayCountdown()
{
    QSettings s;
    s.beginGroup(kFocusGroup);
    const bool enabled = s.value(kFocusTrayCountdown, true).toBool();
    s.endGroup();
    return enabled;
}

void AppSettings::setFocusTrayCountdown(bool enabled)
{
    QSettings s;
    s.beginGroup(kFocusGroup);
    s.setValue(kFocusTrayCountdown, enabled);
    s.endGroup();
}

bool AppSettings::habitPauseDuringFocus()
{
    QSettings s = settings();
    s.beginGroup(kHabitGroup);
    const bool enabled = s.value(kHabitPauseDuringFocus, true).toBool();
    s.endGroup();
    return enabled;
}

void AppSettings::setHabitPauseDuringFocus(bool enabled)
{
    QSettings s = settings();
    s.beginGroup(kHabitGroup);
    s.setValue(kHabitPauseDuringFocus, enabled);
    s.endGroup();
}

bool AppSettings::habitWeekdaysOnly()
{
    QSettings s = settings();
    s.beginGroup(kHabitGroup);
    const bool enabled = s.value(kHabitWeekdaysOnly, false).toBool();
    s.endGroup();
    return enabled;
}

void AppSettings::setHabitWeekdaysOnly(bool enabled)
{
    QSettings s = settings();
    s.beginGroup(kHabitGroup);
    s.setValue(kHabitWeekdaysOnly, enabled);
    s.endGroup();
}

QTime AppSettings::habitActiveStart()
{
    QSettings s = settings();
    s.beginGroup(kHabitGroup);
    const QTime time = QTime::fromString(s.value(kHabitActiveStart, QStringLiteral("00:00")).toString(),
                                         QStringLiteral("HH:mm"));
    s.endGroup();
    return time.isValid() ? time : QTime(0, 0);
}

void AppSettings::setHabitActiveStart(const QTime &time)
{
    QSettings s = settings();
    s.beginGroup(kHabitGroup);
    s.setValue(kHabitActiveStart, time.toString(QStringLiteral("HH:mm")));
    s.endGroup();
}

QTime AppSettings::habitActiveEnd()
{
    QSettings s = settings();
    s.beginGroup(kHabitGroup);
    const QTime time = QTime::fromString(s.value(kHabitActiveEnd, QStringLiteral("23:59")).toString(),
                                         QStringLiteral("HH:mm"));
    s.endGroup();
    return time.isValid() ? time : QTime(23, 59);
}

void AppSettings::setHabitActiveEnd(const QTime &time)
{
    QSettings s = settings();
    s.beginGroup(kHabitGroup);
    s.setValue(kHabitActiveEnd, time.toString(QStringLiteral("HH:mm")));
    s.endGroup();
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

bool AppSettings::usageStatisticsEnabled()
{
    QSettings s;
    s.beginGroup(kUsageGroup);
    const bool enabled = s.value(kUsageStatisticsEnabled, true).toBool();
    s.endGroup();
    return enabled;
}

void AppSettings::setUsageStatisticsEnabled(bool enabled)
{
    QSettings s;
    s.beginGroup(kUsageGroup);
    s.setValue(kUsageStatisticsEnabled, enabled);
    s.endGroup();
}

QString AppSettings::installId()
{
    QSettings s;
    s.beginGroup(kUsageGroup);
    QString id = s.value(kInstallId).toString().trimmed();
    if (id.isEmpty()) {
        id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        s.setValue(kInstallId, id);
    }
    s.endGroup();
    return id;
}

QDateTime AppSettings::lastUsageHeartbeat()
{
    const QString raw = settings().value(QStringLiteral("%1/%2").arg(kUsageGroup, kLastUsageHeartbeat)).toString();
    if (raw.isEmpty())
        return {};
    return QDateTime::fromString(raw, Qt::ISODateWithMs);
}

void AppSettings::setLastUsageHeartbeat(const QDateTime &atUtc)
{
    QSettings s;
    s.beginGroup(kUsageGroup);
    s.setValue(kLastUsageHeartbeat, atUtc.toString(Qt::ISODateWithMs));
    s.endGroup();
}

QString AppSettings::passwordFilePath()
{
    return ::passwordFilePath();
}
