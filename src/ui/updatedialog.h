#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include "../core/update/update_service.h"

#include <QDialog>

class QShowEvent;

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT
public:
    enum class Mode {
        CheckOnline,
        OfflinePackage
    };

    explicit UpdateDialog(UpdateService *service, QWidget *parent = nullptr);
    ~UpdateDialog() override;

    void startOnlineCheck();
    void startOfflineImport(const QString &zipPath);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onCheckFinished(bool hasUpdate);
    void onDownloadProgressChanged(int percent);
    void onDownloadFinished(bool success);
    void onErrorOccurred(const QString &message);
    void onUpgradeClicked();
    void onCloseClicked();

private:
    void setStatusText(const QString &text);
    void showReleaseNotes(const UpdatePackageInfo &package);

    Ui::UpdateDialog *ui;
    UpdateService *m_service = nullptr;
    Mode m_mode = Mode::CheckOnline;
    bool m_hasUpdate = false;
};

#endif // UPDATEDIALOG_H
