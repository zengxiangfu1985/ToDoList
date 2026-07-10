#include "updatedialog.h"
#include "ui_updatedialog.h"

#include "../core/update/update_service.h"
#include "../core/telemetry/usage_report_service.h"
#include "../utils/app_theme.h"
#include "../utils/app_version.h"
#include "../utils/app_version.h"
#include "../utils/window_fit.h"

#include <QApplication>
#include <QMessageBox>
#include <QShowEvent>

UpdateDialog::UpdateDialog(UpdateService *service, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UpdateDialog)
    , m_service(service)
{
    ui->setupUi(this);
    setWindowTitle(tr("检查更新"));
    setMinimumWidth(420);
    AppTheme::styleDialog(this);
    ui->progressBar->setVisible(false);
    ui->textReleaseNotes->setVisible(false);
    ui->btnUpgrade->setVisible(false);

    connect(ui->btnUpgrade, &QPushButton::clicked, this, &UpdateDialog::onUpgradeClicked);
    connect(ui->btnClose, &QPushButton::clicked, this, &UpdateDialog::onCloseClicked);

    if (m_service) {
        connect(m_service, &UpdateService::checkFinished, this, &UpdateDialog::onCheckFinished);
        connect(m_service, &UpdateService::downloadProgressChanged, this,
                &UpdateDialog::onDownloadProgressChanged);
        connect(m_service, &UpdateService::downloadFinished, this, &UpdateDialog::onDownloadFinished);
        connect(m_service, &UpdateService::errorOccurred, this, &UpdateDialog::onErrorOccurred);
    }
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

void UpdateDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    WindowFit::fitDialog(this, 480);
}

void UpdateDialog::setStatusText(const QString &text)
{
    ui->labelStatus->setText(text);
}

void UpdateDialog::showReleaseNotes(const UpdatePackageInfo &package)
{
    ui->textReleaseNotes->setVisible(true);
    ui->textReleaseNotes->setPlainText(
        tr("新版本: %1 (build %2)\n\n%3")
            .arg(package.version)
            .arg(package.build)
            .arg(package.releaseNotes.isEmpty() ? tr("暂无更新说明") : package.releaseNotes));
}

void UpdateDialog::startOnlineCheck(bool recheck)
{
    m_mode = Mode::CheckOnline;
    m_downloadStarted = false;
    if (!m_service) {
        setStatusText(tr("当前版本: %1\n无法检查更新。").arg(AppVersion::displayString()));
        return;
    }

    if (!recheck) {
        startWithKnownUpdate();
        return;
    }

    if (m_service->state() == UpdateService::State::Downloading) {
        setStatusText(tr("当前版本: %1\n正在检查更新...").arg(AppVersion::displayString()));
        syncDownloadUi();
        return;
    }

    if (m_service->state() == UpdateService::State::Ready) {
        const UpdatePackageInfo package = m_service->latestPackage();
        if (AppVersion::isNewerThanCurrent(package.version, package.build)) {
            startWithKnownUpdate();
            return;
        }
    }

    setStatusText(tr("当前版本: %1\n正在检查更新...").arg(AppVersion::displayString()));
    m_service->checkForUpdates();
}

void UpdateDialog::startWithKnownUpdate()
{
    m_mode = Mode::CheckOnline;
    if (!m_service)
        return;

    const UpdatePackageInfo package = m_service->latestPackage();
    if (!AppVersion::isNewerThanCurrent(package.version, package.build)) {
        onCheckFinished(false);
        return;
    }

    m_hasUpdate = true;
    showReleaseNotes(package);
    ui->btnUpgrade->setVisible(true);

    if (m_service->state() == UpdateService::State::Ready
        && m_service->hasValidCachedDownload(package)) {
        syncDownloadUi();
        onDownloadFinished(true);
        return;
    }

    if (m_service->state() == UpdateService::State::Downloading) {
        syncDownloadUi();
        return;
    }

    beginDownloadIfNeeded(package);
}

