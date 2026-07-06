/****************************************************************************
** Meta object code from reading C++ file 'daily_evaluation_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/daily_evaluation_dialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'daily_evaluation_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DailyEvaluationDialog_t {
    QByteArrayData data[12];
    char stringdata0[161];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DailyEvaluationDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DailyEvaluationDialog_t qt_meta_stringdata_DailyEvaluationDialog = {
    {
QT_MOC_LITERAL(0, 0, 21), // "DailyEvaluationDialog"
QT_MOC_LITERAL(1, 22, 11), // "refreshList"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 23), // "onTableSelectionChanged"
QT_MOC_LITERAL(4, 59, 12), // "onRegenerate"
QT_MOC_LITERAL(5, 72, 8), // "onDelete"
QT_MOC_LITERAL(6, 81, 10), // "onSaveEdit"
QT_MOC_LITERAL(7, 92, 19), // "onEvaluationStarted"
QT_MOC_LITERAL(8, 112, 4), // "date"
QT_MOC_LITERAL(9, 117, 20), // "onEvaluationFinished"
QT_MOC_LITERAL(10, 138, 15), // "DailyEvaluation"
QT_MOC_LITERAL(11, 154, 6) // "result"

    },
    "DailyEvaluationDialog\0refreshList\0\0"
    "onTableSelectionChanged\0onRegenerate\0"
    "onDelete\0onSaveEdit\0onEvaluationStarted\0"
    "date\0onEvaluationFinished\0DailyEvaluation\0"
    "result"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DailyEvaluationDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x08 /* Private */,
       3,    0,   50,    2, 0x08 /* Private */,
       4,    0,   51,    2, 0x08 /* Private */,
       5,    0,   52,    2, 0x08 /* Private */,
       6,    0,   53,    2, 0x08 /* Private */,
       7,    1,   54,    2, 0x08 /* Private */,
       9,    2,   57,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QDate,    8,
    QMetaType::Void, QMetaType::QDate, 0x80000000 | 10,    8,   11,

       0        // eod
};

void DailyEvaluationDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DailyEvaluationDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->refreshList(); break;
        case 1: _t->onTableSelectionChanged(); break;
        case 2: _t->onRegenerate(); break;
        case 3: _t->onDelete(); break;
        case 4: _t->onSaveEdit(); break;
        case 5: _t->onEvaluationStarted((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 6: _t->onEvaluationFinished((*reinterpret_cast< const QDate(*)>(_a[1])),(*reinterpret_cast< const DailyEvaluation(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DailyEvaluationDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_DailyEvaluationDialog.data,
    qt_meta_data_DailyEvaluationDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DailyEvaluationDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DailyEvaluationDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DailyEvaluationDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DailyEvaluationDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
