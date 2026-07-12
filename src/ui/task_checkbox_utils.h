#ifndef TASK_CHECKBOX_UTILS_H
#define TASK_CHECKBOX_UTILS_H

#include <QPainter>
#include <QPainterPath>
#include <QRect>

namespace TaskCheckbox {

constexpr int kSize = 22;
constexpr int kRightPad = 8;

inline QRect rectForItem(const QRect &itemRect)
{
    return QRect(itemRect.right() - kRightPad - kSize,
                 itemRect.center().y() - kSize / 2,
                 kSize, kSize);
}

inline void paint(QPainter *p, const QRect &rect, bool checked, bool hover)
{
    p->save();
    p->setRenderHint(QPainter::Antialiasing);

    QColor bg(0x0c, 0x14, 0x24);
    QColor borderTop(0x18, 0x20, 0x30);
    QColor borderBottom(0x06, 0x0a, 0x12);
    if (checked) {
        bg = hover ? QColor(0x20, 0x58, 0x88) : QColor(0x1a, 0x50, 0x80);
        borderTop = hover ? QColor(0x4a, 0x80, 0xa8) : QColor(0x3a, 0x70, 0x98);
        borderBottom = QColor(0x12, 0x30, 0x50);
    } else if (hover) {
        bg = QColor(0x10, 0x18, 0x28);
        borderTop = QColor(0x28, 0x40, 0x60);
    }

    p->setPen(Qt::NoPen);
    p->setBrush(bg);
    p->drawRoundedRect(rect, 6, 6);

    p->setPen(QPen(borderTop, 1));
    p->drawLine(rect.topLeft() + QPoint(1, 0), rect.topRight() + QPoint(-1, 0));
    p->setPen(QPen(borderBottom, 1));
    p->drawLine(rect.bottomLeft() + QPoint(1, 0), rect.bottomRight() + QPoint(-1, 0));
    p->setPen(QPen(QColor(0x0a, 0x10, 0x1c), 1));
    p->setBrush(Qt::NoBrush);
    p->drawRoundedRect(rect.adjusted(0, 0, -1, -1), 6, 6);

    if (checked) {
        QPen tickPen(QColor(0x4a, 0xd4, 0x8a), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p->setPen(tickPen);
        const QPoint c = rect.center();
        QPainterPath tick;
        tick.moveTo(c.x() - 5, c.y());
        tick.lineTo(c.x() - 1, c.y() + 4);
        tick.lineTo(c.x() + 6, c.y() - 5);
        p->drawPath(tick);
    }

    p->restore();
}

} // namespace TaskCheckbox

#endif // TASK_CHECKBOX_UTILS_H
