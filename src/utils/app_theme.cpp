#include "app_theme.h"

#include <QApplication>
#include <QColor>
#include <QDialog>
#include <QFile>
#include <QFont>
#include <QGuiApplication>
#include <QString>
#include <QPalette>
#include <QScreen>
#include <QStyleFactory>
#include <QWidget>

#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include <QObject>

namespace {

class DialogThemeHelper : public QObject
{
public:
    explicit DialogThemeHelper(QWidget *dialog)
        : QObject(dialog)
        , m_dialog(dialog)
    {
        m_dialog->installEventFilter(this);
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_dialog && event->type() == QEvent::Show)
            AppTheme::applyDarkTitleBar(m_dialog);
        return QObject::eventFilter(watched, event);
    }

private:
    QWidget *m_dialog;
};

} // namespace

#include <QtGlobal>

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#endif

ThemeMetrics AppTheme::s_metrics;

ThemeMetrics AppTheme::computeMetrics(QScreen *screen)
{
    const qreal dpi = screen ? screen->logicalDotsPerInch() : 96.0;
    qreal scale = dpi / 96.0;

    if (screen) {
        const int h = screen->availableGeometry().height();
        if (h >= 1080)
            scale = qMax(scale, 1.08);
        if (h >= 1440)
            scale = qMax(scale, 1.18);
        if (h >= 2160)
            scale = qMax(scale, 1.32);
    }

    scale = qBound(1.0, scale, 2.0);

    ThemeMetrics m;
    m.scale = scale;
    m.fontBase = qRound(15 * scale);
    m.fontSmall = qRound(13 * scale);
    m.fontLarge = qRound(17 * scale);
    m.fontTitle = qRound(13 * scale);
    m.rowHeight = qRound(42 * scale);
    m.buttonHeight = qRound(36 * scale);
    m.buttonPadV = qRound(8 * scale);
    m.buttonPadH = qRound(16 * scale);
    m.titleBarHeight = qRound(28 * scale);
    m.titleBarBtnWidth = qRound(36 * scale);
    m.titleBarBtnHeight = m.titleBarHeight;
    m.titleBarFont = qMax(11, qRound(12 * scale));
    m.menuBarHeight = qRound(32 * scale);
    m.toolBarMaxHeight = qRound(56 * scale);
    m.tablePadV = qRound(10 * scale);
    m.tablePadH = qRound(12 * scale);
    m.headerPad = qRound(10 * scale);
    m.listItemPadV = qRound(10 * scale);
    m.listItemPadH = qRound(12 * scale);
    m.scrollBarSize = qMax(10, qRound(12 * scale));
    return m;
}

QString AppTheme::loadStylesheet(const ThemeMetrics &m)
{
    QFile qss(QStringLiteral(":/styles/cursor_dark.qss"));
    if (!qss.open(QIODevice::ReadOnly))
        return {};

    QString sheet = QString::fromUtf8(qss.readAll());

    const auto inject = [&](const QString &key, int value) {
        sheet.replace(QStringLiteral("{{") + key + QStringLiteral("}}"), QString::number(value));
    };

    inject(QStringLiteral("FONT_BASE"), m.fontBase);
    inject(QStringLiteral("FONT_SMALL"), m.fontSmall);
    inject(QStringLiteral("FONT_LARGE"), m.fontLarge);
    inject(QStringLiteral("FONT_TITLE"), m.fontTitle);
    inject(QStringLiteral("BTN_HEIGHT"), m.buttonHeight);
    inject(QStringLiteral("BTN_PAD_V"), m.buttonPadV);
    inject(QStringLiteral("BTN_PAD_H"), m.buttonPadH);
    inject(QStringLiteral("TITLE_BAR_H"), m.titleBarHeight);
    inject(QStringLiteral("TITLE_BTN_W"), m.titleBarBtnWidth);
    inject(QStringLiteral("TITLE_BTN_H"), m.titleBarBtnHeight);
    inject(QStringLiteral("TITLE_BAR_FONT"), m.titleBarFont);
    inject(QStringLiteral("TOOLBAR_MAX_H"), m.toolBarMaxHeight);
    inject(QStringLiteral("TABLE_PAD_V"), m.tablePadV);
    inject(QStringLiteral("TABLE_PAD_H"), m.tablePadH);
    inject(QStringLiteral("HEADER_PAD"), m.headerPad);
    inject(QStringLiteral("LIST_PAD_V"), m.listItemPadV);
    inject(QStringLiteral("LIST_PAD_H"), m.listItemPadH);
    inject(QStringLiteral("SCROLL_SIZE"), m.scrollBarSize);

    return sheet;
}

void AppTheme::apply(QApplication *app)
{
    s_metrics = computeMetrics(QGuiApplication::primaryScreen());

    app->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QFont appFont;
    appFont.setFamilies({QStringLiteral("Segoe UI"), QStringLiteral("Microsoft YaHei UI")});
    appFont.setPixelSize(s_metrics.fontBase);
    app->setFont(appFont);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0x05, 0x0a, 0x18));
    palette.setColor(QPalette::WindowText, QColor(0xd0, 0xd8, 0xe8));
    palette.setColor(QPalette::Base, QColor(0x0a, 0x12, 0x20));
    palette.setColor(QPalette::AlternateBase, QColor(0x0d, 0x15, 0x28));
    palette.setColor(QPalette::Text, QColor(0xd0, 0xd8, 0xe8));
    palette.setColor(QPalette::Button, QColor(0x0f, 0x18, 0x2e));
    palette.setColor(QPalette::ButtonText, QColor(0xd0, 0xd8, 0xe8));
    palette.setColor(QPalette::Highlight, QColor(0x00, 0x70, 0xc0, 140));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::ToolTipBase, QColor(0x0a, 0x12, 0x20));
    palette.setColor(QPalette::ToolTipText, QColor(0xd0, 0xd8, 0xe8));
    app->setPalette(palette);

    app->setStyleSheet(loadStylesheet(s_metrics));
}

const ThemeMetrics &AppTheme::metrics()
{
    return s_metrics;
}

void AppTheme::applyDarkTitleBar(QWidget *window)
{
#ifdef Q_OS_WIN
    if (!window)
        return;
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());
    if (!hwnd)
        return;

    const BOOL useDark = TRUE;
    DwmSetWindowAttribute(hwnd, 20, &useDark, sizeof(useDark));
#else
    Q_UNUSED(window);
#endif
}

void AppTheme::applyElevation(QWidget *widget, int blurRadius, int offsetY, int alpha)
{
    if (!widget)
        return;
    auto *effect = new QGraphicsDropShadowEffect(widget);
    effect->setBlurRadius(blurRadius);
    effect->setOffset(0, offsetY);
    effect->setColor(QColor(0, 0, 0, alpha));
    widget->setGraphicsEffect(effect);
}

void AppTheme::styleDialog(QWidget *dialog)
{
    if (!dialog)
        return;
    dialog->setObjectName(QStringLiteral("appDialog"));
    new DialogThemeHelper(dialog);
}
