#ifndef TASKHISTORYDIALOG_H
#define TASKHISTORYDIALOG_H

#include <QDialog>

class QTableView;

class TaskHistoryDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TaskHistoryDialog(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void updateTableColumnWidths();

    QTableView *m_table = nullptr;
};

#endif // TASKHISTORYDIALOG_H
