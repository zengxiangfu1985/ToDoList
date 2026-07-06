#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addtaskdialog.h"
#include "llmsettingsdialog.h"
#include "top3popupdialog.h"
#include "m365settingsdialog.h"
#include "quadrantboard.h"
#include "taskhistorydialog.h"
#include "todaytasksdialog.h"
#include "daily_evaluation_dialog.h"
#include "weekly_report_dialog.h"
#include "aianalysistracedialog.h"
#include "ai_busy_overlay.h"
#include "appsettingsdialog.h"
#include "lock_screen_widget.h"
#include "windowtitlebar.h"

#include "../core/app_settings.h"

#include "../core/task_repository.h"
#include "../core/task_archive.h"
#include "../core/priority_engine.h"
#include "../core/behavior_learning_engine.h"
#include "../core/daily_evaluation_service.h"
#include "../core/weekly_report_service.h"
#include "../core/ai/llm_service.h"
#include "../core/ai/llm_provider.h"
#include "../core/m365/m365_auth_service.h"
#include "../core/m365/microsoft_graph_client.h"
#include "../core/m365/m365_sync_service.h"
#include "../models/task_table_model.h"
#include "../ui/task_table_delegate.h"
#include "../utils/windows_hotkey_manager.h"
#include "../utils/windows_session_monitor.h"
#include "../utils/global_hotkey.h"
#include "../utils/app_icon.h"
#include "../utils/app_theme.h"
#include "../utils/app_logger.h"
#include "../utils/ai_analysis_trace.h"
#include "../utils/window_fit.h"

#include <QAction>
#include <QCloseEvent>
#include <QFrame>
#include <QScrollArea>
#include <QCursor>
#include <QDesktopServices>
#include <QDir>
#include <QEvent>
#include <QFileInfo>
#include <QGroupBox>
#include <QHeaderView>
#include <QIcon>
#include <QKeySequence>
#include <QListWidgetItem>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QModelIndex>
#include <QShowEvent>
#include <QSizePolicy>
#include <QStandardPaths>
#include <QResizeEvent>
#include <QStatusBar>
#include <QTime>
#include <QUrl>
#include "updatedialog.h"

#include "../core/update/update_config.h"
#include "../core/update/update_service.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_repo(new TaskRepository(this))
    , m_model(new TaskTableModel(this))
    , m_llmService(new LlmService(this))
    , m_dailyEvalService(new DailyEvaluationService(m_repo, this))
    , m_weeklyReportService(new WeeklyReportService(m_repo, this))
    , m_learning(new BehaviorLearningEngine(m_repo, this))
    , m_quadrantBoard(new QuadrantBoard(this))
    , m_m365Auth(new M365AuthService(this))
    , m_m365Graph(new MicrosoftGraphClient(m_m365Auth, this))
    , m_m365Sync(new M365SyncService(m_m365Graph, this))
    , m_trayIcon(new QSystemTrayIcon(qApp))
    , m_hotkeyManager(new WindowsHotkeyManager(this))
    , m_updateService(new UpdateService(this))
{
    ui->setupUi(this);
    setWindowTitle(tr("ToDoList — AI 智能待办"));

    setupWindowChrome();
    setupMenus();
    setupResponsiveLayout();

    ui->tableTasks->setModel(m_model);
    ui->tableTasks->setItemDelegate(new TaskTableDelegate(m_model, ui->tableTasks));
    ui->tableTasks->setShowGrid(false);
    ui->tableTasks->setAlternatingRowColors(false);
    ui->tableTasks->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableTasks->verticalHeader()->setVisible(false);
    ui->tableTasks->verticalHeader()->setDefaultSectionSize(AppTheme::metrics().rowHeight);
    ui->tableTasks->setWordWrap(false);
    ui->tableTasks->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableTasks->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHeaderView *header = ui->tableTasks->horizontalHeader();
    header->setStretchLastSection(false);
    header->setDefaultAlignment(Qt::AlignCenter);
    header->setMinimumSectionSize(48);
    header->setSectionResizeMode(TaskTableModel::ColSelect, QHeaderView::Fixed);
    header->setSectionResizeMode(TaskTableModel::ColIndex, QHeaderView::Fixed);
    header->setSectionResizeMode(TaskTableModel::ColTitle, QHeaderView::Stretch);
    header->setSectionResizeMode(TaskTableModel::ColDue, QHeaderView::Interactive);
    header->setSectionResizeMode(TaskTableModel::ColQuadrant, QHeaderView::Interactive);
    header->setSectionResizeMode(TaskTableModel::ColScore, QHeaderView::Interactive);
    header->setSectionResizeMode(TaskTableModel::ColCompleted, QHeaderView::Fixed);
    ui->tableTasks->setColumnWidth(TaskTableModel::ColSelect, 52);
    ui->tableTasks->setColumnWidth(TaskTableModel::ColIndex, 48);
    ui->tableTasks->setColumnWidth(TaskTableModel::ColDue, 150);
    ui->tableTasks->setColumnWidth(TaskTableModel::ColQuadrant, 72);
    ui->tableTasks->setColumnWidth(TaskTableModel::ColScore, 64);
    ui->tableTasks->setColumnWidth(TaskTableModel::ColCompleted, 72);
    ui->tableTasks->setColumnHidden(TaskTableModel::ColSelect, true);
    ui->tableTasks->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->splitterMain->setStretchFactor(0, 3);
    ui->splitterMain->setStretchFactor(1, 2);

    QString dbErr;
    if (!TaskArchive::ensureDataDirectory(&dbErr))
        QMessageBox::warning(this, tr("数据目录"), dbErr);

    const QString dbPath = TaskArchive::databasePath();

    QString err;
    if (!m_repo->open(dbPath, &err))
        QMessageBox::critical(this, tr("数据库"), err);

    setupQuadrantBoard();
    applyPanelElevation();
    setupConnections();
    setupTrayIcon();
    setupSecurity();
    m_aiBusyOverlay = new AiBusyOverlay(this);
    reloadTasks();
    archiveTodaySnapshot();
    updateProviderStatus();
    updateLearningStatus();
    m_llmService->setScoringWeights(m_learning->currentWeights());
    m_learning->refreshWeights();
    m_dailyEvalService->setLlmConfig(m_llmService->config());
    m_weeklyReportService->setLlmConfig(m_llmService->config());
    m_dailyEvalService->start();
    restoreSavedTop3();

    QTimer::singleShot(5000, this, &MainWindow::onStartupUpdateCheck);
}

