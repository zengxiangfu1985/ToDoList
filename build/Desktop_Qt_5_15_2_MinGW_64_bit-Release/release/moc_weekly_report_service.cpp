/****************************************************************************
** Meta object code from reading C++ file 'weekly_report_service.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/core/weekly_report_service.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'weekly_report_service.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WeeklyReportService_t {
    QByteArrayData data[14];
    char stringdata0[191];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WeeklyReportService_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WeeklyReportService_t qt_meta_stringdata_WeeklyReportService = {
    {
QT_MOC_LITERAL(0, 0, 19), // "WeeklyReportService"
QT_MOC_LITERAL(1, 20, 17), // "generationStarted"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 9), // "weekStart"
QT_MOC_LITERAL(4, 49, 7), // "weekEnd"
QT_MOC_LITERAL(5, 57, 18), // "generationFinished"
QT_MOC_LITERAL(6, 76, 18), // "WeeklyReportRecord"
QT_MOC_LITERAL(7, 95, 6), // "record"
QT_MOC_LITERAL(8, 102, 18), // "WeeklyReportResult"
QT_MOC_LITERAL(9, 121, 6), // "result"
QT_MOC_LITERAL(10, 128, 13), // "generateAsync"
QT_MOC_LITERAL(11, 142, 17), // "QVector<TaskItem>"
QT_MOC_LITERAL(12, 160, 13), // "selectedTasks"
QT_MOC_LITERAL(13, 174, 16) // "onFutureFinished"

    },
    "WeeklyReportService\0generationStarted\0"
    "\0weekStart\0weekEnd\0generationFinished\0"
    "WeeklyReportRecord\0record\0WeeklyReportResult\0"
    "result\0generateAsync\0QVector<TaskItem>\0"
    "selectedTasks\0onFutureFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WeeklyReportService[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       5,    2,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    3,   44,    2, 0x0a /* Public */,
      13,    0,   51,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QDate, QMetaType::QDate,    3,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 8,    7,    9,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 11, QMetaType::QDate, QMetaType::QDate,   12,    3,    4,
    QMetaType::Void,

       0        // eod
};

void WeeklyReportService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WeeklyReportService *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->generationStarted((*reinterpret_cast< const QDate(*)>(_a[1])),(*reinterpret_cast< const QDate(*)>(_a[2]))); break;
        case 1: _t->generationFinished((*reinterpret_cast< const WeeklyReportRecord(*)>(_a[1])),(*reinterpret_cast< const WeeklyReportResult(*)>(_a[2]))); break;
        case 2: _t->generateAsync((*reinterpret_cast< const QVector<TaskItem>(*)>(_a[1])),(*reinterpret_cast< const QDate(*)>(_a[2])),(*reinterpret_cast< const QDate(*)>(_a[3]))); break;
        case 3: _t->onFutureFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WeeklyReportRecord >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WeeklyReportService::*)(const QDate & , const QDate & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeeklyReportService::generationStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WeeklyReportService::*)(const WeeklyReportRecord & , const WeeklyReportResult & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeeklyReportService::generationFinished)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject WeeklyReportService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_WeeklyReportService.data,
    qt_meta_data_WeeklyReportService,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *WeeklyReportService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WeeklyReportService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WeeklyReportService.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WeeklyReportService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void WeeklyReportService::generationStarted(const QDate & _t1, const QDate & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WeeklyReportService::generationFinished(const WeeklyReportRecord & _t1, const WeeklyReportResult & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
