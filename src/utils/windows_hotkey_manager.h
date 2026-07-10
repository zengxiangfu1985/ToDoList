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

    bool install(quintptr windowId, const QKeySequence &todayTasks, const QKeySequence &top3Popup,
                 const QKeySequence &quickCapture, const QKeySequence &focus25);
    void uninstall();
    QString lastError() const;

signals:
    void quickAddTriggered();
    void top3PopupTriggered();
    void quickCaptureTriggered();
    void focus25Triggered();

private:
    friend class HotkeyNativeFilter;

    quintptr m_windowId = 0;
    bool m_todayRegistered = false;
    bool m_top3Registered = false;
    bool m_quickCaptureRegistered = false;
    bool m_focus25Registered = false;
    QString m_lastError;
    HotkeyNativeFilter *m_filter = nullptr;
};

#endif // WINDOWS_HOTKEY_MANAGER_H