void MainWindow::applyPanelElevation()
{
    AppTheme::applyElevation(ui->tableTasks, 20, 3, 50);
    AppTheme::applyElevation(ui->groupQuadrants, 22, 3, 58);
    AppTheme::applyElevation(ui->groupAi, 18, 2, 48);
}

MainWindow::~MainWindow()
{
    m_hotkeyManager->uninstall();
    delete ui;
}

void MainWindow::setupQuadrantBoard()
{
    m_quadrantBoard->bindQuadrantContainers(ui->quadrantQ1, ui->quadrantQ2, ui->quadrantQ3, ui->quadrantQ4);
}

void MainWindow::setupWindowChrome()
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    m_titleBar = new WindowTitleBar(this);
    m_titleBar->setTitle(windowTitle());

    auto *layout = qobject_cast<QVBoxLayout *>(ui->centralwidget->layout());
    if (layout)
        layout->insertWidget(0, m_titleBar);

    connect(m_titleBar, &WindowTitleBar::minimizeRequested, this, &QWidget::showMinimized);
    connect(m_titleBar, &WindowTitleBar::maximizeRequested, this, &MainWindow::toggleMaximize);
    connect(m_titleBar, &WindowTitleBar::closeRequested, this, &MainWindow::onCloseRequested);

    AppTheme::applyDarkTitleBar(this);
}

