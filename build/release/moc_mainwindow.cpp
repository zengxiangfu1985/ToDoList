/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/ui/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[50];
    char stringdata0[737];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 30), // "requestShowFromAnotherInstance"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 11), // "reloadTasks"
QT_MOC_LITERAL(4, 55, 12), // "onTodayTasks"
QT_MOC_LITERAL(5, 68, 9), // "onAddTask"
QT_MOC_LITERAL(6, 78, 11), // "onAiAnalyze"
QT_MOC_LITERAL(7, 90, 13), // "onViewAiTrace"
QT_MOC_LITERAL(8, 104, 16), // "onResetQuadrants"
QT_MOC_LITERAL(9, 121, 13), // "onLlmSettings"
QT_MOC_LITERAL(10, 135, 14), // "onM365Settings"
QT_MOC_LITERAL(11, 150, 13), // "onViewHistory"
QT_MOC_LITERAL(12, 164, 22), // "onViewDailyEvaluations"
QT_MOC_LITERAL(13, 187, 20), // "onExportWeeklyReport"
QT_MOC_LITERAL(14, 208, 13), // "onAppSettings"
QT_MOC_LITERAL(15, 222, 13), // "onDeleteTasks"
QT_MOC_LITERAL(16, 236, 18), // "onCancelDeleteMode"
QT_MOC_LITERAL(17, 255, 17), // "onTaskContextMenu"
QT_MOC_LITERAL(18, 273, 3), // "pos"
QT_MOC_LITERAL(19, 277, 19), // "onTaskDoubleClicked"
QT_MOC_LITERAL(20, 297, 11), // "QModelIndex"
QT_MOC_LITERAL(21, 309, 5), // "index"
QT_MOC_LITERAL(22, 315, 22), // "onTaskCompletedToggled"
QT_MOC_LITERAL(23, 338, 6), // "taskId"
QT_MOC_LITERAL(24, 345, 9), // "completed"
QT_MOC_LITERAL(25, 355, 17), // "onQuadrantChanged"
QT_MOC_LITERAL(26, 373, 18), // "EisenhowerQuadrant"
QT_MOC_LITERAL(27, 392, 4), // "from"
QT_MOC_LITERAL(28, 397, 2), // "to"
QT_MOC_LITERAL(29, 400, 17), // "onAnalysisStarted"
QT_MOC_LITERAL(30, 418, 18), // "onAnalysisFinished"
QT_MOC_LITERAL(31, 437, 22), // "PriorityAnalysisResult"
QT_MOC_LITERAL(32, 460, 6), // "result"
QT_MOC_LITERAL(33, 467, 25), // "onDailyEvaluationFinished"
QT_MOC_LITERAL(34, 493, 4), // "date"
QT_MOC_LITERAL(35, 498, 15), // "DailyEvaluation"
QT_MOC_LITERAL(36, 514, 22), // "onWeeklyReportFinished"
QT_MOC_LITERAL(37, 537, 18), // "WeeklyReportRecord"
QT_MOC_LITERAL(38, 556, 6), // "record"
QT_MOC_LITERAL(39, 563, 18), // "WeeklyReportResult"
QT_MOC_LITERAL(40, 582, 17), // "onProviderChanged"
QT_MOC_LITERAL(41, 600, 15), // "LlmProviderType"
QT_MOC_LITERAL(42, 616, 4), // "type"
QT_MOC_LITERAL(43, 621, 13), // "onTop3Clicked"
QT_MOC_LITERAL(44, 635, 14), // "showMainWindow"
QT_MOC_LITERAL(45, 650, 13), // "showTop3Popup"
QT_MOC_LITERAL(46, 664, 15), // "quitApplication"
QT_MOC_LITERAL(47, 680, 15), // "onTrayActivated"
QT_MOC_LITERAL(48, 696, 33), // "QSystemTrayIcon::ActivationRe..."
QT_MOC_LITERAL(49, 730, 6) // "reason"

    },
    "MainWindow\0requestShowFromAnotherInstance\0"
    "\0reloadTasks\0onTodayTasks\0onAddTask\0"
    "onAiAnalyze\0onViewAiTrace\0onResetQuadrants\0"
    "onLlmSettings\0onM365Settings\0onViewHistory\0"
    "onViewDailyEvaluations\0onExportWeeklyReport\0"
    "onAppSettings\0onDeleteTasks\0"
    "onCancelDeleteMode\0onTaskContextMenu\0"
    "pos\0onTaskDoubleClicked\0QModelIndex\0"
    "index\0onTaskCompletedToggled\0taskId\0"
    "completed\0onQuadrantChanged\0"
    "EisenhowerQuadrant\0from\0to\0onAnalysisStarted\0"
    "onAnalysisFinished\0PriorityAnalysisResult\0"
    "result\0onDailyEvaluationFinished\0date\0"
    "DailyEvaluation\0onWeeklyReportFinished\0"
    "WeeklyReportRecord\0record\0WeeklyReportResult\0"
    "onProviderChanged\0LlmProviderType\0"
    "type\0onTop3Clicked\0showMainWindow\0"
    "showTop3Popup\0quitApplication\0"
    "onTrayActivated\0QSystemTrayIcon::ActivationReason\0"
    "reason"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  159,    2, 0x0a /* Public */,
       3,    0,  160,    2, 0x08 /* Private */,
       4,    0,  161,    2, 0x08 /* Private */,
       5,    0,  162,    2, 0x08 /* Private */,
       6,    0,  163,    2, 0x08 /* Private */,
       7,    0,  164,    2, 0x08 /* Private */,
       8,    0,  165,    2, 0x08 /* Private */,
       9,    0,  166,    2, 0x08 /* Private */,
      10,    0,  167,    2, 0x08 /* Private */,
      11,    0,  168,    2, 0x08 /* Private */,
      12,    0,  169,    2, 0x08 /* Private */,
      13,    0,  170,    2, 0x08 /* Private */,
      14,    0,  171,    2, 0x08 /* Private */,
      15,    0,  172,    2, 0x08 /* Private */,
      16,    0,  173,    2, 0x08 /* Private */,
      17,    1,  174,    2, 0x08 /* Private */,
      19,    1,  177,    2, 0x08 /* Private */,
      22,    2,  180,    2, 0x08 /* Private */,
      25,    3,  185,    2, 0x08 /* Private */,
      29,    0,  192,    2, 0x08 /* Private */,
      30,    1,  193,    2, 0x08 /* Private */,
      33,    2,  196,    2, 0x08 /* Private */,
      36,    2,  201,    2, 0x08 /* Private */,
      40,    1,  206,    2, 0x08 /* Private */,
      43,    0,  209,    2, 0x08 /* Private */,
      44,    0,  210,    2, 0x08 /* Private */,
      45,    0,  211,    2, 0x08 /* Private */,
      46,    0,  212,    2, 0x08 /* Private */,
      47,    1,  213,    2, 0x08 /* Private */,

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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   18,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void, QMetaType::LongLong, QMetaType::Bool,   23,   24,
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 26, 0x80000000 | 26,   23,   27,   28,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 31,   32,
    QMetaType::Void, QMetaType::QDate, 0x80000000 | 35,   34,   32,
    QMetaType::Void, 0x80000000 | 37, 0x80000000 | 39,   38,   32,
    QMetaType::Void, 0x80000000 | 41,   42,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 48,   49,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->requestShowFromAnotherInstance(); break;
        case 1: _t->reloadTasks(); break;
        case 2: _t->onTodayTasks(); break;
        case 3: _t->onAddTask(); break;
        case 4: _t->onAiAnalyze(); break;
        case 5: _t->onViewAiTrace(); break;
        case 6: _t->onResetQuadrants(); break;
        case 7: _t->onLlmSettings(); break;
        case 8: _t->onM365Settings(); break;
        case 9: _t->onViewHistory(); break;
        case 10: _t->onViewDailyEvaluations(); break;
        case 11: _t->onExportWeeklyReport(); break;
        case 12: _t->onAppSettings(); break;
        case 13: _t->onDeleteTasks(); break;
        case 14: _t->onCancelDeleteMode(); break;
        case 15: _t->onTaskContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 16: _t->onTaskDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 17: _t->onTaskCompletedToggled((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 18: _t->onQuadrantChanged((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< EisenhowerQuadrant(*)>(_a[2])),(*reinterpret_cast< EisenhowerQuadrant(*)>(_a[3]))); break;
        case 19: _t->onAnalysisStarted(); break;
        case 20: _t->onAnalysisFinished((*reinterpret_cast< const PriorityAnalysisResult(*)>(_a[1]))); break;
        case 21: _t->onDailyEvaluationFinished((*reinterpret_cast< const QDate(*)>(_a[1])),(*reinterpret_cast< const DailyEvaluation(*)>(_a[2]))); break;
        case 22: _t->onWeeklyReportFinished((*reinterpret_cast< const WeeklyReportRecord(*)>(_a[1])),(*reinterpret_cast< const WeeklyReportResult(*)>(_a[2]))); break;
        case 23: _t->onProviderChanged((*reinterpret_cast< LlmProviderType(*)>(_a[1]))); break;
        case 24: _t->onTop3Clicked(); break;
        case 25: _t->showMainWindow(); break;
        case 26: _t->showTop3Popup(); break;
        case 27: _t->quitApplication(); break;
        case 28: _t->onTrayActivated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 20:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PriorityAnalysisResult >(); break;
            }
            break;
        case 22:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WeeklyReportRecord >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
