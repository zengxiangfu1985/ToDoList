#ifndef TASKHISTORYDIALOG_H
#define TASKHISTORYDIALOG_H

#include <QDate>
#include <QDialog>

class QListWidget;
class QTableView;
class TaskRepository;
class TaskTableModel;

class TaskHistoryDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TaskHistoryDialog(TaskRepository *repo, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void loadDate(int row);
    void onCompletionDoubleClicked(const QModelIndex &index);
    void updateTableColumnWidths();

    TaskRepository *m_repo = nullptr;
    QListWidget *m_dateList = nullptr;
    QTableView *m_table = nullptr;
    TaskTableModel *m_model = nullptr;
};

#endif // TASKHISTORYDIALOG_H