void MainWindow::setupMenus()
{
    m_menuBar = new QMenuBar(this);
    m_menuBar->setNativeMenuBar(false);
    m_menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_menuBar->setFixedHeight(AppTheme::metrics().menuBarHeight);

    auto *viewMenu = m_menuBar->addMenu(tr("视图"));
    m_viewMenu = viewMenu;
    m_menuMinimizeAction = viewMenu->addAction(tr("最小化"));
    m_menuMinimizeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_M));
    connect(m_menuMinimizeAction, &QAction::triggered, this, &QWidget::showMinimized);

    m_menuMaximizeAction = viewMenu->addAction(tr("最大化 / 还原"));
    m_menuMaximizeAction->setShortcut(QKeySequence(Qt::Key_F11));
    connect(m_menuMaximizeAction, &QAction::triggered, this, &MainWindow::toggleMaximize);

    auto *helpMenu = m_menuBar->addMenu(tr("帮助"));
    m_helpMenu = helpMenu;
    m_menuOpenLogDirAction = helpMenu->addAction(tr("打开日志目录"));
    connect(m_menuOpenLogDirAction, &QAction::triggered, this, []() {
        const QString dir = AppLogger::logDirectory();
        QDir().mkpath(dir);
        QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
        AppLogger::info("UI", QStringLiteral("用户打开日志目录: %1").arg(dir));
    });
    m_menuOpenLogFileAction = helpMenu->addAction(tr("打开今日日志"));
    connect(m_menuOpenLogFileAction, &QAction::triggered, this, []() {
        const QString path = AppLogger::currentLogFilePath();
        if (path.isEmpty() || !QFileInfo::exists(path)) {
            AppLogger::warn("UI", QStringLiteral("今日日志文件不存在: %1").arg(path));
            return;
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        AppLogger::info("UI", QStringLiteral("用户打开日志文件: %1").arg(path));
    });
    m_menuOpenAiTraceAction = helpMenu->addAction(tr("打开 AI 分析记录文件"));
    connect(m_menuOpenAiTraceAction, &QAction::triggered, this, []() {
        const QString path = AiAnalysisTrace::latestTraceFilePath();
        if (!QFileInfo::exists(path)) {
            QMessageBox::information(nullptr, QObject::tr("AI 分析过程"),
                                     QObject::tr("暂无 AI 分析记录。请先执行一次「AI 分析优先级」。"));
            return;
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    });

    auto *layout = qobject_cast<QVBoxLayout *>(ui->centralwidget->layout());
    if (layout)
        layout->insertWidget(1, m_menuBar);
}

void MainWindow::setupResponsiveLayout()
{
    auto *layout = qobject_cast<QVBoxLayout *>(ui->centralwidget->layout());
    if (!layout)
        return;

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    const int toolbarIdx = layout->indexOf(ui->toolBarPanel);
    if (toolbarIdx >= 0) {
        m_toolbarScroll = new QScrollArea(ui->centralwidget);
        m_toolbarScroll->setObjectName(QStringLiteral("toolbarScroll"));
        m_toolbarScroll->setFrameShape(QFrame::NoFrame);
        m_toolbarScroll->setWidgetResizable(false);
        m_toolbarScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_toolbarScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_toolbarScroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        layout->removeWidget(ui->toolBarPanel);
        ui->toolBarPanel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        m_toolbarScroll->setWidget(ui->toolBarPanel);
        layout->insertWidget(toolbarIdx, m_toolbarScroll);
    }

    for (int i = 0; i < layout->count(); ++i)
        layout->setStretch(i, 0);

    const int splitterIdx = layout->indexOf(ui->splitterMain);
    if (splitterIdx >= 0)
        layout->setStretch(splitterIdx, 1);

    if (m_titleBar)
        m_titleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->toolBarPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->centralwidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->splitterMain->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->splitterMain->setChildrenCollapsible(false);
    ui->rightPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tableTasks->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->rightLayout->setStretch(0, 3);
    ui->rightLayout->setStretch(1, 2);

    ui->gridQuadrants->setRowStretch(0, 1);
    ui->gridQuadrants->setRowStretch(1, 1);
    ui->gridQuadrants->setColumnStretch(0, 1);
    ui->gridQuadrants->setColumnStretch(1, 1);

    ui->aiLayout->setStretch(0, 2);
    ui->aiLayout->setStretch(1, 1);

    const QSizePolicy expanding(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->groupQuadrants->setSizePolicy(expanding);
    ui->groupAi->setSizePolicy(expanding);
    ui->quadrantQ1->setSizePolicy(expanding);
    ui->quadrantQ2->setSizePolicy(expanding);
    ui->quadrantQ3->setSizePolicy(expanding);
    ui->quadrantQ4->setSizePolicy(expanding);
    ui->listTop3->setSizePolicy(expanding);
    ui->textAiReason->setSizePolicy(expanding);

    updateToolbarScroll();
}

void MainWindow::updateToolbarScroll()
{
    if (!m_toolbarScroll || !ui->toolBarPanel)
        return;

    ui->toolBarPanel->adjustSize();
    const int toolbarHeight = ui->toolBarPanel->sizeHint().height();
    m_toolbarScroll->setFixedHeight(toolbarHeight);
    const int contentWidth = ui->toolBarPanel->sizeHint().width();
    const int viewportWidth = m_toolbarScroll->viewport()->width();
    ui->toolBarPanel->setMinimumWidth(qMax(contentWidth, viewportWidth));
    ui->toolBarPanel->resize(ui->toolBarPanel->minimumWidth(), toolbarHeight);
}

void MainWindow::toggleMaximize()
{
    if (isMaximized())
        showNormal();
    else
        showMaximized();

    if (m_titleBar)
        m_titleBar->updateMaximizeButton(isMaximized());
}

void MainWindow::retranslateDynamicUi()
{
    ui->retranslateUi(this);
    setWindowTitle(tr("ToDoList — AI 智能待办"));

    if (m_viewMenu)
        m_viewMenu->setTitle(tr("视图"));
    if (m_helpMenu)
        m_helpMenu->setTitle(tr("帮助"));
    if (m_menuMinimizeAction)
        m_menuMinimizeAction->setText(tr("最小化"));
    if (m_menuMaximizeAction)
        m_menuMaximizeAction->setText(tr("最大化 / 还原"));
    if (m_menuOpenLogDirAction)
        m_menuOpenLogDirAction->setText(tr("打开日志目录"));
    if (m_menuOpenLogFileAction)
        m_menuOpenLogFileAction->setText(tr("打开今日日志"));
    if (m_menuOpenAiTraceAction)
        m_menuOpenAiTraceAction->setText(tr("打开 AI 分析记录文件"));

    if (m_trayIcon)
        m_trayIcon->setToolTip(tr("ToDoList — AI 智能待办"));
    updateTrayHotkeyLabels();
    updateProviderStatus();
    updateLearningStatus();

    if (m_model && m_model->deleteMode())
        ui->btnDeleteTasks->setText(tr("确认删除"));
    else
        ui->btnDeleteTasks->setText(tr("删除任务"));

    if (!m_lastAnalysis.top3.isEmpty())
        applyTop3ToUi(m_lastAnalysis);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateDynamicUi();
    if (event->type() == QEvent::WindowStateChange && m_titleBar)
        m_titleBar->updateMaximizeButton(isMaximized());
    QMainWindow::changeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateToolbarScroll();
    if (m_lockScreen)
        m_lockScreen->setGeometry(rect());
    if (m_aiBusyOverlay)
        m_aiBusyOverlay->setGeometry(rect());
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == qApp && !m_locked && AppSettings::hasPassword()) {
        switch (event->type()) {
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::KeyPress:
        case QEvent::Wheel:
            resetIdleTimer();
            break;
        default:
            break;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::setupSecurity()
{
    m_lockScreen = new LockScreenWidget(this);
    m_lockScreen->setGeometry(rect());
    m_lockScreen->raise();

    m_sessionMonitor = new WindowsSessionMonitor(this);
    m_systemLockTimer = new QTimer(this);
    m_systemLockTimer->setSingleShot(true);
    m_idleTimer = new QTimer(this);
    m_idleTimer->setSingleShot(true);

    connect(m_lockScreen, &LockScreenWidget::unlockRequested, this, [this](const QString &password) {
        if (!AppSettings::hasPassword()) {
            m_lockScreen->showUnlockError(tr("尚未设置密码"));
            return;
        }
        if (AppSettings::verifyPassword(password))
            unlockApp();
        else
            m_lockScreen->showUnlockError(tr("密码错误，请重试"));
    });
    connect(m_lockScreen, &LockScreenWidget::exitRequested, this, [this]() {
        saveCurrentTop3();
        archiveTodaySnapshot();
        quitApplication();
    });

    connect(m_sessionMonitor, &WindowsSessionMonitor::sessionLocked, this, [this]() {
        if (!AppSettings::hasPassword())
            return;

        const int delayMin = AppSettings::lockAfterSystemLockMinutes();
        if (delayMin <= 0) {
            lockApp();
            return;
        }
        m_systemLockTimer->start(delayMin * 60 * 1000);
    });

    connect(m_sessionMonitor, &WindowsSessionMonitor::sessionUnlocked, this, [this]() {
        m_systemLockTimer->stop();
    });

    connect(m_systemLockTimer, &QTimer::timeout, this, [this]() {
        if (AppSettings::hasPassword())
            lockApp();
    });

    connect(m_idleTimer, &QTimer::timeout, this, [this]() {
        if (AppSettings::hasPassword())
            lockApp();
    });

    qApp->installEventFilter(this);
    resetIdleTimer();

    m_startupLockPending = AppSettings::hasPassword() && AppSettings::lockOnStartup();
    if (m_startupLockPending)
        ui->centralwidget->setVisible(false);
}

void MainWindow::lockApp(bool startupLogin)
{
    if (!AppSettings::hasPassword() || m_locked)
        return;

    m_locked = true;
    m_idleTimer->stop();
    m_systemLockTimer->stop();
    m_lockScreen->setLoginMode(startupLogin);
    ui->centralwidget->setVisible(false);
    if (statusBar())
        statusBar()->hide();
    m_lockScreen->setGeometry(rect());
    m_lockScreen->raise();
    m_lockScreen->show();
    m_lockScreen->activateWindow();
    m_lockScreen->setFocus();
}

void MainWindow::unlockApp()
{
    if (!m_locked)
        return;

    m_locked = false;
    m_lockScreen->onUnlockAccepted();
    ui->centralwidget->setVisible(true);
    if (statusBar())
        statusBar()->show();
    resetIdleTimer();
}

void MainWindow::resetIdleTimer()
{
    const int idleMin = AppSettings::lockOnIdleMinutes();
    if (idleMin <= 0 || !AppSettings::hasPassword() || m_locked) {
        m_idleTimer->stop();
        return;
    }
    m_idleTimer->start(idleMin * 60 * 1000);
}

void MainWindow::onAppSettings()
{
    AppSettingsDialog dlg(this);
    const bool hasTrace = !m_lastAnalysis.trace.isEmpty() || !m_lastAnalysis.quadrantAssignments.isEmpty()
                          || !m_lastAnalysis.top3.isEmpty();
    dlg.setViewAiTraceEnabled(hasTrace);
    connect(&dlg, &AppSettingsDialog::lockRequested, this, [this]() { lockApp(false); });
    connect(&dlg, &AppSettingsDialog::viewAiTraceRequested, this, &MainWindow::onViewAiTrace);
    connect(&dlg, &AppSettingsDialog::m365SettingsRequested, this, &MainWindow::onM365Settings);
    connect(&dlg, &AppSettingsDialog::hotkeysChanged, this, [this]() { reloadGlobalHotkeys(true); });
    connect(&dlg, &AppSettingsDialog::checkUpdateRequested, this, &MainWindow::onCheckForUpdates);
    connect(&dlg, &AppSettingsDialog::importOfflineUpdateRequested, this,
            &MainWindow::onImportOfflineUpdate);
    if (dlg.exec() == QDialog::Accepted)
        resetIdleTimer();
}

void MainWindow::showUpdateDialog(UpdateDialog::Mode mode, const QString &offlineZipPath)
{
    auto *dialog = new UpdateDialog(m_updateService, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    if (mode == UpdateDialog::Mode::CheckOnline)
        dialog->startOnlineCheck();
    else
        dialog->startOfflineImport(offlineZipPath);
    dialog->open();
}

void MainWindow::onCheckForUpdates()
{
    showUpdateDialog(UpdateDialog::Mode::CheckOnline);
}

void MainWindow::onImportOfflineUpdate(const QString &zipPath)
{
    showUpdateDialog(UpdateDialog::Mode::OfflinePackage, zipPath);
}

void MainWindow::onStartupUpdateCheck()
{
    if (m_startupUpdateCheckDone)
        return;
    m_startupUpdateCheckDone = true;

    const UpdateConfig config = UpdateConfigStore::load();
    if (!config.checkOnStartup)
        return;

    connect(m_updateService, &UpdateService::checkFinished, this,
            [this](bool hasUpdate) {
                disconnect(m_updateService, &UpdateService::checkFinished, this, nullptr);
                if (!hasUpdate)
                    return;
                if (m_updateService->latestPackage().mandatory) {
                    showUpdateDialog(UpdateDialog::Mode::CheckOnline);
                    return;
                }
                if (QMessageBox::information(this, tr("发现新版本"),
                                             tr("检测到 ToDoList 有新版本可用，是否查看并升级？"),
                                             QMessageBox::Yes | QMessageBox::No)
                    == QMessageBox::Yes)
                    showUpdateDialog(UpdateDialog::Mode::CheckOnline);
            });
    m_updateService->checkForUpdates();
}

void MainWindow::setupConnections()
{
    connect(m_repo, &TaskRepository::tasksChanged, this, &MainWindow::scheduleReloadTasks);
    connect(ui->btnTodayTasks, &QPushButton::clicked, this, &MainWindow::onTodayTasks);
    connect(ui->btnAddTask, &QPushButton::clicked, this, &MainWindow::onAddTask);
    connect(ui->btnDeleteTasks, &QPushButton::clicked, this, &MainWindow::onDeleteTasks);
    connect(ui->btnCancelDelete, &QPushButton::clicked, this, &MainWindow::onCancelDeleteMode);
    connect(ui->btnAiAnalyze, &QPushButton::clicked, this, &MainWindow::onAiAnalyze);
    connect(ui->btnResetQuadrants, &QPushButton::clicked, this, &MainWindow::onResetQuadrants);
    connect(ui->btnLlmSettings, &QPushButton::clicked, this, &MainWindow::onLlmSettings);
    connect(ui->btnHistory, &QPushButton::clicked, this, &MainWindow::onViewHistory);
    connect(ui->btnDailyEval, &QPushButton::clicked, this, &MainWindow::onViewDailyEvaluations);
    connect(ui->btnWeeklyReport, &QPushButton::clicked, this, &MainWindow::onExportWeeklyReport);
    connect(ui->btnSettings, &QPushButton::clicked, this, &MainWindow::onAppSettings);
    connect(m_quadrantBoard, &QuadrantBoard::taskQuadrantChanged, this, &MainWindow::onQuadrantChanged);
    connect(m_quadrantBoard, &QuadrantBoard::taskCompletedToggled, this, &MainWindow::onTaskCompletedToggled);
    connect(m_learning, &BehaviorLearningEngine::weightsUpdated, this, [this](const ScoringWeights &w) {
        m_llmService->setScoringWeights(w);
        updateLearningStatus();
        scheduleReloadTasks();
    });
    connect(m_llmService, &LlmService::analysisStarted, this, &MainWindow::onAnalysisStarted);
    connect(m_llmService, &LlmService::analysisFinished, this, &MainWindow::onAnalysisFinished);
    connect(m_llmService, &LlmService::providerChanged, this, &MainWindow::onProviderChanged);
    connect(m_dailyEvalService, &DailyEvaluationService::evaluationFinished, this,
            &MainWindow::onDailyEvaluationFinished);
    connect(m_weeklyReportService, &WeeklyReportService::generationFinished, this,
            &MainWindow::onWeeklyReportFinished);
    connect(ui->listTop3, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        onTop3Clicked();
        if (item)
            m_learning->recordEvent(BehaviorEventType::AiRecommendationAccepted,
                                    item->data(Qt::UserRole).toLongLong(),
                                    EisenhowerQuadrant::Q2_NotUrgentImportant);
    });
    connect(ui->listTop3, &QListWidget::itemDoubleClicked, this, [this]() { onTop3Clicked(); });

    connect(ui->tableTasks, &QTableView::doubleClicked, this, &MainWindow::onTaskDoubleClicked);
    connect(ui->tableTasks, &QTableView::customContextMenuRequested, this, &MainWindow::onTaskContextMenu);
    connect(m_model, &TaskTableModel::taskCompletedToggled, this, &MainWindow::onTaskCompletedToggled);

    connect(m_hotkeyManager, &WindowsHotkeyManager::quickAddTriggered, this, [this]() {
        showMainWindow();
        onTodayTasks();
    });
    connect(m_hotkeyManager, &WindowsHotkeyManager::top3PopupTriggered, this, &MainWindow::showTop3Popup);
}

void MainWindow::setupTrayIcon()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        AppLogger::warn("UI", QStringLiteral("系统托盘不可用，托盘图标已禁用"));
        return;
    }

    const QIcon appIcon = loadAppIcon();
    applyWindowIcon(this, appIcon);
    if (m_titleBar)
        m_titleBar->setTitleBarIcon(appIcon);
    m_trayIcon->setIcon(appIcon);
    m_trayIcon->setToolTip(tr("ToDoList — AI 智能待办"));

    auto *menu = new QMenu(this);
    menu->addAction(tr("显示主窗口"), this, &MainWindow::showMainWindow);
    m_trayTodayTasksAction = menu->addAction(QString(), this, &MainWindow::onTodayTasks);
    menu->addAction(tr("添加任务"), this, &MainWindow::onAddTask);
    m_trayTop3Action = menu->addAction(QString(), this, &MainWindow::showTop3Popup);
    menu->addSeparator();
    menu->addAction(tr("退出"), this, &MainWindow::quitApplication);
    m_trayIcon->setContextMenu(menu);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayActivated);
    m_trayIcon->show();
    updateTrayHotkeyLabels();
}

void MainWindow::updateTrayHotkeyLabels()
{
    if (m_trayTodayTasksAction)
        m_trayTodayTasksAction->setText(globalHotkeyMenuLabel(tr("今日任务"), AppSettings::todayTasksHotkey()));
    if (m_trayTop3Action)
        m_trayTop3Action->setText(globalHotkeyMenuLabel(tr("Top 3 弹窗"), AppSettings::top3PopupHotkey()));
}

void MainWindow::setupGlobalHotkeys()
{
    if (!m_hotkeysInstalled)
        reloadGlobalHotkeys();
}

void MainWindow::reloadGlobalHotkeys(bool fromSettings)
{
    setAttribute(Qt::WA_NativeWindow);
    const WId nativeId = winId();
    if (!nativeId)
        return;

    m_hotkeyManager->uninstall();
    const bool ok = m_hotkeyManager->install(nativeId, AppSettings::todayTasksHotkey(),
                                             AppSettings::top3PopupHotkey());
    m_hotkeysInstalled = ok;
    updateTrayHotkeyLabels();

    if (!ok && !m_hotkeyManager->lastError().isEmpty())
        statusBar()->showMessage(tr("全局快捷键未生效：%1").arg(m_hotkeyManager->lastError()), 8000);
    else if (fromSettings && ok)
        statusBar()->showMessage(tr("快捷键已更新"), 3000);
}

ScoringWeights MainWindow::currentWeights() const
{
    return m_learning->currentWeights();
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    static bool initialFitDone = false;
    if (!initialFitDone) {
        initialFitDone = true;
        WindowFit::fitMainWindow(this);
        updateToolbarScroll();
    }
    setupGlobalHotkeys();
    if (m_sessionMonitor)
        m_sessionMonitor->registerWindow(winId());
    AppTheme::applyDarkTitleBar(this);
    if (m_titleBar)
        m_titleBar->updateMaximizeButton(isMaximized());

    if (m_startupLockPending) {
        m_startupLockPending = false;
        lockApp(true);
    }

    if (ui->listTop3->count() == 0)
        restoreSavedTop3();

    const QIcon appIcon = loadAppIcon();
    applyWindowIcon(this, appIcon);
    if (m_titleBar)
        m_titleBar->setTitleBarIcon(appIcon);
    if (m_trayIcon)
        m_trayIcon->setIcon(appIcon);
}

void MainWindow::minimizeToTray()
{
    saveCurrentTop3();
    archiveTodaySnapshot();
    hide();
    if (m_trayIcon->isVisible())
        m_trayIcon->showMessage(tr("ToDoList"), tr("程序已最小化到系统托盘"), QSystemTrayIcon::Information, 2000);
}

void MainWindow::onCloseRequested()
{
    QMessageBox box(this);
    box.setWindowTitle(tr("关闭"));
    box.setText(tr("请选择关闭方式："));
    box.setIcon(QMessageBox::Question);

    QPushButton *btnTray = nullptr;
    if (m_trayIcon->isVisible()) {
        btnTray = box.addButton(tr("最小化到系统托盘"), QMessageBox::AcceptRole);
        box.setDefaultButton(btnTray);
    }
    QPushButton *btnQuit = box.addButton(tr("退出"), QMessageBox::DestructiveRole);
    QPushButton *btnCancel = box.addButton(tr("取消"), QMessageBox::RejectRole);
    if (!btnTray)
        box.setDefaultButton(btnQuit);
    box.setEscapeButton(btnCancel);

    AppTheme::styleDialog(&box);
    box.exec();

    if (box.clickedButton() == btnCancel)
        return;
    if (btnTray && box.clickedButton() == btnTray) {
        minimizeToTray();
        return;
    }
    if (box.clickedButton() == btnQuit) {
        saveCurrentTop3();
        archiveTodaySnapshot();
        quitApplication();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (AppSettings::minimizeToTray() && m_trayIcon->isVisible()) {
        event->ignore();
        minimizeToTray();
        return;
    }
    saveCurrentTop3();
    archiveTodaySnapshot();
    QMainWindow::closeEvent(event);
}

void MainWindow::showMainWindow()
{
    show();
    raise();
    activateWindow();
    if (m_locked) {
        ui->centralwidget->setVisible(false);
        if (m_lockScreen) {
            m_lockScreen->setGeometry(rect());
            m_lockScreen->raise();
            m_lockScreen->setFocus();
        }
    }
}

void MainWindow::requestShowFromAnotherInstance()
{
    showMainWindow();
}

void MainWindow::showTop3Popup()
{
    PriorityAnalysisResult result = m_lastAnalysis;
    if (result.top3.isEmpty()) {
        result.top3 = PriorityEngine::top3FromRules(
            PriorityEngine::applyRuleScores(m_repo->activeTasks(), currentWeights()), currentWeights());
        result.success = true;
        result.usedLlm = false;
    }

    if (result.top3.isEmpty()) {
        m_trayIcon->showMessage(tr("Top 3"), tr("暂无待办任务"), QSystemTrayIcon::Information, 3000);
        return;
    }

    Top3PopupDialog dlg(nullptr);
    dlg.setRecommendations(result.top3);
    dlg.move(QCursor::pos());
    dlg.show();
    dlg.raise();
    dlg.activateWindow();
    dlg.exec();
}

void MainWindow::quitApplication()
{
    m_hotkeyManager->uninstall();
    m_trayIcon->hide();
    qApp->quit();
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
        showMainWindow();
}

void MainWindow::scheduleReloadTasks()
{
    if (m_reloadTasksScheduled)
        return;
    m_reloadTasksScheduled = true;
    QTimer::singleShot(0, this, [this]() {
        m_reloadTasksScheduled = false;
        reloadTasks();
    });
}

void MainWindow::reloadTasks()
{
    if (m_model->deleteMode())
        exitDeleteMode();

    m_repo->archiveOverdueTasks();
    const QVector<TaskItem> scored = PriorityEngine::applyRuleScores(m_repo->activeTasks(), currentWeights());
    m_model->setTasks(scored);
    m_quadrantBoard->setTasks(scored);
}

void MainWindow::archiveTodaySnapshot()
{
    QString err;
    TaskArchive::exportDailySnapshot(m_repo->activeTasks(), QDate::currentDate(), &err);
}

void MainWindow::onTaskDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid() || index.column() != TaskTableModel::ColTitle || m_model->deleteMode())
        return;

    TaskItem task = m_model->taskAt(index.row());
    if (task.id <= 0)
        return;

    AddTaskDialog dlg(this);
    dlg.setEditTask(task);
    if (dlg.exec() != QDialog::Accepted)
        return;

    TaskItem updated = dlg.taskItem();
    updated.id = task.id;
    updated.completed = task.completed;
    updated.aiRank = task.aiRank;
    updated.aiReason = task.aiReason;
    updated.externalSource = task.externalSource;
    updated.externalId = task.externalId;
    updated.createdAt = task.createdAt;

    QString err;
    if (!m_repo->updateTask(updated, &err))
        QMessageBox::warning(this, tr("编辑任务"), err);
    else
        archiveTodaySnapshot();
}

