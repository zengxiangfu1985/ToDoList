#include "ai_busy_overlay.h"

#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>

SpinnerWidget::SpinnerWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(32, 32);
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        m_angle = (m_angle + 30) % 360;
        update();
    });
}

void SpinnerWidget::start()
{
    m_timer->start(80);
}

void SpinnerWidget::stop()
{
    m_timer->stop();
    m_angle = 0;
    update();
}

void SpinnerWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int penWidth = 3;
    const QRectF rect(penWidth, penWidth, width() - penWidth * 2, height() - penWidth * 2);

    p.setPen(QPen(QColor(0x1a, 0x2a, 0x44), penWidth, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(rect, 0, 360 * 16);

    p.setPen(QPen(QColor(0x4a, 0x7a, 0xb8), penWidth, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(rect, -m_angle * 16, 90 * 16);
}

AiBusyOverlay::AiBusyOverlay(QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("aiBusyOverlay"));
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);
    hide();

    m_opacity = new QGraphicsOpacityEffect(this);
    m_opacity->setOpacity(0.0);
    setGraphicsEffect(m_opacity);

    m_fadeIn = new QPropertyAnimation(m_opacity, "opacity", this);
    m_fadeIn->setDuration(220);
    m_fadeIn->setStartValue(0.0);
    m_fadeIn->setEndValue(1.0);

    m_fadeOut = new QPropertyAnimation(m_opacity, "opacity", this);
    m_fadeOut->setDuration(180);
    m_fadeOut->setStartValue(1.0);
    m_fadeOut->setEndValue(0.0);
    connect(m_fadeOut, &QPropertyAnimation::finished, this, [this]() { onFadeOutFinished(); });

    m_panel = new QWidget(this);
    m_panel->setObjectName(QStringLiteral("aiBusyPanel"));
    m_panel->setFixedSize(280, 120);

    m_spinner = new SpinnerWidget(m_panel);

    m_label = new QLabel(tr("分析中请稍后"), m_panel);
    m_label->setObjectName(QStringLiteral("aiBusyLabel"));
    m_label->setAlignment(Qt::AlignCenter);

    auto *row = new QHBoxLayout;
    row->setSpacing(14);
    row->addStretch();
    row->addWidget(m_spinner);
    row->addWidget(m_label);
    row->addStretch();

    auto *layout = new QVBoxLayout(m_panel);
    layout->setContentsMargins(20, 24, 20, 24);
    layout->addLayout(row);
}

void AiBusyOverlay::repositionPanel()
{
    if (!m_panel)
        return;
    m_panel->move((width() - m_panel->width()) / 2, (height() - m_panel->height()) / 2);
}

void AiBusyOverlay::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    repositionPanel();
}

void AiBusyOverlay::showAnimated()
{
    m_hiding = false;
    repositionPanel();
    raise();
    show();
    m_spinner->start();
    m_fadeOut->stop();
    m_fadeIn->start();
}

void AiBusyOverlay::hideAnimated()
{
    if (!isVisible() || m_hiding)
        return;
    m_hiding = true;
    m_fadeIn->stop();
    m_fadeOut->start();
}

void AiBusyOverlay::onFadeOutFinished()
{
    m_spinner->stop();
    hide();
    m_hiding = false;
    m_opacity->setOpacity(0.0);
}

void AiBusyOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.fillRect(rect(), QColor(5, 10, 24, 160));
}
