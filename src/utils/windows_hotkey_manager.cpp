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
                                   const QKeySequence &top3Popup)
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

    const GlobalHotkeyBinding todayBinding = globalHotkeyFromSequence(todayTasks);
    const GlobalHotkeyBinding top3Binding = globalHotkeyFromSequence(top3Popup);

    if (todayBinding.valid) {
        if (RegisterHotKey(hwnd, kHotkeyQuickAdd, todayBinding.mods, todayBinding.vk))
            m_todayRegistered = true;
        else
            m_lastError = QStringLiteral("「今日任务」快捷键已被其他程序占用");
    }

    if (top3Binding.valid) {
        if (RegisterHotKey(hwnd, kHotkeyTop3, top3Binding.mods, top3Binding.vk))
            m_top3Registered = true;
        else {
            if (m_todayRegistered) {
                UnregisterHotKey(hwnd, kHotkeyQuickAdd);
                m_todayRegistered = false;
            }
            if (m_lastError.isEmpty())
                m_lastError = QStringLiteral("「Top 3 弹窗」快捷键已被其他程序占用");
            else
                m_lastError += QStringLiteral("；「Top 3 弹窗」快捷键已被其他程序占用");
        }
    }

    if (!m_todayRegistered && !m_top3Registered) {
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
    }
#endif
    m_todayRegistered = false;
    m_top3Registered = false;
    m_windowId = 0;
}

QString WindowsHotkeyManager::lastError() const
{
    return m_lastError;
}
