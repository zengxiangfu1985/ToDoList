#include "cyber_quadrant_box.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionGroupBox>
#include <QStyle>

CyberQuadrantBox::CyberQuadrantBox(QWidget *parent)
    : QGroupBox(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
}

void CyberQuadrantBox::setQuadrant(EisenhowerQuadrant quadrant)
{
    m_quadrant = quadrant;
    update();
}

CyberQuadrantBox::AccentColors CyberQuadrantBox::accentColors() const
{
    AccentColors c;
    switch (m_quadrant) {
    case EisenhowerQuadrant::Q1_UrgentImportant:
        c.title = QColor(0x5b, 0x9b, 0xd5);
        c.leftBar = QColor(0x5b, 0x9b, 0xd5);
        break;
    case EisenhowerQuadrant::Q2_NotUrgentImportant:
        c.title = QColor(0x7a, 0x8f, 0xd4);
        c.leftBar = QColor(0x7a, 0x8f, 0xd4);
        break;
    case EisenhowerQuadrant::Q3_UrgentNotImportant:
        c.title = QColor(0xb8, 0x9b, 0x5e);
        c.leftBar = QColor(0xb8, 0x9b, 0x5e);
        break;
    case EisenhowerQuadrant::Q4_NotUrgentNotImportant:
    default:
        c.title = QColor(0x6d, 0x98, 0x73);
        c.leftBar = QColor(0x6d, 0x98, 0x73);
        break;
    }
    c.cornerTopRight = c.leftBar;
    c.cornerBottomLeft = c.leftBar;
    return c;
}

void CyberQuadrantBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOptionGroupBox option;
    initStyleOption(&option);
    QRect r = style()->subControlRect(QStyle::CC_GroupBox, &option,
                                      QStyle::SC_GroupBoxContents, this);
    r = r.adjusted(1, 0, -1, -1);
    if (r.height() < 8)
        r = rect().adjusted(4, 22, -4, -4);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const AccentColors accent = accentColors();

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0x0c, 0x14, 0x24));
    p.drawRoundedRect(r, 8, 8);

    p.setPen(QPen(QColor(0x1a, 0x2a, 0x44), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(r, 8, 8);

    p.fillRect(QRect(r.left(), r.top(), 3, r.height()), accent.leftBar);

    QGroupBox::paintEvent(event);
}
