#ifndef APPSETTINGSDIALOG_H
#define APPSETTINGSDIALOG_H

#include <QDialog>

class QShowEvent;

namespace Ui {
class AppSettingsDialog;
}

class AppSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AppSettingsDialog(QWidget *parent = nullptr);
    ~AppSettingsDialog() override;

    void setViewAiTraceEnabled(bool enabled);

protected:
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;

signals:
    void lockRequested();
    void viewAiTraceRequested();
    void m365SettingsRequested();
    void hotkeysChanged();

private slots:
    void onSave();
    void onClearPassword();
    void onLockNow();
    void onViewAiTrace();
    void onM365Settings();

private:
    void loadToUi();
    bool applyPasswordChange(QString *error);

    Ui::AppSettingsDialog *ui;
};

#endif // APPSETTINGSDIALOG_H
