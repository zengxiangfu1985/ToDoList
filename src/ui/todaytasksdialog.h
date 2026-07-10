#ifndef TODAYTASKSDIALOG_H
#define TODAYTASKSDIALOG_H

#include "../core/task_types.h"

#include <QDialog>
#include <QVector>

class QLabel;
class LineNumberPlainTextEdit;
class QPushButton;
class TaskRepository;

class TodayTasksDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TodayTasksDialog(TaskRepository *repo, QWidget *parent = nullptr);

    QStringList newTaskTitles() const;
    QVector<TaskItem> updatedExistingTasks() const;
    QVector<qint64> deletedExistingTaskIds() const;
    bool hasAnyChanges() const;

private slots:
    void onImportYesterday();
    void onTextChanged();
    void refreshAddButtonLabel();

private:
    void preloadExistingTodayTasks();
    void styleTextEdit(LineNumberPlainTextEdit *edit, const char *objectName);

    TaskRepository *m_repo = nullptr;
    LineNumberPlainTextEdit *m_editExisting = nullptr;
    LineNumberPlainTextEdit *m_editNew = nullptr;
    QLabel *m_labelExistingHeader = nullptr;
    QPushButton *m_btnOk = nullptr;
    QVector<TaskItem> m_existingTasks;
};

#endif // TODAYTASKSDIALOG_H
