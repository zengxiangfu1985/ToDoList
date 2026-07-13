#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../core/task_types.h"
#include "../core/weekly_report_service.h"

#include "updatedialog.h"

#include <QDate>
#include <QHash>
#include <QMainWindow>
#include <QQueue>
#include <QSystemTrayIcon>
#include <QTimer>

class QAction;
class QMenu;
class QMenuBar;
class QScrollArea;

class TaskRepository;
class TaskTableModel;
class LlmService;
class WindowsHotkeyManager;
class QuadrantBoard;
class BehaviorLearningEngine;
class DailyEvaluationService;
class WeeklyReportService;
class M365AuthService;
class MicrosoftGraphClient;
class M365SyncService;
class WindowTitleBar;
class LockScreenWidget;
class WindowsSessionMonitor;
class AiBusyOverlay;
class UpdateService;
class UpdateDialog;
class UsageReportService;
class FocusSessionService;
class FocusSessionDialog;
class HabitReminderRepository;
class HabitReminderService;
class HabitReminderPopup;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void requestShowFromAnotherInstance();

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void reloadTasks();
    void scheduleReloadTasks();
    void onTodayTasks();
    void onAddTask();
    void onAiAnalyze();
    void onViewAiTrace();
    void onResetQuadrants();
    void onLlmSettings();
    void onM365Settings();
    void onViewHistory();
    void onViewDailyEvaluations();
    void onExportWeeklyReport();
    void onAppSettings();
    void onAbout();
    void onCheckForUpdates();
    void onImportOfflineUpdate(const QString &zipPath);
    void onStartupUpdateCheck();
    void onDeleteTasks();
    void onCancelDeleteMode();
    void onTaskContextMenu(const QPoint &pos);
    void onTaskDoubleClicked(const QModelIndex &index);
    void onTaskCompletedToggled(qint64 taskId, bool completed);
    void onQuadrantChanged(qint64 taskId, EisenhowerQuadrant from, EisenhowerQuadrant to);
    void onAnalysisStarted();
    void onAnalysisFinished(const PriorityAnalysisResult &result);
    void onDailyEvaluationFinished(const QDate &date, const DailyEvaluation &result);
    void onWeeklyReportFinished(const WeeklyReportRecord &record, const WeeklyReportResult &result);
    void onProviderChanged(LlmProviderType type);
    void onTop3Clicked();
    void showQuickCapture();
    void showMainWindow();
    void showTop3Popup();
    void startFocus25(qint64 taskId);
    void quitApplication();
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void setupWindowChrome();
    void setupMenus();
    void retranslateDynamicUi();
    void setupResponsiveLayout();
    void updateToolbarScroll();
    void toggleMaximize();
    void setupConnections();
    void setupTrayIcon();
    void setupQuadrantBoard();
    void applyPanelElevation();
    void setupGlobalHotkeys();
    void reloadGlobalHotkeys(bool fromSettings = false);
    void updateTrayHotkeyLabels();
    void setupSecurity();
    void lockApp(bool startupLogin = false);
    void unlockApp();
    void resetIdleTimer();
    void onCloseRequested();
    void minimizeToTray();
    void updateProviderStatus();
    void updateLearningStatus();
    QString providerDisplayName(LlmProviderType type) const;
    void applyTop3ToUi(const PriorityAnalysisResult &result);
    void restoreSavedTop3();
    void saveCurrentTop3();
    void clearTop3Ui();
    void syncTop3WithTasks(const QVector<TaskItem> &tasks);
    void removeTaskFromSyncedViews(qint64 taskId);
    void setupFocusSession();
    void setupHabitReminders();
    void updateTrayHabitMenu();
    void showNextHabitReminder();
    void onHabitReminderDue(const HabitReminder &habit);
    void onFocus25Clicked();
    void onFocusTick(int remainingSec);
    void onFocusSessionEnded();
    void updateTrayTooltipForFocus(int remainingSec);
    void restoreDefaultTrayTooltip();
    void endFocusUi();
    void closeFocusSessionRecord(bool completed, bool abandoned);
    QString taskTitleForId(qint64 taskId) const;
    int focusDurationSeconds() const;
    EisenhowerQuadrant quadrantForTaskId(qint64 taskId) const;
    QHash<qint64, bool> taskCompletionMap() const;
    void archiveTodaySnapshot();
    void enterDeleteMode();
    void exitDeleteMode();
    void deleteTasksByIds(const QVector<qint64> &ids);
    ScoringWeights currentWeights() const;
    void showUpdateDialog(UpdateDialog::Mode mode, const QString &offlineZipPath = QString(),
                          bool recheckOnline = true);

    Ui::MainWindow *ui;
    WindowTitleBar *m_titleBar = nullptr;
    QMenuBar *m_menuBar = nullptr;
    QMenu *m_viewMenu = nullptr;
    QMenu *m_helpMenu = nullptr;
    QAction *m_menuMinimizeAction = nullptr;
    QAction *m_menuMaximizeAction = nullptr;
    QAction *m_menuOpenLogDirAction = nullptr;
    QAction *m_menuOpenLogFileAction = nullptr;
    QAction *m_menuOpenAiTraceAction = nullptr;
    QScrollArea *m_toolbarScroll = nullptr;
    TaskRepository *m_repo;
    TaskTableModel *m_model;
    LlmService *m_llmService;
    DailyEvaluationService *m_dailyEvalService;
    WeeklyReportService *m_weeklyReportService;
    BehaviorLearningEngine *m_learning;
    QuadrantBoard *m_quadrantBoard;
    M365AuthService *m_m365Auth;
    MicrosoftGraphClient *m_m365Graph;
    M365SyncService *m_m365Sync;
    QSystemTrayIcon *m_trayIcon;
    QAction *m_trayTodayTasksAction = nullptr;
    QAction *m_trayQuickCaptureAction = nullptr;
    QAction *m_trayTop3Action = nullptr;
    QAction *m_trayFocus25Action = nullptr;
    WindowsHotkeyManager *m_hotkeyManager;
    LockScreenWidget *m_lockScreen = nullptr;
    AiBusyOverlay *m_aiBusyOverlay = nullptr;
    UpdateService *m_updateService = nullptr;
    UpdateDialog *m_updateDialog = nullptr;
    UsageReportService *m_usageReport = nullptr;
    FocusSessionService *m_focusService = nullptr;
    FocusSessionDialog *m_focusDialog = nullptr;
    HabitReminderRepository *m_habitRepo = nullptr;
    HabitReminderService *m_habitService = nullptr;
    HabitReminderPopup *m_habitPopup = nullptr;
    QMenu *m_trayHabitMenu = nullptr;
    QHash<qint64, QAction *> m_trayHabitActions;
    QQueue<HabitReminder> m_pendingHabitReminders;
    QTimer *m_usageHeartbeatTimer = nullptr;
    WindowsSessionMonitor *m_sessionMonitor = nullptr;
    QTimer *m_systemLockTimer = nullptr;
    QTimer *m_idleTimer = nullptr;
    PriorityAnalysisResult m_lastAnalysis;
    QString m_loadedTop3ModelKey;
    bool m_hotkeysInstalled = false;
    bool m_locked = false;
    bool m_startupLockPending = false;
    bool m_reloadTasksScheduled = false;
    bool m_startupUpdateCheckDone = false;
};

#endif // MAINWINDOW_H
