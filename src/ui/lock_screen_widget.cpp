#include "lock_screen_widget.h"

#include "../core/app_settings.h"
#include "../utils/app_theme.h"
#include "../utils/app_version.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QResizeEvent>
#include <QShowEvent>
#include <QVBoxLayout>

LockScreenWidget::LockScreenWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("lockScreenOverlay"));
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);
    setFocusPolicy(Qt::StrongFocus);

    m_panel = new QWidget(this);
    m_panel->setObjectName(QStringLiteral("lockScreenPanel"));
    m_panel->setFixedWidth(360);
    AppTheme::applyElevation(m_panel, 28, 6, 90);

    m_titleLabel = new QLabel(m_panel);
    m_titleLabel->setObjectName(QStringLiteral("lockScreenTitle"));
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_hintLabel = new QLabel(m_panel);
    m_hintLabel->setObjectName(QStringLiteral("lockScreenHint"));
    m_hintLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel->setWordWrap(true);

    m_passwordEdit = new QLineEdit(m_panel);
    m_passwordEdit->setObjectName(QStringLiteral("lockScreenPassword"));
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText(tr("请输入密码"));
    m_passwordEdit->setClearButtonEnabled(true);

    m_errorLabel = new QLabel(m_panel);
    m_errorLabel->setObjectName(QStringLiteral("lockScreenError"));
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setWordWrap(true);
    m_errorLabel->hide();

    auto *unlockBtn = new QPushButton(tr("解锁"), m_panel);
    unlockBtn->setObjectName(QStringLiteral("btnOk"));
    unlockBtn->setDefault(true);

    auto *exitBtn = new QPushButton(tr("退出"), m_panel);
    exitBtn->setObjectName(QStringLiteral("btnCancel"));

    auto *btnRow = new QHBoxLayout;
    btnRow->setSpacing(12);
    btnRow->addWidget(unlockBtn);
    btnRow->addWidget(exitBtn);

    auto *layout = new QVBoxLayout(m_panel);
    layout->setSpacing(12);
    layout->setContentsMargins(28, 28, 28, 28);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_hintLabel);
    layout->addWidget(m_passwordEdit);
    layout->addWidget(m_errorLabel);
    layout->addLayout(btnRow);

    connect(unlockBtn, &QPushButton::clicked, this, &LockScreenWidget::attemptUnlock);
    connect(exitBtn, &QPushButton::clicked, this, &LockScreenWidget::exitRequested);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LockScreenWidget::attemptUnlock);

    setLoginMode(false);
    hide();
}

void LockScreenWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    QLinearGradient base(0, 0, width(), height());
    base.setColorAt(0.0, QColor(0x05, 0x0a, 0x18));
    base.setColorAt(1.0, QColor(0x08, 0x12, 0x22));
    p.fillRect(rect(), base);

    QFont watermarkFont = font();
    watermarkFont.setBold(true);
    const int watermarkSize = qBound(56, qMin(width(), height()) / 5, 120);
    watermarkFont.setPixelSize(watermarkSize);
    p.setFont(watermarkFont);
    p.setPen(QColor(0x14, 0x22, 0x38, 80));
    p.drawText(rect(), Qt::AlignCenter, QStringLiteral("ToDoList"));

    const QString versionText = tr("版本 %1").arg(AppVersion::buildDate());
    QFont versionFont = font();
    versionFont.setPixelSize(qMax(11, watermarkSize / 7));
    p.setFont(versionFont);
    p.setPen(QColor(0x6a, 0x7a, 0x96));
    QRect versionRect(0, height() - 48, width(), 32);
    p.drawText(versionRect, Qt::AlignHCenter | Qt::AlignVCenter, versionText);
}

void LockScreenWidget::setLoginMode(bool startupLogin)
{
    m_startupLogin = startupLogin;
    m_titleLabel->setText(startupLogin ? tr("ToDoList") : tr("ToDoList 已锁定"));
    refreshHint();
}

void LockScreenWidget::refreshHint()
{
    if (AppSettings::hasPassword()) {
        m_hintLabel->setText(m_startupLogin ? tr("请输入登录密码以继续使用")
                                            : tr("输入登录密码以继续使用"));
    } else {
        m_hintLabel->setText(tr("未设置密码，请联系管理员"));
    }
}

void LockScreenWidget::repositionPanel()
{
    if (!m_panel)
        return;
    m_panel->move((width() - m_panel->width()) / 2, (height() - m_panel->height()) / 2);
}

void LockScreenWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    repositionPanel();
}

void LockScreenWidget::attemptUnlock()
{
    m_errorLabel->hide();
    emit unlockRequested(m_passwordEdit->text());
}

void LockScreenWidget::showUnlockError(const QString &message)
{
    m_errorLabel->setText(message);
    m_errorLabel->show();
    m_passwordEdit->selectAll();
    m_passwordEdit->setFocus();
}

void LockScreenWidget::onUnlockAccepted()
{
    m_passwordEdit->clear();
    m_errorLabel->hide();
    hide();
    emit unlockSucceeded();
}

void LockScreenWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_passwordEdit->clear();
    m_errorLabel->hide();
    repositionPanel();
    m_passwordEdit->setFocus();
}
