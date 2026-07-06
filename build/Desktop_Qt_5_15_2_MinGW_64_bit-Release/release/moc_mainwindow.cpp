/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/mainwindow.h"
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
    QByteArrayData data[55];
    char stringdata0[826];
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
QT_MOC_LITERAL(4, 55, 19), // "scheduleReloadTasks"
QT_MOC_LITERAL(5, 75, 12), // "onTodayTasks"
QT_MOC_LITERAL(6, 88, 9), // "onAddTask"
QT_MOC_LITERAL(7, 98, 11), // "onAiAnalyze"
QT_MOC_LITERAL(8, 110, 13), // "onViewAiTrace"
QT_MOC_LITERAL(9, 124, 16), // "onResetQuadrants"
QT_MOC_LITERAL(10, 141, 13), // "onLlmSettings"
QT_MOC_LITERAL(11, 155, 14), // "onM365Settings"
QT_MOC_LITERAL(12, 170, 13), // "onViewHistory"
QT_MOC_LITERAL(13, 184, 22), // "onViewDailyEvaluations"
QT_MOC_LITERAL(14, 207, 20), // "onExportWeeklyReport"
QT_MOC_LITERAL(15, 228, 13), // "onAppSettings"
QT_MOC_LITERAL(16, 242, 17), // "onCheckForUpdates"
QT_MOC_LITERAL(17, 260, 21), // "onImportOfflineUpdate"
QT_MOC_LITERAL(18, 282, 7), // "zipPath"
QT_MOC_LITERAL(19, 290, 20), // "onStartupUpdateCheck"
QT_MOC_LITERAL(20, 311, 13), // "onDeleteTasks"
QT_MOC_LITERAL(21, 325, 18), // "onCancelDeleteMode"
QT_MOC_LITERAL(22, 344, 17), // "onTaskContextMenu"
QT_MOC_LITERAL(23, 362, 3), // "pos"
QT_MOC_LITERAL(24, 366, 19), // "onTaskDoubleClicked"
QT_MOC_LITERAL(25, 386, 11), // "QModelIndex"
QT_MOC_LITERAL(26, 398, 5), // "index"
QT_MOC_LITERAL(27, 404, 22), // "onTaskCompletedToggled"
QT_MOC_LITERAL(28, 427, 6), // "taskId"
QT_MOC_LITERAL(29, 434, 9), // "completed"
QT_MOC_LITERAL(30, 444, 17), // "onQuadrantChanged"
QT_MOC_LITERAL(31, 462, 18), // "EisenhowerQuadrant"
QT_MOC_LITERAL(32, 481, 4), // "from"
QT_MOC_LITERAL(33, 486, 2), // "to"
QT_MOC_LITERAL(34, 489, 17), // "onAnalysisStarted"
QT_MOC_LITERAL(35, 507, 18), // "onAnalysisFinished"
QT_MOC_LITERAL(36, 526, 22), // "PriorityAnalysisResult"
QT_MOC_LITERAL(37, 549, 6), // "result"
QT_MOC_LITERAL(38, 556, 25), // "onDailyEvaluationFinished"
QT_MOC_LITERAL(39, 582, 4), // "date"
QT_MOC_LITERAL(40, 587, 15), // "DailyEvaluation"
QT_MOC_LITERAL(41, 603, 22), // "onWeeklyReportFinished"
QT_MOC_LITERAL(42, 626, 18), // "WeeklyReportRecord"
QT_MOC_LITERAL(43, 645, 6), // "record"
QT_MOC_LITERAL(44, 652, 18), // "WeeklyReportResult"
QT_MOC_LITERAL(45, 671, 17), // "onProviderChanged"
QT_MOC_LITERAL(46, 689, 15), // "LlmProviderType"
QT_MOC_LITERAL(47, 705, 4), // "type"
QT_MOC_LITERAL(48, 710, 13), // "onTop3Clicked"
QT_MOC_LITERAL(49, 724, 14), // "showMainWindow"
QT_MOC_LITERAL(50, 739, 13), // "showTop3Popup"
QT_MOC_LITERAL(51, 753, 15), // "quitApplication"
QT_MOC_LITERAL(52, 769, 15), // "onTrayActivated"
QT_MOC_LITERAL(53, 785, 33), // "QSystemTrayIcon::ActivationRe..."
QT_MOC_LITERAL(54, 819, 6) // "reason"

    },
    "MainWindow\0requestShowFromAnotherInstance\0"
    "\0reloadTasks\0scheduleReloadTasks\0"
    "onTodayTasks\0onAddTask\0onAiAnalyze\0"
    "onViewAiTrace\0onResetQuadrants\0"
    "onLlmSettings\0onM365Settings\0onViewHistory\0"
    "onViewDailyEvaluations\0onExportWeeklyReport\0"
    "onAppSettings\0onCheckForUpdates\0"
    "onImportOfflineUpdate\0zipPath\0"
    "onStartupUpdateCheck\0onDeleteTasks\0"
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
      33,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  179,    2, 0x0a /* Public */,
       3,    0,  180,    2, 0x08 /* Private */,
       4,    0,  181,    2, 0x08 /* Private */,
       5,    0,  182,    2, 0x08 /* Private */,
       6,    0,  183,    2, 0x08 /* Private */,
       7,    0,  184,    2, 0x08 /* Private */,
       8,    0,  185,    2, 0x08 /* Private */,
       9,    0,  186,    2, 0x08 /* Private */,
      10,    0,  187,    2, 0x08 /* Private */,
      11,    0,  188,    2, 0x08 /* Private */,
      12,    0,  189,    2, 0x08 /* Private */,
      13,    0,  190,    2, 0x08 /* Private */,
      14,    0,  191,    2, 0x08 /* Private */,
      15,    0,  192,    2, 0x08 /* Private */,
      16,    0,  193,    2, 0x08 /* Private */,
      17,    1,  194,    2, 0x08 /* Private */,
      19,    0,  197,    2, 0x08 /* Private */,
      20,    0,  198,    2, 0x08 /* Private */,
      21,    0,  199,    2, 0x08 /* Private */,
      22,    1,  200,    2, 0x08 /* Private */,
      24,    1,  203,    2, 0x08 /* Private */,
      27,    2,  206,    2, 0x08 /* Private */,
      30,    3,  211,    2, 0x08 /* Private */,
      34,    0,  218,    2, 0x08 /* Private */,
      35,    1,  219,    2, 0x08 /* Private */,
      38,    2,  222,    2, 0x08 /* Private */,
      41,    2,  227,    2, 0x08 /* Private */,
      45,    1,  232,    2, 0x08 /* Private */,
      48,    0,  235,    2, 0x08 /* Private */,
      49,    0,  236,    2, 0x08 /* Private */,
      50,    0,  237,    2, 0x08 /* Private */,
      51,    0,  238,    2, 0x08 /* Private */,
      52,    1,  239,    2, 0x08 /* Private */,

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
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   23,
    QMetaType::Void, 0x80000000 | 25,   26,
    QMetaType::Void, QMetaType::LongLong, QMetaType::Bool,   28,   29,
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 31, 0x80000000 | 31,   28,   32,   33,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 36,   37,
    QMetaType::Void, QMetaType::QDate, 0x80000000 | 40,   39,   37,
    QMetaType::Void, 0x80000000 | 42, 0x80000000 | 44,   43,   37,
    QMetaType::Void, 0x80000000 | 46,   47,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 53,   54,

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
        case 2: _t->scheduleReloadTasks(); break;
        case 3: _t->onTodayTasks(); break;
        case 4: _t->onAddTask(); break;
        case 5: _t->onAiAnalyze(); break;
        case 6: _t->onViewAiTrace(); break;
        case 7: _t->onResetQuadrants(); break;
        case 8: _t->onLlmSettings(); break;
        case 9: _t->onM365Settings(); break;
        case 10: _t->onViewHistory(); break;
        case 11: _t->onViewDailyEvaluations(); break;
        case 12: _t->onExportWeeklyReport(); break;
        case 13: _t->onAppSettings(); break;
        case 14: _t->onCheckForUpdates(); break;
        case 15: _t->onImportOfflineUpdate((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->onStartupUpdateCheck(); break;
        case 17: _t->onDeleteTasks(); break;
        case 18: _t->onCancelDeleteMode(); break;
        case 19: _t->onTaskContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 20: _t->onTaskDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 21: _t->onTaskCompletedToggled((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 22: _t->onQuadrantChanged((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< EisenhowerQuadrant(*)>(_a[2])),(*reinterpret_cast< EisenhowerQuadrant(*)>(_a[3]))); break;
        case 23: _t->onAnalysisStarted(); break;
        case 24: _t->onAnalysisFinished((*reinterpret_cast< const PriorityAnalysisResult(*)>(_a[1]))); break;
        case 25: _t->onDailyEvaluationFinished((*reinterpret_cast< const QDate(*)>(_a[1])),(*reinterpret_cast< const DailyEvaluation(*)>(_a[2]))); break;
        case 26: _t->onWeeklyReportFinished((*reinterpret_cast< const WeeklyReportRecord(*)>(_a[1])),(*reinterpret_cast< const WeeklyReportResult(*)>(_a[2]))); break;
        case 27: _t->onProviderChanged((*reinterpret_cast< LlmProviderType(*)>(_a[1]))); break;
        case 28: _t->onTop3Clicked(); break;
        case 29: _t->showMainWindow(); break;
        case 30: _t->showTop3Popup(); break;
        case 31: _t->quitApplication(); break;
        case 32: _t->onTrayActivated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 24:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PriorityAnalysisResult >(); break;
            }
            break;
        case 26:
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
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
