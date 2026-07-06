#ifndef WINDOWS_HOTKEY_MANAGER_H
#define WINDOWS_HOTKEY_MANAGER_H

#include <QKeySequence>
#include <QObject>
#include <QtGlobal>

class HotkeyNativeFilter;

// Scenario: S04 — Windows 全局快捷键
class WindowsHotkeyManager : public QObject
{
    Q_OBJECT
public:
    explicit WindowsHotkeyManager(QObject *parent = nullptr);
    ~WindowsHotkeyManager() override;

    bool install(quintptr windowId, const QKeySequence &todayTasks, const QKeySequence &top3Popup);
    void uninstall();
    QString lastError() const;

signals:
    void quickAddTriggered();
    void top3PopupTriggered();

private:
    friend class HotkeyNativeFilter;

    quintptr m_windowId = 0;
    bool m_todayRegistered = false;
    bool m_top3Registered = false;
    QString m_lastError;
    HotkeyNativeFilter *m_filter = nullptr;
};

#endif // WINDOWS_HOTKEY_MANAGER_H
