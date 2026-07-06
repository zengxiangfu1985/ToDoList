#include "src/ui/mainwindow.h"
#include "src/utils/app_theme.h"
#include "src/utils/ssl_support.h"
#include "src/utils/app_logger.h"
#include "src/utils/app_icon.h"
#include "src/utils/single_instance.h"
#include "src/utils/app_translations.h"
#include "src/utils/portable_settings.h"

#include <QApplication>
#include <QCoreApplication>
#include <QSystemTrayIcon>

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
#include <QGuiApplication>
#endif

int main(int argc, char *argv[])
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QCoreApplication::setOrganizationName(QStringLiteral("ToDoList"));
    QCoreApplication::setApplicationName(QStringLiteral("ToDoList"));

    QApplication app(argc, argv);
    AppLogger::initialize();
    PortableSettings::initialize();
    SslSupport::initialize();
    AppTheme::apply(&app);
    AppTranslations::installForApplication(&app);
    QApplication::setQuitOnLastWindowClosed(false);

    SingleInstanceGuard instanceGuard;
    MainWindow w;
    QObject::connect(&instanceGuard, &SingleInstanceGuard::raiseRequested, &w,
                     &MainWindow::requestShowFromAnotherInstance);
    if (!instanceGuard.isPrimaryInstance())
        return 0;

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        // 无托盘时仍可使用主窗口
    }

    w.show();
    const int code = app.exec();
    AppLogger::shutdown();
    return code;
}
