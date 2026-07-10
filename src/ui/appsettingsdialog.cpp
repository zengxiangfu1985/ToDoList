#include "appsettingsdialog.h"
#include "ui_appsettingsdialog.h"

#include "../core/app_settings.h"
#include "../utils/global_hotkey.h"
#include "../utils/app_theme.h"
#include "../utils/app_translations.h"
#include "../utils/window_fit.h"

#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>
#include <QShowEvent>
#include <QEvent>

AppSettingsDialog::AppSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AppSettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("设置"));
    setMinimumWidth(360);
    ui->scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->verticalLayout->setStretch(0, 1);
    AppTheme::styleDialog(this);

    ui->btnClearPassword->setEnabled(AppSettings::hasPassword());
    ui->checkLockOnStartup->setEnabled(AppSettings::hasPassword());
    ui->btnViewAiTrace->setEnabled(false);

    ui->comboLanguage->clear();
    ui->comboLanguage->addItem(tr("中文"), QStringLiteral("zh"));
    ui->comboLanguage->addItem(QStringLiteral("English"), QStringLiteral("en"));

    ui->comboFocusDuration->clear();
    ui->comboFocusDuration->addItem(tr("15 分钟"), 15);
    ui->comboFocusDuration->addItem(tr("25 分钟"), 25);
    ui->comboFocusDuration->addItem(tr("50 分钟"), 50);

    connect(ui->btnSave, &QPushButton::clicked, this, &AppSettingsDialog::onSave);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->btnClearPassword, &QPushButton::clicked, this, &AppSettingsDialog::onClearPassword);
    connect(ui->btnLockNow, &QPushButton::clicked, this, &AppSettingsDialog::onLockNow);
    connect(ui->btnViewAiTrace, &QPushButton::clicked, this, &AppSettingsDialog::onViewAiTrace);
    connect(ui->btnM365, &QPushButton::clicked, this, &AppSettingsDialog::onM365Settings);

    loadToUi();
}

void AppSettingsDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    WindowFit::fitDialog(this, 520);
}

void AppSettingsDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        const QString lang = ui->comboLanguage->currentData().toString();
        ui->comboLanguage->clear();
        ui->comboLanguage->addItem(tr("中文"), QStringLiteral("zh"));
        ui->comboLanguage->addItem(QStringLiteral("English"), QStringLiteral("en"));
        const int langIdx = ui->comboLanguage->findData(lang);
        ui->comboLanguage->setCurrentIndex(langIdx >= 0 ? langIdx : 0);
    }
    QDialog::changeEvent(event);
}

void AppSettingsDialog::setViewAiTraceEnabled(bool enabled)
{
    ui->btnViewAiTrace->setEnabled(enabled);
}

void AppSettingsDialog::onViewAiTrace()
{
    emit viewAiTraceRequested();
}

void AppSettingsDialog::onM365Settings()
{
    emit m365SettingsRequested();
}

AppSettingsDialog::~AppSettingsDialog()
{
    delete ui;
}

void AppSettingsDialog::loadToUi()
{
    ui->spinLockAfterSystemLock->setValue(AppSettings::lockAfterSystemLockMinutes());
    ui->spinLockOnIdle->setValue(AppSettings::lockOnIdleMinutes());
    ui->checkLockOnStartup->setChecked(AppSettings::lockOnStartup());
    ui->checkMinimizeToTray->setChecked(AppSettings::minimizeToTray());
    ui->checkUsageStatistics->setChecked(AppSettings::usageStatisticsEnabled());
    ui->editTodayTasksHotkey->setKeySequence(AppSettings::todayTasksHotkey());
    ui->editTop3Hotkey->setKeySequence(AppSettings::top3PopupHotkey());
    ui->editQuickCaptureHotkey->setKeySequence(AppSettings::quickCaptureHotkey());
    ui->checkQuickCaptureAutoAnalyze->setChecked(AppSettings::quickCaptureAutoAnalyze());
    ui->comboFocusDuration->setCurrentIndex(
        ui->comboFocusDuration->findData(AppSettings::focusDurationMinutes()));
    ui->checkFocusTrayCountdown->setChecked(AppSettings::focusTrayCountdown());
    ui->editCurrentPassword->clear();
    ui->editNewPassword->clear();
    ui->editConfirmPassword->clear();

    const QString lang = AppSettings::uiLanguage();
    const int langIdx = ui->comboLanguage->findData(lang);
    ui->comboLanguage->setCurrentIndex(langIdx >= 0 ? langIdx : 0);
}

