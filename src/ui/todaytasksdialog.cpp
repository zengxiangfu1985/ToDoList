#include "todaytasksdialog.h"

#include "line_number_plain_text_edit.h"

#include "../core/task_repository.h"
#include "../utils/app_theme.h"

#include <QDate>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSet>
#include <QVBoxLayout>

namespace {

QLabel *makeSectionLabel(const QString &text, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setObjectName(QStringLiteral("todayTasksSectionLabel"));
    QFont f = label->font();
    f.setPixelSize(AppTheme::metrics().fontSmall);
    f.setWeight(QFont::DemiBold);
    label->setFont(f);
    return label;
}

QLabel *makeHintLabel(const QString &text, bool secondary, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setWordWrap(true);
    QFont f = label->font();
    f.setPixelSize(secondary ? AppTheme::metrics().fontSmall : AppTheme::metrics().fontBase);
    label->setFont(f);
    if (secondary) {
        label->setObjectName(QStringLiteral("todayTasksHintSecondary"));
        label->setStyleSheet(QStringLiteral("color: #8a9bb8;"));
    }
    return label;
}

QStringList linesPreservingPositions(const LineNumberPlainTextEdit *edit)
{
    if (!edit)
        return {};

    QStringList lines = edit->toPlainText().split(QChar('\n'));
    while (!lines.isEmpty() && lines.last().trimmed().isEmpty())
        lines.removeLast();
    return lines;
}

QStringList nonEmptyLines(const LineNumberPlainTextEdit *edit)
{
    QStringList lines;
    for (const QString &line : linesPreservingPositions(edit)) {
        const QString trimmed = line.trimmed();
        if (!trimmed.isEmpty())
            lines.append(trimmed);
    }
    return lines;
}

} // namespace

