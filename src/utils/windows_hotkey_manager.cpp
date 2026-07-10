#include "windows_hotkey_manager.h"

#include "global_hotkey.h"

#include <QAbstractNativeEventFilter>
#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace {

#ifdef Q_OS_WIN
constexpr int kHotkeyQuickAdd = 1;
constexpr int kHotkeyTop3 = 2;
constexpr int kHotkeyQuickCapture = 3;
constexpr int kHotkeyFocus25 = 4;
#endif

} // namespace

class HotkeyNativeFilter : public QAbstractNativeEventFilter
{
public:
    explicit HotkeyNativeFilter(WindowsHotkeyManager *owner)
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
        if (!msg || msg->message != WM_HOTKEY)
            return false;

        if (msg->wParam == kHotkeyQuickAdd) {
            emit m_owner->quickAddTriggered();
            return true;
        }
        if (msg->wParam == kHotkeyTop3) {
            emit m_owner->top3PopupTriggered();
            return true;
        }
        if (msg->wParam == kHotkeyQuickCapture) {
            emit m_owner->quickCaptureTriggered();
            return true;
        }
        if (msg->wParam == kHotkeyFocus25) {
            emit m_owner->focus25Triggered();
            return true;
        }
#else
        Q_UNUSED(eventType);
        Q_UNUSED(message);
#endif
        return false;
    }

private:
    WindowsHotkeyManager *m_owner;
};

WindowsHotkeyManager::WindowsHotkeyManager(QObject *parent)
    : QObject(parent)
{
}

WindowsHotkeyManager::~WindowsHotkeyManager()
{
    uninstall();
    if (m_filter) {
        qApp->removeNativeEventFilter(m_filter);
        delete m_filter;
        m_filter = nullptr;
    }
}

bool WindowsHotkeyManager::install(quintptr windowId, const QKeySequence &todayTasks,
                                   const QKeySequence &top3Popup, const QKeySequence &quickCapture,
                                   const QKeySequence &focus25)
{
#ifdef Q_OS_WIN
    uninstall();
    m_lastError.clear();
    m_windowId = windowId;

    const HWND hwnd = reinterpret_cast<HWND>(windowId);
    if (!hwnd) {
        m_lastError = QStringLiteral("窗口句柄无效");
        return false;
    }

    auto registerOne = [&](int id, const GlobalHotkeyBinding &binding, bool *flag, const QString &label) -> bool {
        if (!binding.valid)
            return true;
        if (RegisterHotKey(hwnd, id, binding.mods, binding.vk)) {
            *flag = true;
            return true;
        }
        if (m_lastError.isEmpty())
            m_lastError = QStringLiteral("「%1」快捷键已被其他程序占用").arg(label);
        else
            m_lastError += QStringLiteral("；「%1」快捷键已被其他程序占用").arg(label);
        return false;
    };

    const GlobalHotkeyBinding todayBinding = globalHotkeyFromSequence(todayTasks);
    const GlobalHotkeyBinding top3Binding = globalHotkeyFromSequence(top3Popup);
    const GlobalHotkeyBinding captureBinding = globalHotkeyFromSequence(quickCapture);
    const GlobalHotkeyBinding focusBinding = globalHotkeyFromSequence(focus25);

    registerOne(kHotkeyQuickAdd, todayBinding, &m_todayRegistered, QStringLiteral("今日任务"));
    registerOne(kHotkeyTop3, top3Binding, &m_top3Registered, QStringLiteral("Top 3 弹窗"));
    registerOne(kHotkeyQuickCapture, captureBinding, &m_quickCaptureRegistered, QStringLiteral("闪记"));
    registerOne(kHotkeyFocus25, focusBinding, &m_focus25Registered, QStringLiteral("Focus 25"));

    if (!m_todayRegistered && !m_top3Registered && !m_quickCaptureRegistered && !m_focus25Registered) {
        if (m_lastError.isEmpty())
            return true;
        return false;
    }

    if (!m_filter) {
        m_filter = new HotkeyNativeFilter(this);
        qApp->installNativeEventFilter(m_filter);
    }

    return m_lastError.isEmpty();
#else
    Q_UNUSED(windowId);
    Q_UNUSED(todayTasks);
    Q_UNUSED(top3Popup);
    Q_UNUSED(quickCapture);
    Q_UNUSED(focus25);
    m_lastError = QStringLiteral("当前平台不支持全局快捷键");
    return false;
#endif
}

void WindowsHotkeyManager::uninstall()
{
#ifdef Q_OS_WIN
    if (m_windowId) {
        const HWND hwnd = reinterpret_cast<HWND>(m_windowId);
        if (m_todayRegistered)
            UnregisterHotKey(hwnd, kHotkeyQuickAdd);
        if (m_top3Registered)
            UnregisterHotKey(hwnd, kHotkeyTop3);
        if (m_quickCaptureRegistered)
            UnregisterHotKey(hwnd, kHotkeyQuickCapture);
        if (m_focus25Registered)
            UnregisterHotKey(hwnd, kHotkeyFocus25);
    }
#endif
    m_todayRegistered = false;
    m_top3Registered = false;
    m_quickCaptureRegistered = false;
    m_focus25Registered = false;
    m_windowId = 0;
}

QString WindowsHotkeyManager::lastError() const
{
    return m_lastError;
}
