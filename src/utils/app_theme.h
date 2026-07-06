#ifndef APP_THEME_H
#define APP_THEME_H

#include <QtGlobal>

class QApplication;
class QScreen;
class QString;
class QWidget;

struct ThemeMetrics
{
    int fontBase = 15;
    int fontSmall = 13;
    int fontLarge = 16;
    int fontTitle = 15;
    int rowHeight = 42;
    int buttonHeight = 36;
    int buttonPadV = 8;
    int buttonPadH = 16;
    int titleBarHeight = 40;
    int titleBarBtnWidth = 48;
    int titleBarBtnHeight = 36;
    int titleBarFont = 12;
    int menuBarHeight = 32;
    int toolBarMaxHeight = 56;
    int tablePadV = 10;
    int tablePadH = 12;
    int headerPad = 10;
    int listItemPadV = 10;
    int listItemPadH = 12;
    int scrollBarSize = 12;
    qreal scale = 1.0;
};

class AppTheme
{
public:
    static void apply(QApplication *app);
    static void applyDarkTitleBar(QWidget *window);
    static void applyElevation(QWidget *widget, int blurRadius = 18, int offsetY = 2, int alpha = 55);
    static void styleDialog(QWidget *dialog);
    static const ThemeMetrics &metrics();

private:
    static ThemeMetrics computeMetrics(QScreen *screen);
    static QString loadStylesheet(const ThemeMetrics &m);

    static ThemeMetrics s_metrics;
};

#endif // APP_THEME_H
