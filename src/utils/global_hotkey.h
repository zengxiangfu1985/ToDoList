#ifndef GLOBAL_HOTKEY_H
#define GLOBAL_HOTKEY_H

#include <QKeySequence>
#include <QString>

struct GlobalHotkeyBinding
{
    quint32 mods = 0;
    quint32 vk = 0;
    bool valid = false;
};

GlobalHotkeyBinding globalHotkeyFromSequence(const QKeySequence &sequence);
QString globalHotkeyDisplayText(const QKeySequence &sequence);
QString globalHotkeyMenuLabel(const QString &actionName, const QKeySequence &sequence);
bool globalHotkeySequenceValid(const QKeySequence &sequence, QString *error = nullptr);

#endif // GLOBAL_HOTKEY_H
