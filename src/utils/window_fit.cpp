#include "window_fit.h"

#include <QApplication>
#include <QDialog>
#include <QMainWindow>
#include <QScreen>
#include <QWidget>

namespace WindowFit {

QRect availableGeometry(const QWidget *widget)
{
    if (!widget)
        return QRect();

    if (QScreen *screen = widget->screen())
        return screen->availableGeometry();

    if (QScreen *primary = QApplication::primaryScreen())
        return primary->availableGeometry();

    return QRect();
}

void fitMainWindow(QMainWindow *window)
{
    if (!window)
        return;

    const QRect avail = availableGeometry(window);
    if (!avail.isValid())
        return;

    const int maxW = avail.width();
    const int maxH = avail.height();
    const int minW = qMax(640, qMin(960, maxW - 32));
    const int minH = qMax(480, qMin(640, maxH - 48));
    window->setMinimumSize(qMin(minW, maxW), qMin(minH, maxH));

    if (window->isMaximized())
        return;

    QSize target(qMin(1024, int(maxW * 0.92)), qMin(720, int(maxH * 0.88)));
    target.setWidth(qBound(window->minimumWidth(), target.width(), maxW));
    target.setHeight(qBound(window->minimumHeight(), target.height(), maxH));
    window->resize(target);

    const int x = avail.x() + (maxW - target.width()) / 2;
    const int y = avail.y() + (maxH - target.height()) / 2;
    window->move(x, y);
}

void fitDialog(QDialog *dialog, int preferredWidth)
{
    if (!dialog)
        return;

    const QRect avail = availableGeometry(dialog);
    if (!avail.isValid())
        return;

    const int maxW = qMin(preferredWidth, int(avail.width() * 0.92));
    const int maxH = int(avail.height() * 0.88);
    dialog->setMaximumSize(qMax(maxW, 360), qMax(maxH, 320));

    dialog->adjustSize();
    QSize target = dialog->size();
    target.setWidth(qMin(target.width(), maxW));
    target.setHeight(qMin(target.height(), maxH));
    dialog->resize(target);

    const int x = avail.x() + (avail.width() - target.width()) / 2;
    const int y = avail.y() + (avail.height() - target.height()) / 2;
    dialog->move(x, y);
}

} // namespace
