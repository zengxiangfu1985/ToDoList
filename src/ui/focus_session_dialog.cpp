#include "focus_session_dialog.h"
#include "ui_focus_session_dialog.h"

#include "../utils/app_theme.h"
#include "../utils/window_fit.h"

#include <QApplication>
#include <QEvent>
#include <QFont>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPushButton>
#include <QScreen>
#include <QShowEvent>
#include <QTimer>

namespace {

constexpr int kDialogWidth = 300;
constexpr int kCornerMargin = 16;

} // namespace

FocusSessionDialog::FocusSessionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FocusSessionDialog)
{
    ui->setupUi(this);
    setObjectName(QStringLiteral("focusSessionDialog"));
    setWindowTitle(tr("Focus 25"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setFixedWidth(kDialogWidth);

    QFont compactFont = ui->labelCompact->font();
    compactFont.setPixelSize(qMax(14, AppTheme::metrics().fontBase));
    compactFont.setWeight(QFont::DemiBold);
    ui->labelCompact->setFont(compactFont);

    QFont statusFont = ui->labelStatus->font();
    statusFont.setPixelSize(qMax(12, AppTheme::metrics().fontBase - 1));
    ui->labelStatus->setFont(statusFont);
    ui->labelResultHint->setFont(statusFont);

    ui->labelExpandHint->setStyleSheet(QStringLiteral("color: #6a7a98;"));
    ui->labelStatus->setStyleSheet(QStringLiteral("color: #8a9bb8;"));
    ui->labelResultHint->setStyleSheet(QStringLiteral("color: #8a9bb8;"));

    m_collapseTimer = new QTimer(this);
    m_collapseTimer->setSingleShot(true);
    m_collapseTimer->setInterval(450);
    connect(m_collapseTimer, &QTimer::timeout, this, [this]() {
        if (!m_hovered && !shouldStayExpanded())
            setExpanded(false);
    });

    ui->collapsedBar->installEventFilter(this);
    ui->labelCompact->installEventFilter(this);

    connect(ui->btnPause, &QPushButton::clicked, this, [this]() {
        if (!m_service)
            return;
        if (m_service->state() == FocusSessionState::Paused)
            emit resumeRequested();
        else
            emit pauseRequested();
    });
    connect(ui->btnExtend, &QPushButton::clicked, this, &FocusSessionDialog::extendRequested);
    connect(ui->btnAbandon, &QPushButton::clicked, this, &FocusSessionDialog::abandonRequested);
    connect(ui->btnComplete, &QPushButton::clicked, this, &FocusSessionDialog::completeRequested);
    connect(ui->btnAnotherRound, &QPushButton::clicked, this, &FocusSessionDialog::anotherRoundRequested);
    connect(ui->btnSkip, &QPushButton::clicked, this, &FocusSessionDialog::skipRequested);

    ui->expandedPanel->setVisible(false);
    showRunningPanel(true);
    setExpanded(false);
}

FocusSessionDialog::~FocusSessionDialog()
{
    delete ui;
}

void FocusSessionDialog::bindSession(FocusSessionService *service)
{
    m_service = service;
    refreshUi();
}

void FocusSessionDialog::refreshUi()
{
    if (!m_service || !m_service->isActive()) {
        m_taskTitle = tr("Focus 25");
        m_forceExpanded = false;
        m_pinned = false;
        updateCountdownLabel(25 * 60);
        updateCompactLabel(m_taskTitle);
        ui->labelStatus->setText(QString());
        showRunningPanel(true);
        setExpanded(false);
        return;
    }

    const ActiveFocusSession session = m_service->current();
    m_taskTitle = session.taskTitle;
    updateCountdownLabel(session.remainingSec);
    updateCompactLabel(m_taskTitle);

    switch (m_service->state()) {
    case FocusSessionState::Running:
        m_forceExpanded = false;
        ui->labelStatus->setText(tr("专注中…"));
        ui->btnPause->setText(tr("暂停"));
        ui->labelExpandHint->setText(QStringLiteral("▾"));
        showRunningPanel(true);
        break;
    case FocusSessionState::Paused:
        m_forceExpanded = false;
        ui->labelStatus->setText(tr("已暂停"));
        ui->btnPause->setText(tr("继续"));
        ui->labelExpandHint->setText(QStringLiteral("▾"));
        showRunningPanel(true);
        break;
    case FocusSessionState::AwaitingResult:
        m_forceExpanded = true;
        m_pinned = false;
        ui->labelStatus->setText(tr("时间到！"));
        ui->labelExpandHint->setText(QStringLiteral("▴"));
        showRunningPanel(false);
        setExpanded(true);
        break;
    case FocusSessionState::Idle:
        break;
    }

    if (!m_forceExpanded)
        setExpanded(m_hovered || m_pinned);
}

void FocusSessionDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    if (!m_forceExpanded && !m_hovered && !m_pinned)
        setExpanded(false);
    positionBottomRight();
}

void FocusSessionDialog::enterEvent(QEvent *event)
{
    QDialog::enterEvent(event);
    m_hovered = true;
    cancelCollapse();
    setExpanded(true);
}

void FocusSessionDialog::leaveEvent(QEvent *event)
{
    QDialog::leaveEvent(event);
    m_hovered = false;
    if (!shouldStayExpanded())
        scheduleCollapse();
}

bool FocusSessionDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->collapsedBar || watched == ui->labelCompact) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto *mouse = static_cast<QMouseEvent *>(event);
            if (mouse->button() == Qt::LeftButton) {
                if (m_forceExpanded)
                    return false;
                m_pinned = !m_pinned;
                if (m_pinned) {
                    cancelCollapse();
                    setExpanded(true);
                } else if (!m_hovered) {
                    setExpanded(false);
                }
                return true;
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}

