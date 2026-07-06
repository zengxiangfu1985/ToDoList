#include "cyber_ai_panel.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionGroupBox>
#include <QStyle>

CyberAiPanel::CyberAiPanel(QWidget *parent)
    : QGroupBox(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
}

void CyberAiPanel::paintEvent(QPaintEvent *event)
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

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0x0c, 0x14, 0x24));
    p.drawRoundedRect(r, 8, 8);

    p.setPen(QPen(QColor(0x1a, 0x2a, 0x44), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(r, 8, 8);

    p.fillRect(QRect(r.left(), r.top(), 3, r.height()), QColor(0x4a, 0x7a, 0xb8));

    QGroupBox::paintEvent(event);
}
