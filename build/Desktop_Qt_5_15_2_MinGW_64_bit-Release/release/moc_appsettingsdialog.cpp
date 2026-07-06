/****************************************************************************
** Meta object code from reading C++ file 'appsettingsdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/appsettingsdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'appsettingsdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AppSettingsDialog_t {
    QByteArrayData data[16];
    char stringdata0[247];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AppSettingsDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AppSettingsDialog_t qt_meta_stringdata_AppSettingsDialog = {
    {
QT_MOC_LITERAL(0, 0, 17), // "AppSettingsDialog"
QT_MOC_LITERAL(1, 18, 13), // "lockRequested"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 20), // "viewAiTraceRequested"
QT_MOC_LITERAL(4, 54, 21), // "m365SettingsRequested"
QT_MOC_LITERAL(5, 76, 14), // "hotkeysChanged"
QT_MOC_LITERAL(6, 91, 20), // "checkUpdateRequested"
QT_MOC_LITERAL(7, 112, 28), // "importOfflineUpdateRequested"
QT_MOC_LITERAL(8, 141, 7), // "zipPath"
QT_MOC_LITERAL(9, 149, 6), // "onSave"
QT_MOC_LITERAL(10, 156, 15), // "onClearPassword"
QT_MOC_LITERAL(11, 172, 9), // "onLockNow"
QT_MOC_LITERAL(12, 182, 13), // "onViewAiTrace"
QT_MOC_LITERAL(13, 196, 14), // "onM365Settings"
QT_MOC_LITERAL(14, 211, 13), // "onCheckUpdate"
QT_MOC_LITERAL(15, 225, 21) // "onImportOfflineUpdate"

    },
    "AppSettingsDialog\0lockRequested\0\0"
    "viewAiTraceRequested\0m365SettingsRequested\0"
    "hotkeysChanged\0checkUpdateRequested\0"
    "importOfflineUpdateRequested\0zipPath\0"
    "onSave\0onClearPassword\0onLockNow\0"
    "onViewAiTrace\0onM365Settings\0onCheckUpdate\0"
    "onImportOfflineUpdate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AppSettingsDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x06 /* Public */,
       3,    0,   80,    2, 0x06 /* Public */,
       4,    0,   81,    2, 0x06 /* Public */,
       5,    0,   82,    2, 0x06 /* Public */,
       6,    0,   83,    2, 0x06 /* Public */,
       7,    1,   84,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   87,    2, 0x08 /* Private */,
      10,    0,   88,    2, 0x08 /* Private */,
      11,    0,   89,    2, 0x08 /* Private */,
      12,    0,   90,    2, 0x08 /* Private */,
      13,    0,   91,    2, 0x08 /* Private */,
      14,    0,   92,    2, 0x08 /* Private */,
      15,    0,   93,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void AppSettingsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AppSettingsDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->lockRequested(); break;
        case 1: _t->viewAiTraceRequested(); break;
        case 2: _t->m365SettingsRequested(); break;
        case 3: _t->hotkeysChanged(); break;
        case 4: _t->checkUpdateRequested(); break;
        case 5: _t->importOfflineUpdateRequested((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->onSave(); break;
        case 7: _t->onClearPassword(); break;
        case 8: _t->onLockNow(); break;
        case 9: _t->onViewAiTrace(); break;
        case 10: _t->onM365Settings(); break;
        case 11: _t->onCheckUpdate(); break;
        case 12: _t->onImportOfflineUpdate(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AppSettingsDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppSettingsDialog::lockRequested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AppSettingsDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppSettingsDialog::viewAiTraceRequested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AppSettingsDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppSettingsDialog::m365SettingsRequested)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (AppSettingsDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppSettingsDialog::hotkeysChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (AppSettingsDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppSettingsDialog::checkUpdateRequested)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (AppSettingsDialog::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AppSettingsDialog::importOfflineUpdateRequested)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AppSettingsDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_AppSettingsDialog.data,
    qt_meta_data_AppSettingsDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AppSettingsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppSettingsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AppSettingsDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int AppSettingsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void AppSettingsDialog::lockRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AppSettingsDialog::viewAiTraceRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AppSettingsDialog::m365SettingsRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void AppSettingsDialog::hotkeysChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void AppSettingsDialog::checkUpdateRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void AppSettingsDialog::importOfflineUpdateRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
