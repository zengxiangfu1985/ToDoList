#ifndef DAILY_EVALUATION_DIALOG_H
#define DAILY_EVALUATION_DIALOG_H

#include "../core/task_types.h"
#include "../core/task_repository.h"

#include <QDialog>
#include <QDate>

class QDateEdit;
class QLabel;
class QPushButton;
class QTableWidget;
class QTextEdit;
class TaskRepository;
class DailyEvaluationService;

class DailyEvaluationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DailyEvaluationDialog(TaskRepository *repo, DailyEvaluationService *evalService,
                                 QWidget *parent = nullptr);

private slots:
    void refreshList();
    void onTableSelectionChanged();
    void onRegenerate();
    void onDelete();
    void onSaveEdit();
    void onEvaluationStarted(const QDate &date);
    void onEvaluationFinished(const QDate &date, const DailyEvaluation &result);

private:
    QDate selectedDate() const;
    DailyEvaluation currentEvaluation() const;
    void loadEvaluationIntoDetail(const DailyEvaluation &eval);
    void setBusy(bool busy);
    static QString providerLabel(LlmProviderType type);
    static QString modelDisplayText(const DailyEvaluation &eval);
    static QString focusStatsSummary(const FocusDayStats &stats);
    static QString evalStatsSummary(const DailyEvaluation &eval, const FocusDayStats &focusStats);

    TaskRepository *m_repo = nullptr;
    DailyEvaluationService *m_evalService = nullptr;

    QDateEdit *m_dateEdit = nullptr;
    QTableWidget *m_table = nullptr;
    QLabel *m_statsLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    QTextEdit *m_summaryEdit = nullptr;
    QTextEdit *m_planReviewEdit = nullptr;
    QTextEdit *m_feedbackEdit = nullptr;
    QPushButton *m_btnRegenerate = nullptr;
    QPushButton *m_btnDelete = nullptr;
    QPushButton *m_btnSave = nullptr;
};

#endif // DAILY_EVALUATION_DIALOG_H