void MainWindow::onTaskCompletedToggled(qint64 taskId, bool completed)
{
    EisenhowerQuadrant quadrant = EisenhowerQuadrant::Unassigned;
    const int row = m_model->rowForTaskId(taskId);
    if (row >= 0)
        quadrant = m_model->taskAt(row).quadrant;

    QString err;
    if (!m_repo->updateTaskCompleted(taskId, completed, &err)) {
        QMessageBox::warning(this, tr("完成状态"), err);
        scheduleReloadTasks();
        return;
    }
    if (completed && isQuadrantAssigned(quadrant))
        m_learning->recordEvent(BehaviorEventType::TaskCompleted, taskId, quadrant);
    statusBar()->showMessage(completed ? tr("任务已完成") : tr("任务标记为未完成"), 2000);
}

void MainWindow::onViewHistory()
{
    TaskHistoryDialog dlg(this);
    dlg.exec();
}

void MainWindow::onViewDailyEvaluations()
{
    DailyEvaluationDialog dlg(m_repo, m_dailyEvalService, this);
    dlg.exec();
}

void MainWindow::onExportWeeklyReport()
{
    WeeklyReportDialog dlg(m_repo, m_llmService, m_weeklyReportService, this);
    dlg.exec();
}

void MainWindow::onWeeklyReportFinished(const WeeklyReportRecord &record, const WeeklyReportResult &result)
{
    if (!result.success) {
        const QString msg = result.errorMessage.isEmpty() ? tr("周报生成失败") : result.errorMessage;
        statusBar()->showMessage(msg, 10000);
        if (m_trayIcon->isVisible())
            m_trayIcon->showMessage(tr("生成周报"), msg, QSystemTrayIcon::Warning, 5000);
        return;
    }

    const QString week = QStringLiteral("%1 ~ %2")
                             .arg(record.weekStart.toString(QStringLiteral("yyyy-MM-dd")),
                                  record.weekEnd.toString(QStringLiteral("yyyy-MM-dd")));
    QString msg = tr("周报已生成（%1）").arg(week);
    if (!result.usedLlm && !result.errorMessage.isEmpty())
        msg += QStringLiteral(" — ") + tr("已使用规则模板");
    statusBar()->showMessage(msg, 10000);
    if (m_trayIcon->isVisible())
        m_trayIcon->showMessage(tr("生成周报"), msg, QSystemTrayIcon::Information, 5000);
}

