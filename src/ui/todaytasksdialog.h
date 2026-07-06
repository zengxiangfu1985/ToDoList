#ifndef TODAYTASKSDIALOG_H
#define TODAYTASKSDIALOG_H

#include <QDialog>
#include <QSet>
#include <QStringList>

class QPlainTextEdit;
class TaskRepository;

class TodayTasksDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TodayTasksDialog(TaskRepository *repo, QWidget *parent = nullptr);

    QStringList taskTitles() const;
    QStringList newTaskTitles() const;

private slots:
    void onImportYesterday();

private:
    void preloadExistingTodayTasks();

    TaskRepository *m_repo = nullptr;
    QPlainTextEdit *m_editTasks = nullptr;
    QSet<QString> m_existingTitles;
};

#endif // TODAYTASKSDIALOG_H
