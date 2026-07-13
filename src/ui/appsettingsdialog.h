#ifndef APPSETTINGSDIALOG_H
#define APPSETTINGSDIALOG_H

#include <QDialog>

class HabitReminderRepository;
class HabitReminderService;

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
    void setHabitDependencies(HabitReminderRepository *repo, HabitReminderService *service);

protected:
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;

signals:
    void lockRequested();
    void viewAiTraceRequested();
    void m365SettingsRequested();
    void hotkeysChanged();
    void habitsChanged();

private slots:
    void onSave();
    void onClearPassword();
    void onLockNow();
    void onViewAiTrace();
    void onM365Settings();

private:
    void loadToUi();
    void loadHabitsToUi();
    bool saveHabits(QString *error);
    bool applyPasswordChange(QString *error);

    Ui::AppSettingsDialog *ui;
    HabitReminderRepository *m_habitRepo = nullptr;
    HabitReminderService *m_habitService = nullptr;
};

#endif // APPSETTINGSDIALOG_H
