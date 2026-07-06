/****************************************************************************
** Meta object code from reading C++ file 'quadrantboard.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/quadrantboard.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'quadrantboard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QuadrantBoard_t {
    QByteArrayData data[9];
    char stringdata0[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QuadrantBoard_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QuadrantBoard_t qt_meta_stringdata_QuadrantBoard = {
    {
QT_MOC_LITERAL(0, 0, 13), // "QuadrantBoard"
QT_MOC_LITERAL(1, 14, 19), // "taskQuadrantChanged"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 6), // "taskId"
QT_MOC_LITERAL(4, 42, 18), // "EisenhowerQuadrant"
QT_MOC_LITERAL(5, 61, 4), // "from"
QT_MOC_LITERAL(6, 66, 2), // "to"
QT_MOC_LITERAL(7, 69, 20), // "taskCompletedToggled"
QT_MOC_LITERAL(8, 90, 9) // "completed"

    },
    "QuadrantBoard\0taskQuadrantChanged\0\0"
    "taskId\0EisenhowerQuadrant\0from\0to\0"
    "taskCompletedToggled\0completed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QuadrantBoard[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   24,    2, 0x06 /* Public */,
       7,    2,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 4, 0x80000000 | 4,    3,    5,    6,
    QMetaType::Void, QMetaType::LongLong, QMetaType::Bool,    3,    8,

       0        // eod
};

void QuadrantBoard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QuadrantBoard *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->taskQuadrantChanged((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< EisenhowerQuadrant(*)>(_a[2])),(*reinterpret_cast< EisenhowerQuadrant(*)>(_a[3]))); break;
        case 1: _t->taskCompletedToggled((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QuadrantBoard::*)(qint64 , EisenhowerQuadrant , EisenhowerQuadrant );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QuadrantBoard::taskQuadrantChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (QuadrantBoard::*)(qint64 , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QuadrantBoard::taskCompletedToggled)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QuadrantBoard::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_QuadrantBoard.data,
    qt_meta_data_QuadrantBoard,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QuadrantBoard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QuadrantBoard::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QuadrantBoard.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int QuadrantBoard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QuadrantBoard::taskQuadrantChanged(qint64 _t1, EisenhowerQuadrant _t2, EisenhowerQuadrant _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QuadrantBoard::taskCompletedToggled(qint64 _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
