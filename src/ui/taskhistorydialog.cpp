#include "taskhistorydialog.h"

#include "../core/task_archive.h"
#include "../core/task_repository.h"
#include "../core/task_types.h"
#include "../models/task_table_model.h"
#include "../ui/task_table_delegate.h"
#include "../utils/app_theme.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTableView>
#include <QVBoxLayout>

TaskHistoryDialog::TaskHistoryDialog(TaskRepository *repo, QWidget *parent)
    : QDialog(parent)
    , m_repo(repo)
{
    setWindowTitle(tr("历史记录"));
    resize(900, 520);
    AppTheme::styleDialog(this);
    setWindowFlag(Qt::WindowMaximizeButtonHint, true);

    auto *root = new QVBoxLayout(this);
    root->addWidget(new QLabel(tr("按日期查看任务快照与已过期任务（含截止超时自动归档的任务）")));
    root->addWidget(new QLabel(tr("双击「完成」列中的「已完成」/「未完成」可切换状态，修改后全局生效")));

    auto *split = new QHBoxLayout;
    m_dateList = new QListWidget(this);
    m_dateList->setMinimumWidth(160);

    m_table = new QTableView(this);
    m_model = new TaskTableModel(m_table);
    m_model->setReadOnly(true);
    m_table->setModel(m_model);
    m_table->setItemDelegate(new TaskTableDelegate(m_model, m_table));
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setWordWrap(false);
    m_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_table->setColumnHidden(TaskTableModel::ColSelect, true);

    QHeaderView *header = m_table->horizontalHeader();
    header->setStretchLastSection(false);
    header->setDefaultAlignment(Qt::AlignCenter);
    header->setMinimumSectionSize(32);
    header->setSectionResizeMode(TaskTableModel::ColIndex, QHeaderView::Fixed);
    header->setSectionResizeMode(TaskTableModel::ColTitle, QHeaderView::Fixed);
    header->setSectionResizeMode(TaskTableModel::ColDue, QHeaderView::Fixed);
    header->setSectionResizeMode(TaskTableModel::ColQuadrant, QHeaderView::Fixed);
    header->setSectionResizeMode(TaskTableModel::ColScore, QHeaderView::Fixed);
    header->setSectionResizeMode(TaskTableModel::ColCompleted, QHeaderView::Fixed);

    split->addWidget(m_dateList);
    split->addWidget(m_table, 1);
    root->addLayout(split, 1);

    auto *btnClose = new QPushButton(tr("关闭"), this);
    root->addWidget(btnClose, 0, Qt::AlignRight);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

    const QStringList dates = TaskArchive::availableHistoryDates();
    if (dates.isEmpty()) {
        m_dateList->addItem(tr("（暂无历史）"));
    } else {
        for (const QString &d : dates)
            m_dateList->addItem(d);
        m_dateList->setCurrentRow(0);
    }

    connect(m_dateList, &QListWidget::currentRowChanged, this, &TaskHistoryDialog::loadDate);
    connect(m_table, &QTableView::doubleClicked, this, &TaskHistoryDialog::onCompletionDoubleClicked);

    if (!dates.isEmpty())
        loadDate(0);
}

void TaskHistoryDialog::loadDate(int row)
{
    if (!m_model || !m_dateList)
        return;
    if (row < 0 || row >= m_dateList->count())
        return;

    const QString text = m_dateList->item(row)->text();
    if (text.startsWith(QStringLiteral("（")))
        return;

    const QDate date = QDate::fromString(text, QStringLiteral("yyyy-MM-dd"));
    QString err;
    const QVector<TaskItem> tasks = TaskArchive::loadHistoryForDate(date, &err);
    if (!err.isEmpty()) {
        QMessageBox::warning(this, tr("历史"), err);
        return;
    }
    m_model->setTasks(tasks);
}

void TaskHistoryDialog::onCompletionDoubleClicked(const QModelIndex &index)
{
    if (!m_repo || !m_model || !index.isValid() || index.column() != TaskTableModel::ColCompleted)
        return;

    const TaskItem task = m_model->taskAt(index.row());
    if (task.id <= 0)
        return;

    const bool newCompleted = !task.completed;
    QString err;
    if (!m_repo->applyTaskCompletionGlobally(task.id, newCompleted, &err)) {
        QMessageBox::warning(this, tr("完成状态"), err);
        loadDate(m_dateList->currentRow());
        return;
    }

    const QDateTime completedAt = newCompleted ? QDateTime::currentDateTimeUtc() : QDateTime();
    m_model->updateLocalCompletion(index.row(), newCompleted, completedAt);
}

void TaskHistoryDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateTableColumnWidths();
}

void TaskHistoryDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    updateTableColumnWidths();
}

void TaskHistoryDialog::updateTableColumnWidths()
{
    if (!m_table)
        return;

    const int viewportWidth = m_table->viewport()->width();
    if (viewportWidth <= 0)
        return;

    constexpr int indexWidth = 44;
    constexpr int quadrantWidth = 56;
    constexpr int scoreWidth = 56;
    constexpr int completedWidth = 72;
    const int fixedTotal = indexWidth + quadrantWidth + scoreWidth + completedWidth;
    int flexWidth = viewportWidth - fixedTotal;
    if (flexWidth <= 0)
        return;

    int dueWidth = qMax(140, flexWidth * 35 / 100);
    int titleWidth = flexWidth - dueWidth;
    if (titleWidth < 120) {
        titleWidth = 120;
        dueWidth = qMax(120, flexWidth - titleWidth);
    }

    m_table->setColumnWidth(TaskTableModel::ColIndex, indexWidth);
    m_table->setColumnWidth(TaskTableModel::ColTitle, titleWidth);
    m_table->setColumnWidth(TaskTableModel::ColDue, dueWidth);
    m_table->setColumnWidth(TaskTableModel::ColQuadrant, quadrantWidth);
    m_table->setColumnWidth(TaskTableModel::ColScore, scoreWidth);
    m_table->setColumnWidth(TaskTableModel::ColCompleted, completedWidth);
}
