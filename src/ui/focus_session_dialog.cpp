#include "focus_session_dialog.h"
#include "ui_focus_session_dialog.h"

#include "../utils/app_theme.h"
#include "../utils/window_fit.h"

#include <QFont>
#include <QPushButton>
#include <QShowEvent>

FocusSessionDialog::FocusSessionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FocusSessionDialog)
{
    ui->setupUi(this);
    AppTheme::styleDialog(this);
    setWindowTitle(tr("Focus 25"));
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::Tool);

    QFont countdownFont = ui->labelCountdown->font();
    countdownFont.setPixelSize(qMax(36, AppTheme::metrics().fontLarge * 2));
    countdownFont.setWeight(QFont::DemiBold);
    ui->labelCountdown->setFont(countdownFont);

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

    showRunningPanel(true);
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
        ui->labelTaskTitle->setText(tr("Focus 25"));
        ui->labelCountdown->setText(QStringLiteral("25:00"));
        ui->labelStatus->setText(QString());
        showRunningPanel(true);
        return;
    }

    const ActiveFocusSession session = m_service->current();
    ui->labelTaskTitle->setText(session.taskTitle);

    switch (m_service->state()) {
    case FocusSessionState::Running:
        ui->labelStatus->setText(tr("专注中…"));
        ui->btnPause->setText(tr("暂停"));
        showRunningPanel(true);
        break;
    case FocusSessionState::Paused:
        ui->labelStatus->setText(tr("已暂停"));
        ui->btnPause->setText(tr("继续"));
        showRunningPanel(true);
        break;
    case FocusSessionState::AwaitingResult:
        ui->labelStatus->setText(tr("时间到！"));
        showRunningPanel(false);
        break;
    case FocusSessionState::Idle:
        break;
    }

    updateCountdownLabel(session.remainingSec);
}

void FocusSessionDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    WindowFit::fitDialog(this, 380);
}

void FocusSessionDialog::showRunningPanel(bool running)
{
    ui->runningPanel->setVisible(running);
    ui->resultPanel->setVisible(!running);
}

void FocusSessionDialog::updateCountdownLabel(int remainingSec)
{
    const int clamped = qMax(0, remainingSec);
    const int minutes = clamped / 60;
    const int seconds = clamped % 60;
    ui->labelCountdown->setText(QStringLiteral("%1:%2")
                                    .arg(minutes)
                                    .arg(seconds, 2, 10, QLatin1Char('0')));
}
