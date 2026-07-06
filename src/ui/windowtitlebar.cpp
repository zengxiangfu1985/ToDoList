#include "windowtitlebar.h"

#include "../utils/app_icon.h"
#include "../utils/app_theme.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QtGlobal>

WindowTitleBar::WindowTitleBar(QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("windowTitleBar"));
    setFixedHeight(AppTheme::metrics().titleBarHeight);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setObjectName(QStringLiteral("titleBarIcon"));
    m_iconLabel->setFixedSize(AppTheme::metrics().titleBarHeight - 8,
                              AppTheme::metrics().titleBarHeight - 8);
    m_iconLabel->setScaledContents(true);
    m_iconLabel->setPixmap(loadAppIcon().pixmap(m_iconLabel->size(), QIcon::Normal, QIcon::Off));

    m_titleLabel = new QLabel(tr("ToDoList"), this);
    m_titleLabel->setObjectName(QStringLiteral("titleBarLabel"));

    m_minBtn = new QPushButton(QStringLiteral("—"), this);
    m_minBtn->setObjectName(QStringLiteral("titleBarMinBtn"));
    m_minBtn->setToolTip(tr("最小化"));

    m_maxBtn = new QPushButton(QStringLiteral("□"), this);
    m_maxBtn->setObjectName(QStringLiteral("titleBarMaxBtn"));
    m_maxBtn->setToolTip(tr("最大化 / 还原 (F11)"));

    m_closeBtn = new QPushButton(QStringLiteral("×"), this);
    m_closeBtn->setObjectName(QStringLiteral("titleBarCloseBtn"));
    m_closeBtn->setToolTip(tr("关闭"));

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 0, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignVCenter);
    layout->addWidget(m_iconLabel, 0, Qt::AlignVCenter);
    layout->addSpacing(8);
    layout->addWidget(m_titleLabel, 0, Qt::AlignVCenter);
    layout->addStretch();
    layout->addWidget(m_minBtn, 0, Qt::AlignVCenter);
    layout->addWidget(m_maxBtn, 0, Qt::AlignVCenter);
    layout->addWidget(m_closeBtn, 0, Qt::AlignVCenter);

    connect(m_minBtn, &QPushButton::clicked, this, &WindowTitleBar::minimizeRequested);
    connect(m_maxBtn, &QPushButton::clicked, this, &WindowTitleBar::maximizeRequested);
    connect(m_closeBtn, &QPushButton::clicked, this, &WindowTitleBar::closeRequested);
}

void WindowTitleBar::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void WindowTitleBar::setTitleBarIcon(const QIcon &icon)
{
    if (!m_iconLabel)
        return;
    const QSize size = m_iconLabel->size();
    const QPixmap pm = icon.pixmap(size, QIcon::Normal, QIcon::Off);
    if (!pm.isNull())
        m_iconLabel->setPixmap(pm);
}

void WindowTitleBar::updateMaximizeButton(bool maximized)
{
    m_maxBtn->setText(maximized ? QStringLiteral("❐") : QStringLiteral("□"));
    m_maxBtn->setToolTip(maximized ? tr("还原 (F11)") : tr("最大化 (F11)"));
}

void WindowTitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    QWidget *top = window();
    if (!top)
        return;

    m_dragging = true;
    if (top->isMaximized()) {
        const qreal ratio = event->pos().x() / qMax(1, width());
        emit maximizeRequested();
        m_dragOffset = QPoint(int(top->width() * ratio), event->pos().y());
    } else {
        m_dragOffset = event->globalPos() - top->frameGeometry().topLeft();
    }
    event->accept();
}

void WindowTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_dragging || !(event->buttons() & Qt::LeftButton))
        return;

    QWidget *top = window();
    if (top)
        top->move(event->globalPos() - m_dragOffset);
    event->accept();
}

void WindowTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_dragging = false;
}

void WindowTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit maximizeRequested();
}
