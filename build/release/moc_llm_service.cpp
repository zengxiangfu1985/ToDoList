/****************************************************************************
** Meta object code from reading C++ file 'llm_service.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/core/ai/llm_service.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'llm_service.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LlmService_t {
    QByteArrayData data[12];
    char stringdata0[154];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LlmService_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LlmService_t qt_meta_stringdata_LlmService = {
    {
QT_MOC_LITERAL(0, 0, 10), // "LlmService"
QT_MOC_LITERAL(1, 11, 15), // "analysisStarted"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 16), // "analysisFinished"
QT_MOC_LITERAL(4, 45, 22), // "PriorityAnalysisResult"
QT_MOC_LITERAL(5, 68, 6), // "result"
QT_MOC_LITERAL(6, 75, 15), // "providerChanged"
QT_MOC_LITERAL(7, 91, 15), // "LlmProviderType"
QT_MOC_LITERAL(8, 107, 4), // "type"
QT_MOC_LITERAL(9, 112, 17), // "analyzePriorities"
QT_MOC_LITERAL(10, 130, 17), // "QVector<TaskItem>"
QT_MOC_LITERAL(11, 148, 5) // "tasks"

    },
    "LlmService\0analysisStarted\0\0"
    "analysisFinished\0PriorityAnalysisResult\0"
    "result\0providerChanged\0LlmProviderType\0"
    "type\0analyzePriorities\0QVector<TaskItem>\0"
    "tasks"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LlmService[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    1,   35,    2, 0x06 /* Public */,
       6,    1,   38,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    1,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 10,   11,

       0        // eod
};

void LlmService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LlmService *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->analysisStarted(); break;
        case 1: _t->analysisFinished((*reinterpret_cast< const PriorityAnalysisResult(*)>(_a[1]))); break;
        case 2: _t->providerChanged((*reinterpret_cast< LlmProviderType(*)>(_a[1]))); break;
        case 3: _t->analyzePriorities((*reinterpret_cast< const QVector<TaskItem>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PriorityAnalysisResult >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LlmService::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LlmService::analysisStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LlmService::*)(const PriorityAnalysisResult & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LlmService::analysisFinished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (LlmService::*)(LlmProviderType );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LlmService::providerChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject LlmService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_LlmService.data,
    qt_meta_data_LlmService,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LlmService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LlmService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LlmService.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LlmService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void LlmService::analysisStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void LlmService::analysisFinished(const PriorityAnalysisResult & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void LlmService::providerChanged(LlmProviderType _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
