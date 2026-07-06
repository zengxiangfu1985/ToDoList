#include "m365settingsdialog.h"
#include "ui_m365settingsdialog.h"

#include "../core/m365/m365_auth_service.h"
#include "../core/m365/m365_sync_service.h"
#include "../core/task_repository.h"
#include "../utils/app_theme.h"

#include <QClipboard>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QMessageBox>
#include <QTimer>
#include <QUrl>

M365SettingsDialog::M365SettingsDialog(M365AuthService *auth, M365SyncService *sync, TaskRepository *repo,
                                         QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::M365SettingsDialog)
    , m_auth(auth)
    , m_sync(sync)
    , m_repo(repo)
{
    ui->setupUi(this);
    AppTheme::styleDialog(this);
    setWindowTitle(tr("Microsoft 365 集成"));

    const M365Config cfg = m_auth->config();
    ui->editTenant->setText(cfg.tenantId);
    ui->editClientId->setText(cfg.clientId);
    ui->labelAuthStatus->setText(cfg.isConfigured() ? tr("已登录") : tr("未登录"));

    connect(ui->btnLogin, &QPushButton::clicked, this, &M365SettingsDialog::onStartLogin);
    connect(ui->btnPoll, &QPushButton::clicked, this, &M365SettingsDialog::onPollToken);
    connect(ui->btnSync, &QPushButton::clicked, this, &M365SettingsDialog::onSyncMail);
    connect(ui->btnOpenUrl, &QPushButton::clicked, this, [this]() {
        QDesktopServices::openUrl(QUrl(ui->labelDeviceUrl->text()));
    });
    connect(ui->btnCopyCode, &QPushButton::clicked, this, [this]() {
        QGuiApplication::clipboard()->setText(ui->labelUserCode->text());
    });
    connect(m_auth, &M365AuthService::deviceCodeReady, this, &M365SettingsDialog::onDeviceCode);
    connect(m_sync, &M365SyncService::syncFinished, this, &M365SettingsDialog::onSyncFinished);
}

M365SettingsDialog::~M365SettingsDialog()
{
    delete ui;
}

void M365SettingsDialog::onStartLogin()
{
    M365Config cfg;
    cfg.tenantId = ui->editTenant->text().trimmed();
    cfg.clientId = ui->editClientId->text().trimmed();
    m_auth->setConfig(cfg);
    M365AuthService::saveToSettings(cfg);

    QString err;
    if (!m_auth->startDeviceCodeFlow(&err))
        QMessageBox::warning(this, tr("登录"), err);
}

void M365SettingsDialog::onPollToken()
{
    QString err;
    if (m_auth->pollAccessToken(&err)) {
        ui->labelAuthStatus->setText(tr("已登录"));
        QMessageBox::information(this, tr("登录"), tr("Microsoft 365 授权成功"));
    } else if (!err.isEmpty() && err != QStringLiteral("authorization_pending")) {
        QMessageBox::warning(this, tr("登录"), err);
    } else {
        QMessageBox::information(this, tr("登录"), tr("尚未完成授权，请在浏览器中完成登录后重试"));
    }
}

void M365SettingsDialog::onSyncMail()
{
    ui->btnSync->setEnabled(false);
    m_sync->syncFlaggedMail(m_repo);
}

void M365SettingsDialog::onDeviceCode(const QString &url, const QString &code)
{
    ui->labelDeviceUrl->setText(url);
    ui->labelUserCode->setText(code);
}

void M365SettingsDialog::onSyncFinished(const M365SyncResult &result)
{
    ui->btnSync->setEnabled(true);
    if (!result.success) {
        QMessageBox::warning(this, tr("同步"), result.errorMessage);
        return;
    }
    QMessageBox::information(this, tr("同步"),
                             tr("已导入 %1 封 flagged 邮件，跳过 %2 条重复项")
                                 .arg(result.imported)
                                 .arg(result.skipped));
}
