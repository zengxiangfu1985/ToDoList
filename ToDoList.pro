QT += core gui sql network concurrent svg
win32: QT += winextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += src

SOURCES += \
    main.cpp \
    src/core/task_types.cpp \
    src/core/task_repository.cpp \
    src/core/priority_engine.cpp \
    src/core/behavior_learning_engine.cpp \
    src/core/task_archive.cpp \
    src/core/daily_evaluation_service.cpp \
    src/core/weekly_report_service.cpp \
    src/core/app_settings.cpp \
    src/core/quick_capture_service.cpp \
    src/core/focus/focus_session_service.cpp \
    src/core/schema_migrator.cpp \
    src/core/update/update_manifest.cpp \
    src/core/update/update_config.cpp \
    src/core/update/update_apply.cpp \
    src/core/update/update_service.cpp \
    src/core/telemetry/usage_report_config.cpp \
    src/core/telemetry/usage_report_service.cpp \
    src/core/ai/llm_provider.cpp \
    src/core/ai/ai_prompts.cpp \
    src/core/ai/ollama_llm_provider.cpp \
    src/core/ai/openai_compatible_provider.cpp \
    src/core/ai/llm_service.cpp \
    src/core/m365/m365_auth_service.cpp \
    src/core/m365/microsoft_graph_client.cpp \
    src/core/m365/m365_sync_service.cpp \
    src/models/task_table_model.cpp \
    src/ui/mainwindow.cpp \
    src/ui/addtaskdialog.cpp \
    src/ui/todaytasksdialog.cpp \
    src/ui/quick_capture_dialog.cpp \
    src/ui/focus_session_dialog.cpp \
    src/ui/line_number_plain_text_edit.cpp \
    src/ui/llmsettingsdialog.cpp \
    src/ui/top3popupdialog.cpp \
    src/ui/top3listwidget.cpp \
    src/ui/m365settingsdialog.cpp \
    src/ui/taskhistorydialog.cpp \
    src/ui/daily_evaluation_dialog.cpp \
    src/ui/weekly_report_dialog.cpp \
    src/ui/aianalysistracedialog.cpp \
    src/ui/quadrantlistwidget.cpp \
    src/ui/quadrantboard.cpp \
    src/ui/cyber_quadrant_box.cpp \
    src/ui/cyber_ai_panel.cpp \
    src/ui/task_table_delegate.cpp \
    src/ui/windowtitlebar.cpp \
    src/ui/appsettingsdialog.cpp \
    src/ui/aboutdialog.cpp \
    src/ui/updatedialog.cpp \
    src/ui/lock_screen_widget.cpp \
    src/ui/ai_busy_overlay.cpp \
    src/utils/windows_hotkey_manager.cpp \
    src/utils/global_hotkey.cpp \
    src/utils/window_fit.cpp \
    src/utils/app_icon.cpp \
    src/utils/single_instance.cpp \
    src/utils/windows_session_monitor.cpp \
    src/utils/app_theme.cpp \
    src/utils/ssl_support.cpp \
    src/utils/app_logger.cpp \
    src/utils/app_translations.cpp \
    src/utils/app_version.cpp \
    src/utils/portable_settings.cpp \
    src/utils/llm_json_utils.cpp \
    src/utils/ai_analysis_trace.cpp

