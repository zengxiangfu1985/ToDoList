#include "windows_session_monitor.h"

#include <QAbstractNativeEventFilter>
#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#include <wtsapi32.h>
#endif

namespace {

#ifdef Q_OS_WIN
constexpr UINT kWtsSessionChange = 0x02B1;
#endif

class SessionNativeFilter : public QAbstractNativeEventFilter
{
public:
    explicit SessionNativeFilter(WindowsSessionMonitor *owner)
        : m_owner(owner)
    {
    }

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override
    {
        Q_UNUSED(result);
#ifdef Q_OS_WIN
        if (eventType != "windows_generic_MSG" && eventType != "windows_dispatcher_MSG")
            return false;

        const MSG *msg = static_cast<MSG *>(message);
        if (!msg || msg->message != kWtsSessionChange)
            return false;

        if (msg->wParam == WTS_SESSION_LOCK) {
            emit m_owner->sessionLocked();
            return false;
        }
        if (msg->wParam == WTS_SESSION_UNLOCK) {
            emit m_owner->sessionUnlocked();
            return false;
        }
#else
        Q_UNUSED(eventType);
        Q_UNUSED(message);
#endif
        return false;
    }

private:
    WindowsSessionMonitor *m_owner;
};

SessionNativeFilter *s_filter = nullptr;

} // namespace

WindowsSessionMonitor::WindowsSessionMonitor(QObject *parent)
    : QObject(parent)
{
}

WindowsSessionMonitor::~WindowsSessionMonitor()
{
    unregisterWindow();
}

void WindowsSessionMonitor::registerWindow(quintptr windowId)
{
#ifdef Q_OS_WIN
    if (m_registered && m_windowId == windowId)
        return;

    unregisterWindow();
    m_windowId = windowId;
    const HWND hwnd = reinterpret_cast<HWND>(windowId);
    if (!hwnd)
        return;

    if (!WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION))
        return;

    if (!s_filter) {
        s_filter = new SessionNativeFilter(this);
        qApp->installNativeEventFilter(s_filter);
    }

    m_registered = true;
#else
    Q_UNUSED(windowId);
#endif
}

void WindowsSessionMonitor::unregisterWindow()
{
#ifdef Q_OS_WIN
    if (m_registered && m_windowId) {
        const HWND hwnd = reinterpret_cast<HWND>(m_windowId);
        WTSUnRegisterSessionNotification(hwnd);
    }
#endif
    m_registered = false;
    m_windowId = 0;
}
