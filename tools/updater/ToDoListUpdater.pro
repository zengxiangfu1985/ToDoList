QT += core
CONFIG += console c++17

TARGET = ToDoListUpdater
TEMPLATE = app

INCLUDEPATH += ../../src

SOURCES += \
    main.cpp \
    ../../src/core/update/update_apply.cpp \
    ../../src/core/update/update_manifest.cpp

HEADERS += \
    ../../src/core/update/update_apply.h \
    ../../src/core/update/update_manifest.h

win32: LIBS += -luser32
