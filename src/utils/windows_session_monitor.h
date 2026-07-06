#ifndef WINDOWS_SESSION_MONITOR_H
#define WINDOWS_SESSION_MONITOR_H

#include <QObject>

class WindowsSessionMonitor : public QObject
{
    Q_OBJECT
public:
    explicit WindowsSessionMonitor(QObject *parent = nullptr);
    ~WindowsSessionMonitor() override;

    void registerWindow(quintptr windowId);
    void unregisterWindow();

signals:
    void sessionLocked();
    void sessionUnlocked();

private:
    quintptr m_windowId = 0;
    bool m_registered = false;
};

#endif // WINDOWS_SESSION_MONITOR_H
