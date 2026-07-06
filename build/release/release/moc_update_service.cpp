/****************************************************************************
** Meta object code from reading C++ file 'update_service.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/core/update/update_service.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'update_service.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UpdateService_t {
    QByteArrayData data[22];
    char stringdata0[257];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UpdateService_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UpdateService_t qt_meta_stringdata_UpdateService = {
    {
QT_MOC_LITERAL(0, 0, 13), // "UpdateService"
QT_MOC_LITERAL(1, 14, 12), // "stateChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 20), // "UpdateService::State"
QT_MOC_LITERAL(4, 49, 5), // "state"
QT_MOC_LITERAL(5, 55, 13), // "checkFinished"
QT_MOC_LITERAL(6, 69, 9), // "hasUpdate"
QT_MOC_LITERAL(7, 79, 23), // "downloadProgressChanged"
QT_MOC_LITERAL(8, 103, 7), // "percent"
QT_MOC_LITERAL(9, 111, 16), // "downloadFinished"
QT_MOC_LITERAL(10, 128, 7), // "success"
QT_MOC_LITERAL(11, 136, 13), // "errorOccurred"
QT_MOC_LITERAL(12, 150, 7), // "message"
QT_MOC_LITERAL(13, 158, 15), // "onCheckFinished"
QT_MOC_LITERAL(14, 174, 19), // "onDownloadReadyRead"
QT_MOC_LITERAL(15, 194, 18), // "onDownloadFinished"
QT_MOC_LITERAL(16, 213, 5), // "State"
QT_MOC_LITERAL(17, 219, 4), // "Idle"
QT_MOC_LITERAL(18, 224, 8), // "Checking"
QT_MOC_LITERAL(19, 233, 11), // "Downloading"
QT_MOC_LITERAL(20, 245, 5), // "Ready"
QT_MOC_LITERAL(21, 251, 5) // "Error"

    },
    "UpdateService\0stateChanged\0\0"
    "UpdateService::State\0state\0checkFinished\0"
    "hasUpdate\0downloadProgressChanged\0"
    "percent\0downloadFinished\0success\0"
    "errorOccurred\0message\0onCheckFinished\0"
    "onDownloadReadyRead\0onDownloadFinished\0"
    "State\0Idle\0Checking\0Downloading\0Ready\0"
    "Error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UpdateService[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       1,   72, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       5,    1,   57,    2, 0x06 /* Public */,
       7,    1,   60,    2, 0x06 /* Public */,
       9,    1,   63,    2, 0x06 /* Public */,
      11,    1,   66,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    0,   69,    2, 0x08 /* Private */,
      14,    0,   70,    2, 0x08 /* Private */,
      15,    0,   71,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::QString,   12,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // enums: name, alias, flags, count, data
      16,   16, 0x2,    5,   77,

 // enum data: key, value
      17, uint(UpdateService::State::Idle),
      18, uint(UpdateService::State::Checking),
      19, uint(UpdateService::State::Downloading),
      20, uint(UpdateService::State::Ready),
      21, uint(UpdateService::State::Error),

       0        // eod
};

void UpdateService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UpdateService *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stateChanged((*reinterpret_cast< UpdateService::State(*)>(_a[1]))); break;
        case 1: _t->checkFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->downloadProgressChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->downloadFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->onCheckFinished(); break;
        case 6: _t->onDownloadReadyRead(); break;
        case 7: _t->onDownloadFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UpdateService::*)(UpdateService::State );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UpdateService::stateChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UpdateService::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UpdateService::checkFinished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (UpdateService::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UpdateService::downloadProgressChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (UpdateService::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UpdateService::downloadFinished)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (UpdateService::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UpdateService::errorOccurred)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UpdateService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_UpdateService.data,
    qt_meta_data_UpdateService,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UpdateService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UpdateService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UpdateService.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UpdateService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void UpdateService::stateChanged(UpdateService::State _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UpdateService::checkFinished(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void UpdateService::downloadProgressChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void UpdateService::downloadFinished(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void UpdateService::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
