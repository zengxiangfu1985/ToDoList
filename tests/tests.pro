QT += core testlib sql network

CONFIG += c++17 console
CONFIG -= app_bundle

INCLUDEPATH += ../src

SOURCES += \
    tst_core.cpp \
    ../src/core/task_types.cpp \
    ../src/core/task_archive.cpp \
    ../src/core/task_repository.cpp \
    ../src/core/priority_engine.cpp \
    ../src/core/behavior_learning_engine.cpp \
    ../src/core/ai/llm_provider.cpp \
    ../src/core/ai/ollama_llm_provider.cpp \
    ../src/core/ai/openai_compatible_provider.cpp \
    ../src/utils/portable_settings.cpp \
    ../src/utils/app_logger.cpp

HEADERS += \
    ../src/core/task_types.h \
    ../src/core/task_archive.h \
    ../src/core/task_repository.h \
    ../src/core/priority_engine.h \
    ../src/core/behavior_learning_engine.h \
    ../src/core/ai/llm_provider.h \
    ../src/core/ai/ollama_llm_provider.h \
    ../src/core/ai/openai_compatible_provider.h \
    ../src/utils/portable_settings.h \
    ../src/utils/app_logger.h

TARGET = tst_core