void MainWindow::onDailyEvaluationFinished(const QDate &date, const DailyEvaluation &result)
{
    const QString msg = result.summary.isEmpty()
        ? tr("%1 每日评估已完成").arg(date.toString(QStringLiteral("yyyy-MM-dd")))
        : result.summary;
    statusBar()->showMessage(msg, 8000);
    if (m_trayIcon->isVisible())
        m_trayIcon->showMessage(tr("每日评估"), msg, QSystemTrayIcon::Information, 5000);
}

void MainWindow::enterDeleteMode()
{
    m_model->setDeleteMode(true);
    ui->tableTasks->setColumnHidden(TaskTableModel::ColSelect, false);
    ui->btnDeleteTasks->setText(tr("确认删除"));
    ui->btnCancelDelete->setVisible(true);
    statusBar()->showMessage(tr("请勾选要删除的任务，然后点击「确认删除」"), 8000);
}

void MainWindow::exitDeleteMode()
{
    m_model->setDeleteMode(false);
    ui->tableTasks->setColumnHidden(TaskTableModel::ColSelect, true);
    ui->btnDeleteTasks->setText(tr("删除任务"));
    ui->btnCancelDelete->setVisible(false);
}

void MainWindow::onDeleteTasks()
{
    if (!m_model->deleteMode()) {
        enterDeleteMode();
        return;
    }

    const QVector<qint64> ids = m_model->selectedTaskIdsForDelete();
    if (ids.isEmpty()) {
        QMessageBox::information(this, tr("删除任务"), tr("请先勾选要删除的任务"));
        return;
    }

    const int ret = QMessageBox::question(this, tr("确认删除"),
                                          tr("确定删除选中的 %1 条任务吗？此操作不可撤销。").arg(ids.size()),
                                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;

    deleteTasksByIds(ids);
    exitDeleteMode();
}

void MainWindow::onCancelDeleteMode()
{
    exitDeleteMode();
    statusBar()->showMessage(tr("已取消删除"), 2000);
}

void MainWindow::onTaskContextMenu(const QPoint &pos)
{
    const QModelIndex index = ui->tableTasks->indexAt(pos);
    if (!index.isValid())
        return;

    const TaskItem task = m_model->taskAt(index.row());
    if (task.id <= 0)
        return;

    QMenu menu(this);
    QAction *editAction = menu.addAction(tr("编辑任务"));
    QAction *deleteAction = menu.addAction(tr("删除此任务"));
    const QAction *chosen = menu.exec(ui->tableTasks->viewport()->mapToGlobal(pos));
    if (!chosen)
        return;

    if (chosen == editAction) {
        onTaskDoubleClicked(m_model->index(index.row(), TaskTableModel::ColTitle));
    } else if (chosen == deleteAction) {
        const int ret = QMessageBox::question(this, tr("确认删除"),
                                              tr("确定删除任务「%1」吗？").arg(task.title),
                                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::Yes)
            deleteTasksByIds({task.id});
    }
}

void MainWindow::deleteTasksByIds(const QVector<qint64> &ids)
{
    AppLogger::info("UI", QStringLiteral("删除任务 ids=%1").arg(ids.size()));
    QString err;
    int deleted = 0;
    for (qint64 id : ids) {
        if (m_repo->deleteTask(id, &err))
            deleted++;
    }
    if (!err.isEmpty() && deleted == 0)
        QMessageBox::warning(this, tr("删除任务"), err);
    else {
        statusBar()->showMessage(tr("已删除 %1 条任务").arg(deleted), 3000);
        archiveTodaySnapshot();
    }
}

void MainWindow::onQuadrantChanged(qint64 taskId, EisenhowerQuadrant /*from*/, EisenhowerQuadrant to)
{
    QString err;
    if (!m_repo->updateTaskQuadrant(taskId, to, &err)) {
        QMessageBox::warning(this, tr("象限"), err);
        return;
    }
    m_learning->recordEvent(BehaviorEventType::QuadrantChanged, taskId, to);
    statusBar()->showMessage(tr("任务已移至 Q%1").arg(static_cast<int>(to)), 2000);
}

void MainWindow::onAddTask()
{
    AddTaskDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    TaskItem task = dlg.taskItem();
    QString err;
    if (!m_repo->addTask(&task, &err))
        QMessageBox::warning(this, tr("添加任务"), err);
    else {
        reloadTasks();
        archiveTodaySnapshot();
    }
}

void MainWindow::onTodayTasks()
{
    TodayTasksDialog dlg(m_repo, this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    const QStringList titles = dlg.newTaskTitles();
    if (titles.isEmpty()) {
        statusBar()->showMessage(tr("没有新增任务（已有任务已保留）"), 3000);
        return;
    }

    const QDateTime dueToday(QDate::currentDate(), QTime(23, 59));
    int added = 0;
    for (const QString &title : titles) {
        TaskItem task;
        task.title = title;
        task.dueAt = dueToday;
        task.quadrant = EisenhowerQuadrant::Unassigned;
        QString err;
        if (m_repo->addTask(&task, &err))
            ++added;
        else
            QMessageBox::warning(this, tr("今日任务"), err);
    }

    if (added > 0) {
        reloadTasks();
        archiveTodaySnapshot();
        statusBar()->showMessage(tr("已追加 %1 条今日任务").arg(added), 5000);
    }
}

void MainWindow::onAiAnalyze()
{
    m_llmService->setScoringWeights(currentWeights());
    const LlmConfig cfg = m_llmService->config();
    const QString currentKey = TaskArchive::top3ModelKey(cfg);
    const SavedDailyTop3 saved = TaskArchive::loadDailyTop3(QDate::currentDate());
    if (saved.valid) {
        const QString savedKey = QStringLiteral("%1|%2")
                                     .arg(static_cast<int>(saved.provider))
                                     .arg(saved.model.trimmed());
        if (savedKey != currentKey) {
            clearTop3Ui();
            m_lastAnalysis.top3.clear();
            m_loadedTop3ModelKey.clear();
        }
    }

    const QVector<TaskItem> tasks = m_repo->activeTasks();
    if (tasks.isEmpty()) {
        QMessageBox::information(this, tr("AI 分析"), tr("请先添加任务"));
        return;
    }
    AppLogger::info("UI", QStringLiteral("用户触发 AI 分析，任务数 %1").arg(tasks.size()));
    m_llmService->analyzePriorities(tasks);
}

void MainWindow::onViewAiTrace()
{
    if (m_lastAnalysis.trace.isEmpty() && m_lastAnalysis.quadrantAssignments.isEmpty()
        && m_lastAnalysis.top3.isEmpty()) {
        QMessageBox::information(this, tr("AI 分析过程"),
                                 tr("暂无分析记录。请先点击「AI 分析优先级」执行一次分析。"));
        return;
    }
    AiAnalysisTraceDialog dlg(m_lastAnalysis, this);
    dlg.exec();
}

void MainWindow::onResetQuadrants()
{
    if (m_repo->allTasks().isEmpty()) {
        QMessageBox::information(this, tr("重置象限"), tr("当前没有任务"));
        return;
    }

    const int ret = QMessageBox::question(
        this, tr("重置象限"),
        tr("确定将所有任务的象限重置为「不确定 (由 AI 划分)」吗？\n"
           "重置后需再次执行 AI 分析才会重新划分象限。"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;

    if (m_model->deleteMode())
        exitDeleteMode();

    int count = 0;
    QString err;
    if (!m_repo->resetAllQuadrants(&count, &err)) {
        QMessageBox::warning(this, tr("重置象限"), err);
        return;
    }

    AppLogger::info("UI", QStringLiteral("用户重置所有象限为不确定，影响 %1 条").arg(count));
    reloadTasks();
    archiveTodaySnapshot();
    statusBar()->showMessage(tr("已重置 %1 条任务象限为「不确定」").arg(count), 5000);
}

void MainWindow::onLlmSettings()
{
    LlmSettingsDialog dlg(m_llmService, this);
    dlg.exec();
    m_dailyEvalService->setLlmConfig(m_llmService->config());
    m_weeklyReportService->setLlmConfig(m_llmService->config());
    updateProviderStatus();
}

void MainWindow::onM365Settings()
{
    M365SettingsDialog dlg(m_m365Auth, m_m365Sync, m_repo, this);
    dlg.exec();
}

void MainWindow::onAnalysisStarted()
{
    ui->btnAiAnalyze->setEnabled(false);
    if (m_aiBusyOverlay)
        m_aiBusyOverlay->showAnimated();
    statusBar()->showMessage(tr("AI 分析中…"));
}

void MainWindow::applyTop3ToUi(const PriorityAnalysisResult &result)
{
    ui->listTop3->clear();
    ui->textAiReason->clear();

    for (const PriorityRecommendation &rec : result.top3) {
        auto *item = new QListWidgetItem(
            tr("#%1 %2 (%3)").arg(rec.rank).arg(rec.title).arg(rec.score, 0, 'f', 1));
        item->setData(Qt::UserRole, rec.taskId);
        item->setData(Qt::UserRole + 1, rec.reason);
        item->setToolTip(rec.reason.isEmpty() ? tr("点击或双击查看推荐理由") : rec.reason);
        ui->listTop3->addItem(item);
    }

    if (!result.top3.isEmpty()) {
        ui->listTop3->setCurrentRow(0);
        onTop3Clicked();
    }
}

void MainWindow::clearTop3Ui()
{
    ui->listTop3->clear();
    ui->textAiReason->clear();
}

void MainWindow::restoreSavedTop3()
{
    if (!m_repo || !m_repo->isOpen())
        return;

    const SavedDailyTop3 saved = TaskArchive::loadDailyTop3(QDate::currentDate());
    if (!saved.valid) {
        AppLogger::debug("UI", QStringLiteral("今日无已保存 Top3，跳过恢复"));
        return;
    }

    const QVector<TaskItem> scored =
        PriorityEngine::applyRuleScores(m_repo->activeTasks(), currentWeights());
    const QVector<PriorityRecommendation> top3 =
        TaskArchive::hydrateTop3Recommendations(saved.top3, scored);
    if (top3.isEmpty()) {
        AppLogger::info("UI", QStringLiteral("今日 Top3 对应任务已不存在，跳过恢复"));
        return;
    }

    const QString savedKey = QStringLiteral("%1|%2")
                                 .arg(static_cast<int>(saved.provider))
                                 .arg(saved.model.trimmed());
    const QString currentKey = TaskArchive::top3ModelKey(m_llmService->config());
    const bool modelMismatch = savedKey != currentKey;

    m_lastAnalysis.top3 = top3;
    m_lastAnalysis.trace = saved.trace;
    m_lastAnalysis.usedLlm = saved.usedLlm;
    m_lastAnalysis.success = true;
    m_loadedTop3ModelKey = savedKey;

    PriorityAnalysisResult uiResult;
    uiResult.top3 = top3;
    applyTop3ToUi(uiResult);

    QString msg = tr("已恢复今日 Top3 推荐 (%1 条)").arg(top3.size());
    if (modelMismatch)
        msg += tr("（保存于 %1）").arg(saved.model);
    AppLogger::info("UI", msg);
    statusBar()->showMessage(msg, 5000);
}

void MainWindow::saveCurrentTop3()
{
    if (m_lastAnalysis.top3.isEmpty())
        return;
    QString err;
    TaskArchive::saveDailyTop3(QDate::currentDate(), m_llmService->config(), m_lastAnalysis, &err);
    if (!err.isEmpty())
        AppLogger::warn("UI", QStringLiteral("保存 Top3 失败: %1").arg(err));
    else
        m_loadedTop3ModelKey = TaskArchive::top3ModelKey(m_llmService->config());
}

void MainWindow::onAnalysisFinished(const PriorityAnalysisResult &result)
{
    ui->btnAiAnalyze->setEnabled(true);
    if (m_aiBusyOverlay)
        m_aiBusyOverlay->hideAnimated();
    m_lastAnalysis = result;

    for (const QuadrantAssignment &a : result.quadrantAssignments) {
        QString err;
        if (!m_repo->updateTaskQuadrant(a.taskId, a.quadrant, &err))
            AppLogger::error("UI", QStringLiteral("保存象限失败 id=%1: %2").arg(a.taskId).arg(err));
    }

    applyTop3ToUi(result);
    saveCurrentTop3();
    reloadTasks();
    archiveTodaySnapshot();

    QString msg = result.usedLlm ? tr("AI 分析完成") : tr("已使用规则层 Top 3");
    if (!result.quadrantAssignments.isEmpty()) {
        msg += tr("，已划分 %1 个待分配象限").arg(result.quadrantAssignments.size());
        msg += result.usedLlmForQuadrants ? tr("（AI 划分）") : tr("（规则推断）");
    }
    if (!result.quadrantClassifyMessage.isEmpty())
        msg += QStringLiteral(" — ") + result.quadrantClassifyMessage;
    if (!result.errorMessage.isEmpty())
        msg += QStringLiteral(" — ") + result.errorMessage;

    AppLogger::info("UI", msg);
    statusBar()->showMessage(msg, 10000);

    if (!result.usedLlm && !result.errorMessage.isEmpty())
        m_trayIcon->showMessage(tr("AI 分析"), msg, QSystemTrayIcon::Warning, 5000);
}

void MainWindow::onProviderChanged(LlmProviderType type)
{
    m_dailyEvalService->setLlmConfig(m_llmService->config());
    m_weeklyReportService->setLlmConfig(m_llmService->config());
    updateProviderStatus();
    statusBar()->showMessage(tr("已切换 Provider: %1").arg(providerDisplayName(type)), 3000);
}

void MainWindow::onTop3Clicked()
{
    const QListWidgetItem *item = ui->listTop3->currentItem();
    if (!item)
        return;
    const QString reason = item->data(Qt::UserRole + 1).toString();
    ui->textAiReason->setPlainText(reason.isEmpty()
                                       ? tr("选中: %1").arg(item->text())
                                       : reason);
}

void MainWindow::updateProviderStatus()
{
    const LlmConfig cfg = m_llmService->config();
    ui->labelProviderStatus->setText(tr("Provider: %1 | Model: %2")
                                         .arg(providerDisplayName(cfg.provider), cfg.model));
}

void MainWindow::updateLearningStatus()
{
    const ScoringWeights w = currentWeights();
    statusBar()->showMessage(
        tr("学习权重 U:%1 I:%2 B:%3 E:%4")
            .arg(w.urgency, 0, 'f', 2)
            .arg(w.importance, 0, 'f', 2)
            .arg(w.business, 0, 'f', 2)
            .arg(w.effort, 0, 'f', 2),
        4000);
}

QString MainWindow::providerDisplayName(LlmProviderType type) const
{
    switch (type) {
    case LlmProviderType::Ollama: return tr("Ollama/Qwen");
    case LlmProviderType::DeepSeek: return tr("DeepSeek");
    case LlmProviderType::Kimi: return tr("Kimi");
    case LlmProviderType::CustomOpenAI: return tr("Custom");
    }
    return tr("Unknown");
}
