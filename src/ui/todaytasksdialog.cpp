#include "todaytasksdialog.h"

#include "../core/task_repository.h"
#include "../utils/app_theme.h"

#include <QDate>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSet>
#include <QVBoxLayout>

TodayTasksDialog::TodayTasksDialog(TaskRepository *repo, QWidget *parent)
    : QDialog(parent)
    , m_repo(repo)
{
    setWindowTitle(tr("今日任务"));
    setMinimumSize(480, 360);
    AppTheme::styleDialog(this);

    auto *root = new QVBoxLayout(this);
    root->addWidget(new QLabel(
        tr("每行一条任务（截止今天 23:59，象限由 AI 划分）。"
           "已有任务会保留在列表中，本次仅追加新行。")));

    m_editTasks = new QPlainTextEdit(this);
    m_editTasks->setPlaceholderText(tr("例如：\n完成项目报告\n回复客户邮件\n健身 30 分钟"));
    root->addWidget(m_editTasks, 1);

    auto *btnImport = new QPushButton(tr("导入昨日未完成任务"), this);
    root->addWidget(btnImport);

    auto *btnRow = new QHBoxLayout;
    auto *btnOk = new QPushButton(tr("添加"), this);
    auto *btnCancel = new QPushButton(tr("取消"), this);
    btnRow->addStretch();
    btnRow->addWidget(btnOk);
    btnRow->addWidget(btnCancel);
    root->addLayout(btnRow);

    connect(btnImport, &QPushButton::clicked, this, &TodayTasksDialog::onImportYesterday);
    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    preloadExistingTodayTasks();

    if (m_repo && m_existingTitles.isEmpty()) {
        const QVector<TaskItem> unfinished = m_repo->yesterdayUnfinishedTasks();
        if (!unfinished.isEmpty()) {
            const auto reply = QMessageBox::question(
                this, tr("导入昨日任务"),
                tr("检测到昨日有 %1 条未完成任务，是否导入到今日任务列表？").arg(unfinished.size()),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (reply == QMessageBox::Yes)
                onImportYesterday();
        }
    }
}

void TodayTasksDialog::preloadExistingTodayTasks()
{
    m_existingTitles.clear();
    if (!m_repo)
        return;

    QStringList lines;
    for (const TaskItem &t : m_repo->activeTasksDueOnDate(QDate::currentDate())) {
        m_existingTitles.insert(t.title);
        lines.append(t.title);
    }
    if (!lines.isEmpty())
        m_editTasks->setPlainText(lines.join(QChar('\n')));
}

QStringList TodayTasksDialog::taskTitles() const
{
    QStringList lines;
    const QString text = m_editTasks->toPlainText();
    for (const QString &line : text.split(QChar('\n'))) {
        const QString trimmed = line.trimmed();
        if (!trimmed.isEmpty())
            lines.append(trimmed);
    }
    return lines;
}

QStringList TodayTasksDialog::newTaskTitles() const
{
    QStringList out;
    for (const QString &title : taskTitles()) {
        if (!m_existingTitles.contains(title))
            out.append(title);
    }
    return out;
}

void TodayTasksDialog::onImportYesterday()
{
    if (!m_repo)
        return;

    const QVector<TaskItem> unfinished = m_repo->yesterdayUnfinishedTasks();
    if (unfinished.isEmpty()) {
        QMessageBox::information(this, tr("导入"), tr("没有可导入的昨日未完成任务。"));
        return;
    }

    QStringList existing = taskTitles();
    QSet<QString> seen;
    for (const QString &s : existing)
        seen.insert(s);

    int added = 0;
    for (const TaskItem &t : unfinished) {
        if (seen.contains(t.title))
            continue;
        existing.append(t.title);
        seen.insert(t.title);
        ++added;
    }

    m_editTasks->setPlainText(existing.join(QChar('\n')));
    if (added > 0)
        QMessageBox::information(this, tr("导入"), tr("已导入 %1 条昨日未完成任务。").arg(added));
}
