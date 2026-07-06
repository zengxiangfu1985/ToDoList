#include "app_translations.h"

#include "../core/app_settings.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QTranslator>
#include <QWidget>

namespace {

QTranslator *s_appTranslator = nullptr;

QString translatorFileName(const QString &languageCode)
{
    if (languageCode == QStringLiteral("en"))
        return QStringLiteral("todolist_en.qm");
    return {};
}

QString findQmPath(const QString &fileName)
{
    const QStringList candidates = {
        QCoreApplication::applicationDirPath() + QStringLiteral("/translations/") + fileName,
        QCoreApplication::applicationDirPath() + QStringLiteral("/") + fileName,
        QStringLiteral(":/translations/") + fileName,
    };
    for (const QString &path : candidates) {
        if (QFile::exists(path))
            return path;
    }
    return {};
}

void removeInstalledTranslator(QApplication *app)
{
    if (s_appTranslator) {
        app->removeTranslator(s_appTranslator);
        delete s_appTranslator;
        s_appTranslator = nullptr;
    }
}

} // namespace

namespace AppTranslations {

QString currentLanguageCode()
{
    return AppSettings::uiLanguage();
}

bool installForApplication(QApplication *app)
{
    if (!app)
        return false;

    removeInstalledTranslator(app);

    const QString languageCode = AppSettings::uiLanguage();
    if (languageCode == QStringLiteral("zh"))
        return true;

    const QString fileName = translatorFileName(languageCode);
    if (fileName.isEmpty())
        return false;

    const QString qmPath = findQmPath(fileName);
    if (qmPath.isEmpty())
        return false;

    auto *translator = new QTranslator(app);
    if (!translator->load(qmPath)) {
        delete translator;
        return false;
    }

    app->installTranslator(translator);
    s_appTranslator = translator;
    return true;
}

void notifyLanguageChanged()
{
    if (!QCoreApplication::instance())
        return;

    const QWidgetList widgets = QApplication::topLevelWidgets();
    for (QWidget *widget : widgets) {
        if (widget)
            QCoreApplication::sendEvent(widget, new QEvent(QEvent::LanguageChange));
    }
}

} // namespace AppTranslations
