#ifndef APP_TRANSLATIONS_H
#define APP_TRANSLATIONS_H

#include <QString>

class QApplication;

namespace AppTranslations {

QString currentLanguageCode();
bool installForApplication(QApplication *app);
void notifyLanguageChanged();

} // namespace AppTranslations

#endif // APP_TRANSLATIONS_H
