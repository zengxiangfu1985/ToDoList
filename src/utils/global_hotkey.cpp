#include "global_hotkey.h"

#include <Qt>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

GlobalHotkeyBinding globalHotkeyFromSequence(const QKeySequence &sequence)
{
    GlobalHotkeyBinding binding;
    if (sequence.isEmpty())
        return binding;

    const int combined = sequence[0];
    const int key = combined & ~Qt::KeyboardModifierMask;
    const Qt::KeyboardModifiers modifiers = Qt::KeyboardModifiers(combined & Qt::KeyboardModifierMask);

    if (key == 0 || key == Qt::Key_unknown)
        return binding;

#ifdef Q_OS_WIN
    quint32 mods = 0;
    if (modifiers & Qt::ShiftModifier)
        mods |= MOD_SHIFT;
    if (modifiers & Qt::ControlModifier)
        mods |= MOD_CONTROL;
    if (modifiers & Qt::AltModifier)
        mods |= MOD_ALT;
    if (modifiers & Qt::MetaModifier)
        mods |= MOD_WIN;

    if (mods == 0)
        return binding;

    quint32 vk = 0;
    if (key >= Qt::Key_A && key <= Qt::Key_Z)
        vk = static_cast<quint32>('A' + (key - Qt::Key_A));
    else if (key >= Qt::Key_0 && key <= Qt::Key_9)
        vk = static_cast<quint32>('0' + (key - Qt::Key_0));
    else if (key >= Qt::Key_F1 && key <= Qt::Key_F24)
        vk = static_cast<quint32>(VK_F1 + (key - Qt::Key_F1));
    else
        return binding;

    binding.mods = mods;
    binding.vk = vk;
    binding.valid = true;
#endif

    return binding;
}

QString globalHotkeyDisplayText(const QKeySequence &sequence)
{
    if (sequence.isEmpty())
        return QString();
    return sequence.toString(QKeySequence::NativeText);
}

QString globalHotkeyMenuLabel(const QString &actionName, const QKeySequence &sequence)
{
    const QString shortcut = globalHotkeyDisplayText(sequence);
    if (shortcut.isEmpty())
        return actionName;
    return QStringLiteral("%1 (%2)").arg(actionName, shortcut);
}

bool globalHotkeySequenceValid(const QKeySequence &sequence, QString *error)
{
    if (sequence.isEmpty()) {
        if (error)
            *error = QStringLiteral("请设置快捷键");
        return false;
    }

    const GlobalHotkeyBinding binding = globalHotkeyFromSequence(sequence);
    if (!binding.valid) {
        if (error)
            *error = QStringLiteral("快捷键需包含 Win / Ctrl / Alt / Shift 之一，并绑定字母、数字或功能键");
        return false;
    }

    return true;
}