HEADERS += \
    src/core/task_types.h \
    src/core/task_repository.h \
    src/core/priority_engine.h \
    src/core/behavior_learning_engine.h \
    src/core/task_archive.h \
    src/core/daily_evaluation_service.h \
    src/core/weekly_report_service.h \
    src/core/app_settings.h \
    src/core/quick_capture_service.h \
    src/core/focus/focus_session_service.h \
    src/core/schema_migrator.h \
    src/core/update/update_manifest.h \
    src/core/update/update_config.h \
    src/core/update/update_apply.h \
    src/core/update/update_service.h \
    src/core/telemetry/usage_report_config.h \
    src/core/telemetry/usage_report_service.h \
    src/core/ai/llm_provider.h \
    src/core/ai/ai_prompts.h \
    src/core/ai/ollama_llm_provider.h \
    src/core/ai/openai_compatible_provider.h \
    src/core/ai/llm_service.h \
    src/core/m365/m365_auth_service.h \
    src/core/m365/microsoft_graph_client.h \
    src/core/m365/m365_sync_service.h \
    src/models/task_table_model.h \
    src/ui/mainwindow.h \
    src/ui/addtaskdialog.h \
    src/ui/todaytasksdialog.h \
    src/ui/quick_capture_dialog.h \
    src/ui/focus_session_dialog.h \
    src/ui/line_number_plain_text_edit.h \
    src/ui/llmsettingsdialog.h \
    src/ui/top3popupdialog.h \
    src/ui/top3listwidget.h \
    src/ui/task_checkbox_utils.h \
    src/ui/m365settingsdialog.h \
    src/ui/taskhistorydialog.h \
    src/ui/daily_evaluation_dialog.h \
    src/ui/weekly_report_dialog.h \
    src/ui/aianalysistracedialog.h \
    src/ui/quadrantlistwidget.h \
    src/ui/quadrantboard.h \
    src/ui/cyber_quadrant_box.h \
    src/ui/cyber_ai_panel.h \
    src/ui/task_table_delegate.h \
    src/ui/windowtitlebar.h \
    src/ui/appsettingsdialog.h \
    src/ui/aboutdialog.h \
    src/ui/updatedialog.h \
    src/ui/lock_screen_widget.h \
    src/ui/ai_busy_overlay.h \
    src/utils/windows_hotkey_manager.h \
    src/utils/global_hotkey.h \
    src/utils/window_fit.h \
    src/utils/app_icon.h \
    src/utils/single_instance.h \
    src/utils/windows_session_monitor.h \
    src/utils/app_theme.h \
    src/utils/ssl_support.h \
    src/utils/app_logger.h \
    src/utils/app_translations.h \
    src/utils/app_version.h \
    src/utils/portable_settings.h \
    src/utils/llm_json_utils.h \
    src/utils/ai_analysis_trace.h

FORMS += \
    src/ui/mainwindow.ui \
    src/ui/addtaskdialog.ui \
    src/ui/quick_capture_dialog.ui \
    src/ui/focus_session_dialog.ui \
    src/ui/llmsettingsdialog.ui \
    src/ui/top3popupdialog.ui \
    src/ui/m365settingsdialog.ui \
    src/ui/appsettingsdialog.ui \
    src/ui/aboutdialog.ui \
    src/ui/updatedialog.ui

RESOURCES += resources/resources.qrc

TRANSLATIONS += translations/todolist_en.ts

win32: RC_ICONS = resources/icons/app.ico

# Regenerate icon files from app_source.png before each link.
win32 {
    APP_ICON_ICO = $$PWD/resources/icons/app.ico
    APP_ICON_PNG = $$PWD/resources/icons/app.png
    APP_ICON_SRC = $$PWD/resources/icons/app_source.png
    APP_ICON_GEN = $$PWD/scripts/generate-app-icon.py
    CONFIG(release, debug|release): ICON_BUILD_DIR = release
    CONFIG(debug, debug|release): ICON_BUILD_DIR = debug

    gen_app_icon.commands = python $$shell_path($$APP_ICON_GEN)
    gen_app_icon.depends = $$APP_ICON_SRC $$APP_ICON_GEN
    gen_app_icon.target = $$APP_ICON_ICO
    QMAKE_EXTRA_TARGETS += gen_app_icon

    # windres only watches ToDoList_resource.rc; drop stale .o when app.ico changes.
    win_res_icon_sync.target = $$OUT_PWD/$$ICON_BUILD_DIR/.win_res_icon_sync
    win_res_icon_sync.depends = $$APP_ICON_ICO
    win_res_icon_sync.commands = $$QMAKE_DEL_FILE $$OUT_PWD/$$ICON_BUILD_DIR/ToDoList_resource_res.o $$escape_expand(\\n\\t) echo synced > $$OUT_PWD/$$ICON_BUILD_DIR/.win_res_icon_sync
    QMAKE_EXTRA_TARGETS += win_res_icon_sync

    PRE_TARGETDEPS += $$APP_ICON_ICO $$OUT_PWD/$$ICON_BUILD_DIR/.win_res_icon_sync $$APP_ICON_PNG
}

win32: LIBS += -luser32 -ldwmapi -lwtsapi32

OPENSSL_WIN64 = $$PWD/third_party/openssl/win64
win32:exists($$OPENSSL_WIN64/libssl-1_1-x64.dll) {
    OSSL_DLLS = $$OPENSSL_WIN64/libssl-1_1-x64.dll $$OPENSSL_WIN64/libcrypto-1_1-x64.dll
    OSSL_DEST = $$OUT_PWD/$$DESTDIR
    for(ossl, OSSL_DLLS) {
        QMAKE_POST_LINK += $$QMAKE_COPY \"$$shell_path($$ossl)\" \"$$shell_path($$OSSL_DEST)\" $$escape_expand(\\n\\t)
    }
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
