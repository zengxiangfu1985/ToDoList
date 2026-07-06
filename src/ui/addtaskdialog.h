#ifndef ADDTASKDIALOG_H
#define ADDTASKDIALOG_H

#include "../core/task_types.h"

#include <QDialog>

namespace Ui {
class AddTaskDialog;
}

class AddTaskDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddTaskDialog(QWidget *parent = nullptr);
    ~AddTaskDialog() override;

    TaskItem taskItem() const;
    void setEditTask(const TaskItem &task);

private slots:
    void onAccepted();

private:
    Ui::AddTaskDialog *ui;
    TaskItem m_task;
    bool m_editMode = false;
};

#endif // ADDTASKDIALOG_H
