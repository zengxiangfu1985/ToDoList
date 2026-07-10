#include "taskhistorydialog.h"

#include "../core/task_archive.h"
#include "../core/task_types.h"
#include "../models/task_table_model.h"
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

TaskHistoryDialog::TaskHistoryDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("历史记录"));
    resize(900, 520);
    AppTheme::styleDialog(this);
    setWindowFlag(Qt::WindowMaximizeButtonHint, true);

    auto *root = new QVBoxLayout(this);
    root->addWidget(new QLabel(tr("按日期查看任务快照与已过期任务（含截止超时自动归档的任务）")));

    auto *split = new QHBoxLayout;
    auto *dateList = new QListWidget(this);
    dateList->setMinimumWidth(160);

    m_table = new QTableView(this);
    auto *model = new TaskTableModel(m_table);
    m_table->setModel(model);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setWordWrap(false);
    m_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_table->setColumnHidden(TaskTableModel::ColSelect, true);
    m_table->setColumnHidden(TaskTableModel::ColCompleted, true);

    QHeaderView *header = m_table->horizontalHeader();
    header->setStretchLastSection(false);
    header->setDefaultAlignment(Qt::AlignCenter);
    header->setMinimumSectionSize(32);
    header->setSectionResizeMode(TaskTableModel::ColIndex, QHeaderView::Fixed);
    header->setSectionResizeMode(TaskTableModel::ColTitle, QHeaderView::Fixed);
    header->setSectionResizeMode(TaskTableModel::ColDue, QHeaderView::Fixed);
    header->setSectionResizeMode(TaskTableModel::ColQuadrant, QHeaderView::Fixed);
    header->setSectionResizeMode(TaskTableModel::ColScore, QHeaderView::Fixed);

    split->addWidget(dateList);
    split->addWidget(m_table, 1);
    root->addLayout(split, 1);

    auto *btnClose = new QPushButton(tr("关闭"), this);
    root->addWidget(btnClose, 0, Qt::AlignRight);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

    const QStringList dates = TaskArchive::availableHistoryDates();
    if (dates.isEmpty()) {
        dateList->addItem(tr("（暂无历史）"));
    } else {
        for (const QString &d : dates)
            dateList->addItem(d);
        dateList->setCurrentRow(0);
    }

    auto loadDate = [model, dateList](int row) {
        if (row < 0 || row >= dateList->count())
            return;
        const QString text = dateList->item(row)->text();
        if (text.startsWith(QStringLiteral("（")))
            return;
        const QDate date = QDate::fromString(text, QStringLiteral("yyyy-MM-dd"));
        QString err;
        const QVector<TaskItem> tasks = TaskArchive::loadHistoryForDate(date, &err);
        if (!err.isEmpty()) {
            QMessageBox::warning(nullptr, QObject::tr("历史"), err);
            return;
        }
        model->setTasks(tasks);
    };

    connect(dateList, &QListWidget::currentRowChanged, this, loadDate);
    if (!dates.isEmpty())
        loadDate(0);
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
    const int fixedTotal = indexWidth + quadrantWidth + scoreWidth;
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
}