bool AppSettingsDialog::applyPasswordChange(QString *error)
{
    const QString current = ui->editCurrentPassword->text();
    const QString newPassword = ui->editNewPassword->text();
    const QString confirm = ui->editConfirmPassword->text();

    if (newPassword.isEmpty() && confirm.isEmpty())
        return true;

    if (AppSettings::hasPassword() && !AppSettings::verifyPassword(current)) {
        if (error)
            *error = tr("当前密码不正确");
        return false;
    }

    if (newPassword != confirm) {
        if (error)
            *error = tr("两次输入的新密码不一致");
        return false;
    }

    QString pwdErr;
    if (!AppSettings::setPassword(newPassword, &pwdErr)) {
        if (pwdErr == QStringLiteral("密码至少 4 位"))
            pwdErr = tr("密码至少 4 位");
        else if (pwdErr == QStringLiteral("无法写入密码文件"))
            pwdErr = tr("无法写入密码文件");
        if (error)
            *error = pwdErr;
        return false;
    }

    ui->btnClearPassword->setEnabled(true);
    ui->checkLockOnStartup->setEnabled(true);
    return true;
}

void AppSettingsDialog::onSave()
{
    const QString previousLanguage = AppSettings::uiLanguage();

    QString err;
    if (!applyPasswordChange(&err)) {
        QMessageBox::warning(this, tr("设置"), err);
        return;
    }

    AppSettings::setLockAfterSystemLockMinutes(ui->spinLockAfterSystemLock->value());
    AppSettings::setLockOnIdleMinutes(ui->spinLockOnIdle->value());
    AppSettings::setLockOnStartup(ui->checkLockOnStartup->isChecked());
    AppSettings::setMinimizeToTray(ui->checkMinimizeToTray->isChecked());
    AppSettings::setUsageStatisticsEnabled(ui->checkUsageStatistics->isChecked());

    const QKeySequence todayHotkey = ui->editTodayTasksHotkey->keySequence();
    const QKeySequence top3Hotkey = ui->editTop3Hotkey->keySequence();
    const QKeySequence captureHotkey = ui->editQuickCaptureHotkey->keySequence();
    QString hotkeyErr;
    if (!globalHotkeySequenceValid(todayHotkey, &hotkeyErr)) {
        QMessageBox::warning(this, tr("设置"), tr("今日任务：%1").arg(hotkeyErr));
        return;
    }
    if (!globalHotkeySequenceValid(top3Hotkey, &hotkeyErr)) {
        QMessageBox::warning(this, tr("设置"), tr("Top 3 弹窗：%1").arg(hotkeyErr));
        return;
    }
    if (!globalHotkeySequenceValid(captureHotkey, &hotkeyErr)) {
        QMessageBox::warning(this, tr("设置"), tr("闪记：%1").arg(hotkeyErr));
        return;
    }
    if (todayHotkey == top3Hotkey || todayHotkey == captureHotkey || top3Hotkey == captureHotkey) {
        QMessageBox::warning(this, tr("设置"), tr("快捷键不能相同"));
        return;
    }

    AppSettings::setTodayTasksHotkey(todayHotkey);
    AppSettings::setTop3PopupHotkey(top3Hotkey);
    AppSettings::setQuickCaptureHotkey(captureHotkey);
    AppSettings::setQuickCaptureAutoAnalyze(ui->checkQuickCaptureAutoAnalyze->isChecked());
    AppSettings::setFocusDurationMinutes(ui->comboFocusDuration->currentData().toInt());
    AppSettings::setFocusTrayCountdown(ui->checkFocusTrayCountdown->isChecked());

    const QString selectedLanguage = ui->comboLanguage->currentData().toString();
    AppSettings::setUiLanguage(selectedLanguage);

    accept();
    emit hotkeysChanged();

    if (selectedLanguage != previousLanguage) {
        AppTranslations::installForApplication(qobject_cast<QApplication *>(QCoreApplication::instance()));
        AppTranslations::notifyLanguageChanged();
    }
}

void AppSettingsDialog::onClearPassword()
{
    if (!AppSettings::hasPassword())
        return;

    if (QMessageBox::question(this, tr("清除密码"), tr("确定清除登录密码并关闭所有锁定功能？"))
        != QMessageBox::Yes)
        return;

    if (!AppSettings::verifyPassword(ui->editCurrentPassword->text())) {
        QMessageBox::warning(this, tr("清除密码"), tr("请先输入正确的当前密码"));
        return;
    }

    AppSettings::clearPassword();
    ui->btnClearPassword->setEnabled(false);
    ui->checkLockOnStartup->setEnabled(false);
    ui->checkLockOnStartup->setChecked(false);
    ui->editCurrentPassword->clear();
    ui->editNewPassword->clear();
    ui->editConfirmPassword->clear();
    QMessageBox::information(this, tr("清除密码"), tr("密码已清除"));
}

void AppSettingsDialog::onLockNow()
{
    if (!AppSettings::hasPassword()) {
        QMessageBox::information(this, tr("立即锁定"), tr("请先设置登录密码"));
        return;
    }
    emit lockRequested();
    accept();
}
