/****************************************************************************
** Meta object code from reading C++ file 'quadrantlistwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/quadrantlistwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'quadrantlistwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QuadrantListWidget_t {
    QByteArrayData data[9];
    char stringdata0[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QuadrantListWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QuadrantListWidget_t qt_meta_stringdata_QuadrantListWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "QuadrantListWidget"
QT_MOC_LITERAL(1, 19, 19), // "taskQuadrantChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 6), // "taskId"
QT_MOC_LITERAL(4, 47, 18), // "EisenhowerQuadrant"
QT_MOC_LITERAL(5, 66, 4), // "from"
QT_MOC_LITERAL(6, 71, 2), // "to"
QT_MOC_LITERAL(7, 74, 20), // "taskCompletedToggled"
QT_MOC_LITERAL(8, 95, 9) // "completed"

    },
    "QuadrantListWidget\0taskQuadrantChanged\0"
    "\0taskId\0EisenhowerQuadrant\0from\0to\0"
    "taskCompletedToggled\0completed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QuadrantListWidget[] = {

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

void QuadrantListWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QuadrantListWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->taskQuadrantChanged((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< EisenhowerQuadrant(*)>(_a[2])),(*reinterpret_cast< EisenhowerQuadrant(*)>(_a[3]))); break;
        case 1: _t->taskCompletedToggled((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QuadrantListWidget::*)(qint64 , EisenhowerQuadrant , EisenhowerQuadrant );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QuadrantListWidget::taskQuadrantChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (QuadrantListWidget::*)(qint64 , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QuadrantListWidget::taskCompletedToggled)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QuadrantListWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QListWidget::staticMetaObject>(),
    qt_meta_stringdata_QuadrantListWidget.data,
    qt_meta_data_QuadrantListWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QuadrantListWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QuadrantListWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QuadrantListWidget.stringdata0))
        return static_cast<void*>(this);
    return QListWidget::qt_metacast(_clname);
}

int QuadrantListWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
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
void QuadrantListWidget::taskQuadrantChanged(qint64 _t1, EisenhowerQuadrant _t2, EisenhowerQuadrant _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QuadrantListWidget::taskCompletedToggled(qint64 _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
