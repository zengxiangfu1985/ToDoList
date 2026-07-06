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

    auto *table = new QTableView(this);
    auto *model = new TaskTableModel(table);
    table->setModel(model);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    split->addWidget(dateList);
    split->addWidget(table, 1);
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
