#include "addtaskdialog.h"
#include "ui_addtaskdialog.h"

#include "../utils/app_theme.h"

#include <QDateTime>
#include <QMessageBox>
#include <QTime>

AddTaskDialog::AddTaskDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddTaskDialog)
{
    ui->setupUi(this);
    AppTheme::styleDialog(this);
    setWindowTitle(tr("添加任务"));
    ui->dateDue->setDate(QDate::currentDate());
    ui->dateDue->setCalendarPopup(true);
    ui->comboQuadrant->addItem(tr("不确定 (由 AI 划分)"), static_cast<int>(EisenhowerQuadrant::Unassigned));
    ui->comboQuadrant->addItem(tr("Q1 重要且紧急"), static_cast<int>(EisenhowerQuadrant::Q1_UrgentImportant));
    ui->comboQuadrant->addItem(tr("Q2 重要不紧急"), static_cast<int>(EisenhowerQuadrant::Q2_NotUrgentImportant));
    ui->comboQuadrant->addItem(tr("Q3 紧急不重要"), static_cast<int>(EisenhowerQuadrant::Q3_UrgentNotImportant));
    ui->comboQuadrant->addItem(tr("Q4 不重要不紧急"), static_cast<int>(EisenhowerQuadrant::Q4_NotUrgentNotImportant));
    ui->comboQuadrant->setCurrentIndex(0);

    connect(ui->btnOk, &QPushButton::clicked, this, &AddTaskDialog::onAccepted);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

AddTaskDialog::~AddTaskDialog()
{
    delete ui;
}

TaskItem AddTaskDialog::taskItem() const
{
    return m_task;
}

void AddTaskDialog::setEditTask(const TaskItem &task)
{
    m_editMode = true;
    m_task = task;
    setWindowTitle(tr("编辑任务"));
    ui->editTitle->setText(task.title);
    ui->dateDue->setDate(task.dueAt.isValid() ? task.dueAt.toLocalTime().date() : QDate::currentDate());

    for (int i = 0; i < ui->comboQuadrant->count(); ++i) {
        if (ui->comboQuadrant->itemData(i).toInt() == static_cast<int>(task.quadrant)) {
            ui->comboQuadrant->setCurrentIndex(i);
            break;
        }
    }
}

void AddTaskDialog::onAccepted()
{
    const QString title = ui->editTitle->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, tr("校验"), tr("标题不能为空"));
        return;
    }
    m_task.title = title;
    m_task.dueAt = QDateTime(ui->dateDue->date(), QTime(23, 59));
    m_task.quadrant = static_cast<EisenhowerQuadrant>(ui->comboQuadrant->currentData().toInt());
    if (!m_editMode)
        m_task.completed = false;
    accept();
}
