#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <QString>

#include <QKeySequence>

class AppSettings
{
public:
    static bool isPasswordEnabled();
    static bool hasPassword();
    static bool verifyPassword(const QString &password);
    static bool setPassword(const QString &password, QString *error = nullptr);
    static void clearPassword();

    static int lockAfterSystemLockMinutes();
    static void setLockAfterSystemLockMinutes(int minutes);

    static int lockOnIdleMinutes();
    static void setLockOnIdleMinutes(int minutes);

    static bool lockOnStartup();
    static void setLockOnStartup(bool enabled);

    static bool minimizeToTray();
    static void setMinimizeToTray(bool enabled);

    static QKeySequence todayTasksHotkey();
    static void setTodayTasksHotkey(const QKeySequence &sequence);
    static QKeySequence top3PopupHotkey();
    static void setTop3PopupHotkey(const QKeySequence &sequence);

    static QString uiLanguage();
    static void setUiLanguage(const QString &code);

    static QString passwordFilePath();

private:
    static QByteArray passwordSalt();
    static QString hashPassword(const QString &password, const QByteArray &salt);
};

#endif // APP_SETTINGS_H
