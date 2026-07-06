#ifndef M365SETTINGSDIALOG_H
#define M365SETTINGSDIALOG_H

#include "../core/task_types.h"

#include <QDialog>

class M365AuthService;
class M365SyncService;
class TaskRepository;

namespace Ui {
class M365SettingsDialog;
}

class M365SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit M365SettingsDialog(M365AuthService *auth, M365SyncService *sync, TaskRepository *repo,
                                QWidget *parent = nullptr);
    ~M365SettingsDialog() override;

private slots:
    void onStartLogin();
    void onPollToken();
    void onSyncMail();
    void onDeviceCode(const QString &url, const QString &code);
    void onSyncFinished(const M365SyncResult &result);

private:
    Ui::M365SettingsDialog *ui;
    M365AuthService *m_auth;
    M365SyncService *m_sync;
    TaskRepository *m_repo;
};

#endif // M365SETTINGSDIALOG_H
