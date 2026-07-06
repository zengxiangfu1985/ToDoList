#ifndef WEEKLY_REPORT_DIALOG_H
#define WEEKLY_REPORT_DIALOG_H

#include "../core/task_types.h"
#include "../core/weekly_report_service.h"

#include <QDialog>
#include <QDate>
#include <QSet>

class QLabel;
class QListWidget;
class QPushButton;
class QTableWidget;
class QTextEdit;
class LlmService;
class TaskRepository;
class WeeklyReportService;

class WeeklyReportDialog : public QDialog
{
    Q_OBJECT
public:
    explicit WeeklyReportDialog(TaskRepository *repo, LlmService *llmService,
                              WeeklyReportService *reportService, QWidget *parent = nullptr);

private slots:
    void refreshHistoryList();
    void onHistorySelectionChanged();
    void onSelectAll();
    void onSelectNone();
    void onSelectCompleted();
    void onGenerate();
    void onRunInBackground();
    void onDelete();
    void onSaveEdit();
    void onExport();
    void onGenerationStarted(const QDate &weekStart, const QDate &weekEnd);
    void onGenerationFinished(const WeeklyReportRecord &record, const WeeklyReportResult &result);

private:
    void populateTaskList(const QSet<qint64> &checkedIds = {});
    QVector<qint64> selectedTaskIds() const;
    QVector<TaskItem> selectedTasks() const;
    void loadRecordIntoDetail(const WeeklyReportRecord &record);
    void setBusyUi(bool busy);
    void updateLocalModelHint();
    void startGeneration();
    QString formatTaskListLabel(const TaskItem &t) const;
    static QString providerLabel(LlmProviderType type);
    static QString modelDisplayText(const WeeklyReportRecord &record);
    static QSet<qint64> parseTaskIdsJson(const QString &json);
    WeeklyReportRecord selectedRecord() const;

    TaskRepository *m_repo = nullptr;
    LlmService *m_llmService = nullptr;
    WeeklyReportService *m_reportService = nullptr;
    QDate m_weekStart;
    QDate m_weekEnd;
    qint64 m_selectedRecordId = 0;

    QLabel *m_rangeLabel = nullptr;
    QLabel *m_localHintLabel = nullptr;
    QLabel *m_metaLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    QTableWidget *m_historyTable = nullptr;
    QListWidget *m_taskList = nullptr;
    QTextEdit *m_reportEdit = nullptr;
    QPushButton *m_btnGenerate = nullptr;
    QPushButton *m_btnBackground = nullptr;
    QPushButton *m_btnDelete = nullptr;
    QPushButton *m_btnSave = nullptr;
    QPushButton *m_btnExport = nullptr;
};

#endif // WEEKLY_REPORT_DIALOG_H
