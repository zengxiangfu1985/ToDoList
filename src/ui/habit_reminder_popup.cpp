#include "habit_reminder_popup.h"

#include "../utils/app_theme.h"

#include <QGuiApplication>
#include <QLabel>
#include <QPushButton>
#include <QScreen>
#include <QShowEvent>
#include <QVBoxLayout>
#include <QWindow>

namespace {

constexpr int kPopupWidth = 320;
constexpr int kCornerMargin = 16;

} // namespace

HabitReminderPopup::HabitReminderPopup(QWidget *parent)
    : QDialog(parent)
{
    setObjectName(QStringLiteral("habitReminderPopup"));
    setWindowTitle(tr("健康提醒"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setFixedWidth(kPopupWidth);
    AppTheme::styleDialog(this);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(14, 12, 14, 12);
    root->setSpacing(8);

    m_titleLabel = new QLabel(this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPixelSize(qMax(15, AppTheme::metrics().fontBase + 1));
    titleFont.setWeight(QFont::DemiBold);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setWordWrap(true);
    root->addWidget(m_titleLabel);

    m_messageLabel = new QLabel(this);
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setStyleSheet(QStringLiteral("color: #8a9bb8;"));
    root->addWidget(m_messageLabel);

    auto *btnDone = new QPushButton(tr("已完成"), this);
    btnDone->setObjectName(QStringLiteral("btnOk"));
    auto *btnSnooze = new QPushButton(tr("稍后 5 分钟"), this);
    btnSnooze->setObjectName(QStringLiteral("btnSecondary"));
    auto *btnSkipToday = new QPushButton(tr("今日不再提醒"), this);
    btnSkipToday->setObjectName(QStringLiteral("btnSecondary"));

    root->addWidget(btnDone);
    root->addWidget(btnSnooze);
    root->addWidget(btnSkipToday);

    connect(btnDone, &QPushButton::clicked, this, [this]() {
        if (m_habitId > 0)
            emit acknowledged(m_habitId);
        hide();
    });
    connect(btnSnooze, &QPushButton::clicked, this, [this]() {
        if (m_habitId > 0)
            emit snoozed(m_habitId);
        hide();
    });
    connect(btnSkipToday, &QPushButton::clicked, this, [this]() {
        if (m_habitId > 0)
            emit disabledForToday(m_habitId);
        hide();
    });
}

void HabitReminderPopup::showReminder(const HabitReminder &habit)
{
    m_habitId = habit.id;
    m_titleLabel->setText(habit.title);
    m_messageLabel->setText(habit.message);
    show();
    raise();
    activateWindow();
}

void HabitReminderPopup::dismissIfHabit(qint64 habitId)
{
    if (habitId <= 0 || m_habitId != habitId)
        return;
    m_habitId = 0;
    hide();
}

void HabitReminderPopup::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    positionBottomRight();
}

void HabitReminderPopup::positionBottomRight()
{
    QScreen *screen = windowHandle() ? windowHandle()->screen() : QGuiApplication::primaryScreen();
    if (!screen)
        return;

    const QRect avail = screen->availableGeometry();
    const int x = avail.right() - width() - kCornerMargin;
    const int y = avail.bottom() - height() - kCornerMargin - 80;
    move(x, qMax(avail.top() + kCornerMargin, y));
}