TodayTasksDialog::TodayTasksDialog(TaskRepository *repo, QWidget *parent)
    : QDialog(parent)
    , m_repo(repo)
{
    setWindowTitle(tr("今日任务"));
    resize(600, 480);
    setMinimumSize(520, 400);
    AppTheme::styleDialog(this);

    auto *root = new QVBoxLayout(this);
    root->setSpacing(8);
    root->setContentsMargins(16, 14, 16, 14);

    root->addWidget(makeHintLabel(tr("追加今日新任务（截止今天 23:59，象限由 AI 划分）"), false, this));
    root->addWidget(makeHintLabel(tr("已有任务可编辑或删除行；新增任务请在下方输入，每行一条。"), true, this));

    m_labelExistingHeader = makeSectionLabel(tr("已有任务 (0)"), this);
    root->addWidget(m_labelExistingHeader);

    m_editExisting = new LineNumberPlainTextEdit(this);
    styleTextEdit(m_editExisting, "todayTasksExistingEdit");
    m_editExisting->setPlaceholderText(tr("（暂无今日已有任务）"));
    root->addWidget(m_editExisting, 2);

    root->addWidget(makeSectionLabel(tr("新增任务"), this));

    m_editNew = new LineNumberPlainTextEdit(this);
    styleTextEdit(m_editNew, "todayTasksNewEdit");
    m_editNew->setPlaceholderText(tr("每行一条，回车继续\n例如：完成项目报告\n回复客户邮件"));
    root->addWidget(m_editNew, 1);

    auto *btnImport = new QPushButton(tr("导入昨日未完成"), this);
    btnImport->setObjectName(QStringLiteral("btnSecondary"));

    m_btnOk = new QPushButton(tr("添加"), this);
    m_btnOk->setObjectName(QStringLiteral("btnOk"));
    auto *btnCancel = new QPushButton(tr("取消"), this);
    btnCancel->setObjectName(QStringLiteral("btnCancel"));

    auto *btnRow = new QHBoxLayout;
    btnRow->setSpacing(8);
    btnRow->addWidget(btnImport);
    btnRow->addStretch();
    btnRow->addWidget(m_btnOk);
    btnRow->addWidget(btnCancel);
    root->addLayout(btnRow);

    connect(btnImport, &QPushButton::clicked, this, &TodayTasksDialog::onImportYesterday);
    connect(m_btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_editExisting, &QPlainTextEdit::textChanged, this, &TodayTasksDialog::onTextChanged);
    connect(m_editNew, &QPlainTextEdit::textChanged, this, &TodayTasksDialog::onTextChanged);

    preloadExistingTodayTasks();
    refreshAddButtonLabel();

    if (m_repo && m_existingTasks.isEmpty()) {
        const QVector<TaskItem> unfinished = m_repo->yesterdayUnfinishedTasks();
        if (!unfinished.isEmpty()) {
            const auto reply = QMessageBox::question(
                this, tr("导入昨日任务"),
                tr("检测到昨日有 %1 条未完成任务，是否导入到「新增任务」列表？").arg(unfinished.size()),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (reply == QMessageBox::Yes)
                onImportYesterday();
        }
    }
}

void TodayTasksDialog::styleTextEdit(LineNumberPlainTextEdit *edit, const char *objectName)
{
    edit->setObjectName(QString::fromLatin1(objectName));
    edit->setTabChangesFocus(true);
    edit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    edit->document()->setDefaultStyleSheet(QStringLiteral("body { line-height: 150%; }"));
    edit->setStyleSheet(QStringLiteral(
        "QPlainTextEdit {"
        "  background-color: #0c1424;"
        "  color: #d0d8e8;"
        "  border: 1px solid #1a2a44;"
        "  border-radius: 6px;"
        "  padding: 8px 4px;"
        "  selection-background-color: rgba(0, 112, 192, 0.45);"
        "}"
        "QPlainTextEdit#todayTasksExistingEdit {"
        "  background-color: #0a101c;"
        "  color: #b8c4d8;"
        "}"));
}

void TodayTasksDialog::preloadExistingTodayTasks()
{
    m_existingTasks.clear();
    if (m_repo)
        m_existingTasks = m_repo->activeTasksDueOnDate(QDate::currentDate());

    QStringList lines;
    lines.reserve(m_existingTasks.size());
    for (const TaskItem &t : m_existingTasks)
        lines.append(t.title);

    m_editExisting->blockSignals(true);
    m_editExisting->setPlainText(lines.join(QChar('\n')));
    m_editExisting->setReadOnly(m_existingTasks.isEmpty());
    m_editExisting->blockSignals(false);

    m_labelExistingHeader->setText(tr("已有任务 (%1)").arg(m_existingTasks.size()));
}

QStringList TodayTasksDialog::newTaskTitles() const
{
    const QStringList existingLines = linesPreservingPositions(m_editExisting);

    QSet<QString> remainingExisting;
    for (int i = 0; i < m_existingTasks.size() && i < existingLines.size(); ++i) {
        const QString trimmed = existingLines.at(i).trimmed();
        if (!trimmed.isEmpty())
            remainingExisting.insert(trimmed);
    }

    QStringList titles;
    QSet<QString> seen;

    for (const QString &line : nonEmptyLines(m_editNew)) {
        if (remainingExisting.contains(line) || seen.contains(line))
            continue;
        titles.append(line);
        seen.insert(line);
    }

    for (int i = m_existingTasks.size(); i < existingLines.size(); ++i) {
        const QString trimmed = existingLines.at(i).trimmed();
        if (trimmed.isEmpty() || remainingExisting.contains(trimmed) || seen.contains(trimmed))
            continue;
        titles.append(trimmed);
        seen.insert(trimmed);
    }

    return titles;
}

QVector<TaskItem> TodayTasksDialog::updatedExistingTasks() const
{
    QVector<TaskItem> updated;
    const QStringList current = linesPreservingPositions(m_editExisting);

    const int compareCount = qMin(current.size(), m_existingTasks.size());
    for (int i = 0; i < compareCount; ++i) {
        const QString trimmed = current.at(i).trimmed();
        if (trimmed.isEmpty() || trimmed == m_existingTasks.at(i).title)
            continue;
        TaskItem task = m_existingTasks.at(i);
        task.title = trimmed;
        updated.append(task);
    }
    return updated;
}

QVector<qint64> TodayTasksDialog::deletedExistingTaskIds() const
{
    QVector<qint64> ids;
    const QStringList current = linesPreservingPositions(m_editExisting);

    for (int i = 0; i < m_existingTasks.size(); ++i) {
        if (i >= current.size() || current.at(i).trimmed().isEmpty())
            ids.append(m_existingTasks.at(i).id);
    }
    return ids;
}

bool TodayTasksDialog::hasAnyChanges() const
{
    return !deletedExistingTaskIds().isEmpty()
           || !updatedExistingTasks().isEmpty()
           || !newTaskTitles().isEmpty();
}

void TodayTasksDialog::onTextChanged()
{
    if (m_editExisting->isReadOnly() && !m_editExisting->toPlainText().trimmed().isEmpty())
        m_editExisting->setReadOnly(false);
    refreshAddButtonLabel();
}

void TodayTasksDialog::refreshAddButtonLabel()
{
    const int changeCount = deletedExistingTaskIds().size()
                            + updatedExistingTasks().size()
                            + newTaskTitles().size();
    if (changeCount > 0)
        m_btnOk->setText(tr("保存 (%1)").arg(changeCount));
    else
        m_btnOk->setText(tr("添加"));
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

    QSet<QString> seen;
    for (const TaskItem &t : m_existingTasks)
        seen.insert(t.title);
    for (const QString &line : nonEmptyLines(m_editExisting))
        seen.insert(line);
    for (const QString &line : nonEmptyLines(m_editNew))
        seen.insert(line);

    QStringList newLines = nonEmptyLines(m_editNew);
    int added = 0;
    for (const TaskItem &t : unfinished) {
        if (seen.contains(t.title))
            continue;
        newLines.append(t.title);
        seen.insert(t.title);
        ++added;
    }

    m_editNew->setPlainText(newLines.join(QChar('\n')));
    if (added > 0) {
        QMessageBox::information(this, tr("导入"), tr("已导入 %1 条到「新增任务」。").arg(added));
        refreshAddButtonLabel();
    } else {
        QMessageBox::information(this, tr("导入"), tr("昨日未完成任务均已在列表中。"));
    }
}