void UpdateDialog::syncDownloadUi()
{
    if (!m_service)
        return;

    ui->progressBar->setVisible(true);
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(m_service->downloadProgress());
    if (m_service->state() == UpdateService::State::Ready) {
        setStatusText(tr("下载完成，可以升级。"));
        ui->btnUpgrade->setVisible(true);
        m_hasUpdate = true;
    } else {
        setStatusText(tr("正在下载更新包... %1%").arg(m_service->downloadProgress()));
    }
}

void UpdateDialog::beginDownloadIfNeeded(const UpdatePackageInfo &package)
{
    if (!m_service || package.url.isEmpty())
        return;
    if (m_downloadStarted)
        return;

    m_downloadStarted = true;
    ui->progressBar->setVisible(true);
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
    setStatusText(tr("正在下载更新包..."));
    m_service->downloadUpdate(package);
}

void UpdateDialog::startOfflineImport(const QString &zipPath)
{
    m_mode = Mode::OfflinePackage;
    setStatusText(tr("正在校验离线更新包..."));
    QString error;
    if (!m_service || !m_service->prepareOfflineUpdate(zipPath, &error)) {
        setStatusText(error.isEmpty() ? tr("离线更新包无效") : error);
        return;
    }

    m_hasUpdate = true;
    showReleaseNotes(m_service->latestPackage());
    ui->btnUpgrade->setVisible(true);
    ui->btnUpgrade->setText(tr("立即升级"));
    setStatusText(tr("离线更新包校验通过，可以升级。"));
}

void UpdateDialog::onCheckFinished(bool hasUpdate)
{
    m_hasUpdate = hasUpdate;
    if (!hasUpdate) {
        if (m_service && !m_service->lastError().isEmpty()) {
            setStatusText(tr("当前版本: %1\n%2")
                              .arg(AppVersion::displayString(), m_service->lastError()));
            return;
        }
        const UpdatePackageInfo remote = m_service ? m_service->latestPackage() : UpdatePackageInfo{};
        if (remote.valid) {
            setStatusText(tr("当前版本: %1\n服务器版本: %2 (build %3)\n已是最新版本。")
                              .arg(AppVersion::displayString(), remote.version)
                              .arg(remote.build));
        } else {
            setStatusText(tr("当前版本: %1\n已是最新版本。").arg(AppVersion::displayString()));
        }
        return;
    }

    const UpdatePackageInfo package = m_service->latestPackage();
    showReleaseNotes(package);
    ui->btnUpgrade->setVisible(true);
    setStatusText(tr("发现新版本，可立即升级。"));
    beginDownloadIfNeeded(package);
}

void UpdateDialog::onDownloadProgressChanged(int percent)
{
    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(percent);
    setStatusText(tr("正在下载更新包... %1%").arg(percent));
}

void UpdateDialog::onDownloadFinished(bool success)
{
    if (!success) {
        ui->progressBar->setValue(0);
        m_hasUpdate = false;
        ui->btnUpgrade->setVisible(false);
        if (m_service && !m_service->lastError().isEmpty())
            setStatusText(m_service->lastError());
        else
            setStatusText(tr("下载更新包失败，请稍后重试或使用「导入离线更新包」。"));
        return;
    }

    ui->progressBar->setValue(100);
    setStatusText(tr("下载完成，可以升级。"));
    ui->btnUpgrade->setVisible(true);
    m_hasUpdate = true;
}

void UpdateDialog::onErrorOccurred(const QString &message)
{
    setStatusText(message);
}

void UpdateDialog::onUpgradeClicked()
{
    if (!m_service || !m_hasUpdate)
        return;

    if (QMessageBox::question(this, tr("确认升级"),
                            tr("升级将关闭当前程序并保留 data 目录中的数据。是否继续？"))
        != QMessageBox::Yes)
        return;

    QString error;
    if (!m_service->launchUpdaterAndExit(m_service->latestPackage(), &error)) {
        QMessageBox::warning(this, tr("升级失败"), error);
        return;
    }

    UsageReportService reporter(this);
    reporter.reportUpgradeSuccess(AppVersion::versionString());

    QApplication::quit();
}

void UpdateDialog::onCloseClicked()
{
    reject();
}
