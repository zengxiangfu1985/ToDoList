#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "../utils/app_theme.h"
#include "../utils/app_version.h"
#include "../utils/window_fit.h"

#include <QFileDialog>
#include <QShowEvent>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("关于"));
    setMinimumWidth(360);
    AppTheme::styleDialog(this);

    ui->labelVersion->setText(AppVersion::displayString());

    connect(ui->btnCheckUpdate, &QPushButton::clicked, this, &AboutDialog::onCheckUpdate);
    connect(ui->btnImportOfflineUpdate, &QPushButton::clicked, this,
            &AboutDialog::onImportOfflineUpdate);
    connect(ui->btnClose, &QPushButton::clicked, this, &QDialog::accept);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    WindowFit::fitDialog(this, 420);
}

void AboutDialog::onCheckUpdate()
{
    emit checkUpdateRequested();
}

void AboutDialog::onImportOfflineUpdate()
{
    const QString zipPath = QFileDialog::getOpenFileName(
        this, tr("选择离线更新包"), QString(),
        tr("便携版更新包 (*.zip);;所有文件 (*.*)"));
    if (zipPath.isEmpty())
        return;
    emit importOfflineUpdateRequested(zipPath);
}
