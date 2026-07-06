#ifndef APP_ICON_H
#define APP_ICON_H

#include <QIcon>

class QWidget;

QIcon loadAppIcon();
void applyWindowIcon(QWidget *widget, const QIcon &icon = QIcon());

#endif // APP_ICON_H
