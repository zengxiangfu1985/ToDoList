/****************************************************************************
** Meta object code from reading C++ file 'daily_evaluation_service.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/core/daily_evaluation_service.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'daily_evaluation_service.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DailyEvaluationService_t {
    QByteArrayData data[14];
    char stringdata0[211];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DailyEvaluationService_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DailyEvaluationService_t qt_meta_stringdata_DailyEvaluationService = {
    {
QT_MOC_LITERAL(0, 0, 22), // "DailyEvaluationService"
QT_MOC_LITERAL(1, 23, 17), // "evaluationStarted"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 4), // "date"
QT_MOC_LITERAL(4, 47, 18), // "evaluationFinished"
QT_MOC_LITERAL(5, 66, 15), // "DailyEvaluation"
QT_MOC_LITERAL(6, 82, 6), // "result"
QT_MOC_LITERAL(7, 89, 15), // "catchUpFinished"
QT_MOC_LITERAL(8, 105, 14), // "evaluatedCount"
QT_MOC_LITERAL(9, 120, 10), // "runCatchUp"
QT_MOC_LITERAL(10, 131, 17), // "evaluateDateAsync"
QT_MOC_LITERAL(11, 149, 19), // "regenerateDateAsync"
QT_MOC_LITERAL(12, 169, 14), // "onScheduleTick"
QT_MOC_LITERAL(13, 184, 26) // "onEvaluationFutureFinished"

    },
    "DailyEvaluationService\0evaluationStarted\0"
    "\0date\0evaluationFinished\0DailyEvaluation\0"
    "result\0catchUpFinished\0evaluatedCount\0"
    "runCatchUp\0evaluateDateAsync\0"
    "regenerateDateAsync\0onScheduleTick\0"
    "onEvaluationFutureFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DailyEvaluationService[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    2,   57,    2, 0x06 /* Public */,
       7,    1,   62,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   65,    2, 0x0a /* Public */,
      10,    1,   66,    2, 0x0a /* Public */,
      11,    1,   69,    2, 0x0a /* Public */,
      12,    0,   72,    2, 0x08 /* Private */,
      13,    0,   73,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QDate,    3,
    QMetaType::Void, QMetaType::QDate, 0x80000000 | 5,    3,    6,
    QMetaType::Void, QMetaType::Int,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QDate,    3,
    QMetaType::Void, QMetaType::QDate,    3,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DailyEvaluationService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DailyEvaluationService *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->evaluationStarted((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 1: _t->evaluationFinished((*reinterpret_cast< const QDate(*)>(_a[1])),(*reinterpret_cast< const DailyEvaluation(*)>(_a[2]))); break;
        case 2: _t->catchUpFinished((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->runCatchUp(); break;
        case 4: _t->evaluateDateAsync((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 5: _t->regenerateDateAsync((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 6: _t->onScheduleTick(); break;
        case 7: _t->onEvaluationFutureFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DailyEvaluationService::*)(const QDate & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DailyEvaluationService::evaluationStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DailyEvaluationService::*)(const QDate & , const DailyEvaluation & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DailyEvaluationService::evaluationFinished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DailyEvaluationService::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DailyEvaluationService::catchUpFinished)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DailyEvaluationService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DailyEvaluationService.data,
    qt_meta_data_DailyEvaluationService,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DailyEvaluationService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DailyEvaluationService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DailyEvaluationService.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DailyEvaluationService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void DailyEvaluationService::evaluationStarted(const QDate & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DailyEvaluationService::evaluationFinished(const QDate & _t1, const DailyEvaluation & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DailyEvaluationService::catchUpFinished(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