void FocusSessionDialog::showRunningPanel(bool running)
{
    ui->stackPanels->setCurrentIndex(running ? 0 : 1);
}

void FocusSessionDialog::updateCountdownLabel(int remainingSec)
{
    const int clamped = qMax(0, remainingSec);
    const int minutes = clamped / 60;
    const int seconds = clamped % 60;
    const QString timeText = QStringLiteral("%1:%2")
                                 .arg(minutes)
                                 .arg(seconds, 2, 10, QLatin1Char('0'));
    ui->labelCompact->setProperty("focusTimeText", timeText);
    updateCompactLabel(m_taskTitle);
}

void FocusSessionDialog::updateCompactLabel(const QString &taskTitle)
{
    const QString timeText = ui->labelCompact->property("focusTimeText").toString();
    const QString timePart = timeText.isEmpty() ? QStringLiteral("25:00") : timeText;
    const QString separator = QStringLiteral(" · ");
    const QString full = timePart + separator + taskTitle;

    const int textWidth = kDialogWidth - 44;
    const QFontMetrics fm(ui->labelCompact->font());
    ui->labelCompact->setText(fm.elidedText(full, Qt::ElideRight, qMax(120, textWidth)));
    ui->labelCompact->setToolTip(full + QStringLiteral("\n") + tr("悬停展开；点击固定或收起"));
}

void FocusSessionDialog::setExpanded(bool expanded)
{
    if (m_forceExpanded)
        expanded = true;

    if (m_expanded == expanded)
        return;

    m_expanded = expanded;
    ui->expandedPanel->setVisible(expanded);
    ui->labelExpandHint->setText(expanded ? QStringLiteral("▴") : QStringLiteral("▾"));

    adjustSize();
    setFixedWidth(kDialogWidth);
    positionBottomRight();
}

void FocusSessionDialog::positionBottomRight()
{
    const QRect avail = WindowFit::availableGeometry(this);
    if (!avail.isValid())
        return;

    const QSize sz = size();
    const int x = avail.right() - sz.width() - kCornerMargin;
    const int y = avail.bottom() - sz.height() - kCornerMargin;
    move(x, y);
}

void FocusSessionDialog::scheduleCollapse()
{
    if (shouldStayExpanded())
        return;
    m_collapseTimer->start();
}

void FocusSessionDialog::cancelCollapse()
{
    m_collapseTimer->stop();
}

bool FocusSessionDialog::shouldStayExpanded() const
{
    return m_forceExpanded || m_pinned || m_hovered;
}
