/****************************************************************************
** Meta object code from reading C++ file 'weekly_report_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/ui/weekly_report_dialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'weekly_report_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WeeklyReportDialog_t {
    QByteArrayData data[20];
    char stringdata0[277];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WeeklyReportDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WeeklyReportDialog_t qt_meta_stringdata_WeeklyReportDialog = {
    {
QT_MOC_LITERAL(0, 0, 18), // "WeeklyReportDialog"
QT_MOC_LITERAL(1, 19, 18), // "refreshHistoryList"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 25), // "onHistorySelectionChanged"
QT_MOC_LITERAL(4, 65, 11), // "onSelectAll"
QT_MOC_LITERAL(5, 77, 12), // "onSelectNone"
QT_MOC_LITERAL(6, 90, 17), // "onSelectCompleted"
QT_MOC_LITERAL(7, 108, 10), // "onGenerate"
QT_MOC_LITERAL(8, 119, 17), // "onRunInBackground"
QT_MOC_LITERAL(9, 137, 8), // "onDelete"
QT_MOC_LITERAL(10, 146, 10), // "onSaveEdit"
QT_MOC_LITERAL(11, 157, 8), // "onExport"
QT_MOC_LITERAL(12, 166, 19), // "onGenerationStarted"
QT_MOC_LITERAL(13, 186, 9), // "weekStart"
QT_MOC_LITERAL(14, 196, 7), // "weekEnd"
QT_MOC_LITERAL(15, 204, 20), // "onGenerationFinished"
QT_MOC_LITERAL(16, 225, 18), // "WeeklyReportRecord"
QT_MOC_LITERAL(17, 244, 6), // "record"
QT_MOC_LITERAL(18, 251, 18), // "WeeklyReportResult"
QT_MOC_LITERAL(19, 270, 6) // "result"

    },
    "WeeklyReportDialog\0refreshHistoryList\0"
    "\0onHistorySelectionChanged\0onSelectAll\0"
    "onSelectNone\0onSelectCompleted\0"
    "onGenerate\0onRunInBackground\0onDelete\0"
    "onSaveEdit\0onExport\0onGenerationStarted\0"
    "weekStart\0weekEnd\0onGenerationFinished\0"
    "WeeklyReportRecord\0record\0WeeklyReportResult\0"
    "result"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WeeklyReportDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x08 /* Private */,
       3,    0,   75,    2, 0x08 /* Private */,
       4,    0,   76,    2, 0x08 /* Private */,
       5,    0,   77,    2, 0x08 /* Private */,
       6,    0,   78,    2, 0x08 /* Private */,
       7,    0,   79,    2, 0x08 /* Private */,
       8,    0,   80,    2, 0x08 /* Private */,
       9,    0,   81,    2, 0x08 /* Private */,
      10,    0,   82,    2, 0x08 /* Private */,
      11,    0,   83,    2, 0x08 /* Private */,
      12,    2,   84,    2, 0x08 /* Private */,
      15,    2,   89,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QDate, QMetaType::QDate,   13,   14,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 18,   17,   19,

       0        // eod
};

void WeeklyReportDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WeeklyReportDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->refreshHistoryList(); break;
        case 1: _t->onHistorySelectionChanged(); break;
        case 2: _t->onSelectAll(); break;
        case 3: _t->onSelectNone(); break;
        case 4: _t->onSelectCompleted(); break;
        case 5: _t->onGenerate(); break;
        case 6: _t->onRunInBackground(); break;
        case 7: _t->onDelete(); break;
        case 8: _t->onSaveEdit(); break;
        case 9: _t->onExport(); break;
        case 10: _t->onGenerationStarted((*reinterpret_cast< const QDate(*)>(_a[1])),(*reinterpret_cast< const QDate(*)>(_a[2]))); break;
        case 11: _t->onGenerationFinished((*reinterpret_cast< const WeeklyReportRecord(*)>(_a[1])),(*reinterpret_cast< const WeeklyReportResult(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WeeklyReportRecord >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject WeeklyReportDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_WeeklyReportDialog.data,
    qt_meta_data_WeeklyReportDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *WeeklyReportDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WeeklyReportDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WeeklyReportDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int WeeklyReportDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
