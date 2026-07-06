#include "weekly_report_dialog.h"

#include "../core/weekly_report_service.h"
#include "../core/ai/llm_service.h"
#include "../core/task_repository.h"
#include "../utils/app_theme.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

namespace {

enum HistoryColumn {
    ColWeek = 0,
    ColModel,
    ColType,
    ColCreated,
    ColCount
};

} // namespace

WeeklyReportDialog::WeeklyReportDialog(TaskRepository *repo, LlmService *llmService,
                                       WeeklyReportService *reportService, QWidget *parent)
    : QDialog(parent)
    , m_repo(repo)
    , m_llmService(llmService)
    , m_reportService(reportService)
    , m_weekStart(QDate::currentDate().addDays(-6))
    , m_weekEnd(QDate::currentDate())
{
    setWindowTitle(tr("生成周报"));
    resize(980, 660);
    AppTheme::styleDialog(this);
    setWindowFlag(Qt::WindowMaximizeButtonHint, true);

    auto *root = new QVBoxLayout(this);
    root->addWidget(new QLabel(tr(
        "勾选任务生成周报；历史记录自动保存。打开本窗口时将显示最近一次生成的周报，可编辑、删除或重新生成。")));

    m_localHintLabel = new QLabel(this);
    m_localHintLabel->setWordWrap(true);
    m_localHintLabel->setStyleSheet(QStringLiteral("color: #c9a227;"));
    root->addWidget(m_localHintLabel);

    m_rangeLabel = new QLabel(this);
    m_rangeLabel->setWordWrap(true);
    root->addWidget(m_rangeLabel);

    auto *mainSplit = new QSplitter(Qt::Horizontal, this);

    m_historyTable = new QTableWidget(mainSplit);
    m_historyTable->setColumnCount(ColCount);
    m_historyTable->setHorizontalHeaderLabels(
        {tr("周期"), tr("AI 模型"), tr("方式"), tr("生成时间")});
    m_historyTable->horizontalHeader()->setStretchLastSection(true);
    m_historyTable->horizontalHeader()->setSectionResizeMode(ColWeek, QHeaderView::Stretch);
    m_historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_historyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_historyTable->setMinimumWidth(300);

    auto *rightPanel = new QWidget(mainSplit);
    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    auto *selectRow = new QHBoxLayout;
    auto *btnAll = new QPushButton(tr("全选"), this);
    auto *btnNone = new QPushButton(tr("全不选"), this);
    auto *btnCompleted = new QPushButton(tr("仅选已完成"), this);
    selectRow->addWidget(btnAll);
    selectRow->addWidget(btnNone);
    selectRow->addWidget(btnCompleted);
    selectRow->addStretch();
    rightLayout->addLayout(selectRow);

    m_taskList = new QListWidget(this);
    m_taskList->setMinimumHeight(140);
    rightLayout->addWidget(m_taskList);

    m_metaLabel = new QLabel(this);
    m_metaLabel->setWordWrap(true);
    rightLayout->addWidget(m_metaLabel);

    rightLayout->addWidget(new QLabel(tr("周报内容（可编辑）"), this));
    m_reportEdit = new QTextEdit(this);
    m_reportEdit->setPlaceholderText(tr("生成的周报将显示在这里…"));
    rightLayout->addWidget(m_reportEdit, 1);

    mainSplit->addWidget(m_historyTable);
    mainSplit->addWidget(rightPanel);
    mainSplit->setStretchFactor(0, 2);
    mainSplit->setStretchFactor(1, 3);
    root->addWidget(mainSplit, 1);

    auto *actionRow = new QHBoxLayout;
    m_btnGenerate = new QPushButton(tr("生成周报"), this);
    m_btnBackground = new QPushButton(tr("后台运行"), this);
    m_btnDelete = new QPushButton(tr("删除选中"), this);
    m_btnSave = new QPushButton(tr("保存编辑"), this);
    m_btnExport = new QPushButton(tr("导出文件"), this);
    auto *btnClose = new QPushButton(tr("关闭"), this);
    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet(QStringLiteral("color: #888;"));

    actionRow->addWidget(m_btnGenerate);
    actionRow->addWidget(m_btnBackground);
    actionRow->addWidget(m_btnDelete);
    actionRow->addWidget(m_btnSave);
    actionRow->addStretch();
    actionRow->addWidget(m_statusLabel);
    actionRow->addWidget(m_btnExport);
    actionRow->addWidget(btnClose);
    root->addLayout(actionRow);

    connect(btnAll, &QPushButton::clicked, this, &WeeklyReportDialog::onSelectAll);
    connect(btnNone, &QPushButton::clicked, this, &WeeklyReportDialog::onSelectNone);
    connect(btnCompleted, &QPushButton::clicked, this, &WeeklyReportDialog::onSelectCompleted);
    connect(m_btnGenerate, &QPushButton::clicked, this, &WeeklyReportDialog::onGenerate);
    connect(m_btnBackground, &QPushButton::clicked, this, &WeeklyReportDialog::onRunInBackground);
    connect(m_btnDelete, &QPushButton::clicked, this, &WeeklyReportDialog::onDelete);
    connect(m_btnSave, &QPushButton::clicked, this, &WeeklyReportDialog::onSaveEdit);
    connect(m_btnExport, &QPushButton::clicked, this, &WeeklyReportDialog::onExport);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_historyTable, &QTableWidget::itemSelectionChanged, this,
            &WeeklyReportDialog::onHistorySelectionChanged);

    if (m_reportService) {
        connect(m_reportService, &WeeklyReportService::generationStarted, this,
                &WeeklyReportDialog::onGenerationStarted);
        connect(m_reportService, &WeeklyReportService::generationFinished, this,
                &WeeklyReportDialog::onGenerationFinished);
    }

    updateLocalModelHint();
    populateTaskList();
    refreshHistoryList();

    if (m_reportService && m_reportService->isBusy()) {
        setBusyUi(true);
        m_reportEdit->setPlainText(
            tr("正在后台生成周报（%1 ~ %2），请稍候…")
                .arg(m_reportService->pendingWeekStart().toString(QStringLiteral("yyyy-MM-dd")),
                     m_reportService->pendingWeekEnd().toString(QStringLiteral("yyyy-MM-dd"))));
    }
}

QString WeeklyReportDialog::formatTaskListLabel(const TaskItem &t) const
{
    QString label = t.title;
    if (t.completed) {
        label += tr("  [已完成");
        if (t.completedAt.isValid())
            label += tr(" %1").arg(t.completedAt.toLocalTime().toString(QStringLiteral("MM-dd")));
        label += QLatin1Char(']');
    } else {
        label += tr("  [未完成");
        if (t.dueAt.isValid())
            label += tr(" 截止%1").arg(t.dueAt.toLocalTime().toString(QStringLiteral("MM-dd")));
        label += QLatin1Char(']');
    }
    return label;
}

QString WeeklyReportDialog::providerLabel(LlmProviderType type)
{
    switch (type) {
    case LlmProviderType::Ollama: return QStringLiteral("Ollama");
    case LlmProviderType::DeepSeek: return QStringLiteral("DeepSeek");
    case LlmProviderType::Kimi: return QStringLiteral("Kimi");
    case LlmProviderType::CustomOpenAI: return QStringLiteral("Custom");
    }
    return QStringLiteral("-");
}

QString WeeklyReportDialog::modelDisplayText(const WeeklyReportRecord &record)
{
    if (record.llmModel.trimmed().isEmpty() && !record.usedLlm)
        return tr("规则模板");
    const QString provider = providerLabel(record.llmProvider);
    if (record.llmModel.trimmed().isEmpty())
        return provider;
    return provider + QStringLiteral(" / ") + record.llmModel;
}

QSet<qint64> WeeklyReportDialog::parseTaskIdsJson(const QString &json)
{
    QSet<qint64> ids;
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (!doc.isArray())
        return ids;
    for (const QJsonValue &v : doc.array()) {
        const qint64 id = static_cast<qint64>(v.toDouble());
        if (id > 0)
            ids.insert(id);
    }
    return ids;
}

void WeeklyReportDialog::updateLocalModelHint()
{
    if (!m_llmService) {
        m_localHintLabel->clear();
        return;
    }
    const LlmConfig cfg = m_llmService->config();
    if (cfg.provider == LlmProviderType::Ollama) {
        m_localHintLabel->setText(
            tr("当前为本地模型（Ollama / %1），生成周报可能需要数分钟。"
               "可点击「后台运行」关闭窗口后在后台继续生成，完成后会通知您。")
                .arg(cfg.model.trimmed().isEmpty() ? tr("未命名") : cfg.model));
    } else {
        m_localHintLabel->setText(
            tr("当前 AI 模型：%1 / %2")
                .arg(providerLabel(cfg.provider),
                     cfg.model.trimmed().isEmpty() ? tr("-") : cfg.model));
    }
}

void WeeklyReportDialog::populateTaskList(const QSet<qint64> &checkedIds)
{
    m_taskList->clear();
    m_rangeLabel->setText(tr("任务统计周期：%1 至 %2（最近 7 天）")
                              .arg(m_weekStart.toString(QStringLiteral("yyyy-MM-dd")),
                                   m_weekEnd.toString(QStringLiteral("yyyy-MM-dd"))));

    const QVector<TaskItem> tasks = collectWeekCandidateTasks(m_repo, m_weekStart, m_weekEnd);
    if (tasks.isEmpty()) {
        auto *item = new QListWidgetItem(tr("（该周期内暂无任务记录）"), m_taskList);
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        m_btnGenerate->setEnabled(false);
        m_btnBackground->setEnabled(false);
        return;
    }

    m_btnGenerate->setEnabled(true);
    for (const TaskItem &t : tasks) {
        auto *item = new QListWidgetItem(formatTaskListLabel(t), m_taskList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        const bool checked = checkedIds.isEmpty() ? t.completed : checkedIds.contains(t.id);
        item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
        item->setData(Qt::UserRole, t.id);
    }
}

QVector<qint64> WeeklyReportDialog::selectedTaskIds() const
{
    QVector<qint64> ids;
    for (int i = 0; i < m_taskList->count(); ++i) {
        const QListWidgetItem *item = m_taskList->item(i);
        if (!item || item->checkState() != Qt::Checked)
            continue;
        const qint64 id = item->data(Qt::UserRole).toLongLong();
        if (id > 0)
            ids.append(id);
    }
    return ids;
}

QVector<TaskItem> WeeklyReportDialog::selectedTasks() const
{
    const QVector<qint64> ids = selectedTaskIds();
    QSet<qint64> idSet(ids.cbegin(), ids.cend());
    QVector<TaskItem> selected;
    for (const TaskItem &t : collectWeekCandidateTasks(m_repo, m_weekStart, m_weekEnd)) {
        if (idSet.contains(t.id))
            selected.append(t);
    }
    return selected;
}

WeeklyReportRecord WeeklyReportDialog::selectedRecord() const
{
    if (m_selectedRecordId <= 0 || !m_repo)
        return {};
    for (const WeeklyReportRecord &r : m_repo->allWeeklyReports()) {
        if (r.id == m_selectedRecordId)
            return r;
    }
    return {};
}

void WeeklyReportDialog::refreshHistoryList()
{
    m_historyTable->setRowCount(0);
    if (!m_repo)
        return;

    const QVector<WeeklyReportRecord> list = m_repo->allWeeklyReports();
    m_historyTable->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i) {
        const WeeklyReportRecord &r = list.at(i);
        const QString week = QStringLiteral("%1 ~ %2")
                                 .arg(r.weekStart.toString(QStringLiteral("yyyy-MM-dd")),
                                      r.weekEnd.toString(QStringLiteral("yyyy-MM-dd")));
        auto *weekItem = new QTableWidgetItem(week);
        weekItem->setData(Qt::UserRole, r.id);
        m_historyTable->setItem(i, ColWeek, weekItem);
        m_historyTable->setItem(i, ColModel, new QTableWidgetItem(modelDisplayText(r)));
        m_historyTable->setItem(i, ColType,
                                new QTableWidgetItem(r.usedLlm ? tr("AI 生成") : tr("规则模板")));
        const QString created = r.createdAt.isValid()
            ? r.createdAt.toLocalTime().toString(QStringLiteral("MM-dd HH:mm"))
            : QStringLiteral("-");
        m_historyTable->setItem(i, ColCreated, new QTableWidgetItem(created));
    }

    if (list.isEmpty()) {
        m_selectedRecordId = 0;
        m_metaLabel->setText(tr("尚无已保存的周报。勾选任务后点击「生成周报」。"));
        m_btnDelete->setEnabled(false);
        m_btnSave->setEnabled(false);
        m_btnExport->setEnabled(m_reportEdit->toPlainText().trimmed().length() > 0);
        return;
    }

    m_historyTable->selectRow(0);
}

void WeeklyReportDialog::loadRecordIntoDetail(const WeeklyReportRecord &record)
{
    if (!record.weekStart.isValid()) {
        m_selectedRecordId = 0;
        m_metaLabel->clear();
        return;
    }

    m_selectedRecordId = record.id;
    m_weekStart = record.weekStart;
    m_weekEnd = record.weekEnd;

    const QSet<qint64> savedIds = parseTaskIdsJson(record.selectedTaskIdsJson);
    populateTaskList(savedIds);

    const QString created = record.createdAt.isValid()
        ? record.createdAt.toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm"))
        : QStringLiteral("-");
    m_metaLabel->setText(
        tr("周期：%1 ~ %2  |  方式：%3  |  AI 模型：%4  |  生成时间：%5")
            .arg(record.weekStart.toString(QStringLiteral("yyyy-MM-dd")),
                 record.weekEnd.toString(QStringLiteral("yyyy-MM-dd")),
                 record.usedLlm ? tr("AI 生成") : tr("规则模板"),
                 modelDisplayText(record),
                 created));

    m_reportEdit->setPlainText(record.markdown);
    m_btnDelete->setEnabled(record.id > 0);
    m_btnSave->setEnabled(record.id > 0);
    m_btnExport->setEnabled(!record.markdown.trimmed().isEmpty());
}

void WeeklyReportDialog::onHistorySelectionChanged()
{
    const int row = m_historyTable->currentRow();
    if (row < 0 || !m_historyTable->item(row, ColWeek))
        return;
    const qint64 id = m_historyTable->item(row, ColWeek)->data(Qt::UserRole).toLongLong();
    for (const WeeklyReportRecord &r : m_repo->allWeeklyReports()) {
        if (r.id == id) {
            loadRecordIntoDetail(r);
            return;
        }
    }
}

void WeeklyReportDialog::setBusyUi(bool busy)
{
    const bool canAct = !busy;
    m_btnGenerate->setEnabled(canAct && m_taskList->count() > 0);
    m_btnBackground->setEnabled(canAct || busy);
    m_btnDelete->setEnabled(canAct && m_selectedRecordId > 0);
    m_btnSave->setEnabled(canAct && m_selectedRecordId > 0);
    m_taskList->setEnabled(canAct);
    m_historyTable->setEnabled(canAct);

    if (busy) {
        m_btnGenerate->setText(tr("生成中…"));
        m_btnBackground->setText(tr("后台运行"));
        m_statusLabel->setText(tr("正在生成周报…"));
    } else {
        m_btnGenerate->setText(tr("生成周报"));
        m_btnBackground->setText(tr("后台运行"));
        if (m_statusLabel->text() == tr("正在生成周报…"))
            m_statusLabel->clear();
    }
}

void WeeklyReportDialog::startGeneration()
{
    if (!m_repo || !m_reportService || !m_llmService) {
        QMessageBox::warning(this, tr("生成周报"), tr("服务未就绪"));
        return;
    }
    if (m_reportService->isBusy()) {
        QMessageBox::information(this, tr("生成周报"), tr("周报正在生成中，请稍候或使用「后台运行」关闭窗口"));
        return;
    }
    if (m_llmService->isBusy()) {
        QMessageBox::information(this, tr("生成周报"), tr("AI 正在执行优先级分析，请稍后再试"));
        return;
    }

    const QVector<TaskItem> selected = selectedTasks();
    if (selected.isEmpty()) {
        QMessageBox::information(this, tr("生成周报"), tr("请至少勾选一项任务"));
        return;
    }

    const WeeklyReportRecord existing = m_repo->weeklyReportForWeek(m_weekStart, m_weekEnd);
    if (existing.id > 0) {
        const int ret = QMessageBox::question(
            this, tr("重新生成"),
            tr("该周期（%1 ~ %2）已有周报，重新生成将覆盖现有内容（含手动编辑）。是否继续？")
                .arg(m_weekStart.toString(QStringLiteral("yyyy-MM-dd")),
                     m_weekEnd.toString(QStringLiteral("yyyy-MM-dd"))),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret != QMessageBox::Yes)
            return;
    }

    m_reportEdit->setPlainText(tr("正在调用 AI 生成周报，请稍候…"));
    setBusyUi(true);
    m_reportService->setLlmConfig(m_llmService->config());
    m_reportService->generateAsync(selected, m_weekStart, m_weekEnd);
}

void WeeklyReportDialog::onGenerate()
{
    startGeneration();
}

void WeeklyReportDialog::onRunInBackground()
{
    if (m_reportService && m_reportService->isBusy()) {
        accept();
        return;
    }
    startGeneration();
    if (m_reportService && m_reportService->isBusy())
        accept();
}

void WeeklyReportDialog::onDelete()
{
    WeeklyReportRecord record = selectedRecord();
    if (record.id <= 0)
        return;

    const int ret = QMessageBox::question(
        this, tr("删除周报"),
        tr("确定删除 %1 ~ %2 的周报记录吗？")
            .arg(record.weekStart.toString(QStringLiteral("yyyy-MM-dd")),
                 record.weekEnd.toString(QStringLiteral("yyyy-MM-dd"))),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;

    QString err;
    if (!m_repo->deleteWeeklyReport(record.id, &err)) {
        QMessageBox::warning(this, tr("删除周报"), err);
        return;
    }

    m_reportEdit->clear();
    m_selectedRecordId = 0;
    refreshHistoryList();
}

void WeeklyReportDialog::onSaveEdit()
{
    WeeklyReportRecord record = selectedRecord();
    if (record.id <= 0) {
        QMessageBox::information(this, tr("保存编辑"), tr("请先在左侧选择一条周报记录"));
        return;
    }

    record.markdown = m_reportEdit->toPlainText();
    QString err;
    if (!m_repo->saveWeeklyReport(record, &err)) {
        QMessageBox::warning(this, tr("保存编辑"), err);
        return;
    }

    m_statusLabel->setText(tr("已保存编辑"));
    refreshHistoryList();
    for (int row = 0; row < m_historyTable->rowCount(); ++row) {
        if (m_historyTable->item(row, ColWeek)->data(Qt::UserRole).toLongLong() == record.id) {
            m_historyTable->selectRow(row);
            break;
        }
    }
}

void WeeklyReportDialog::onExport()
{
    const QString content = m_reportEdit->toPlainText().trimmed();
    if (content.isEmpty()) {
        QMessageBox::information(this, tr("生成周报"), tr("请先生成或选择周报"));
        return;
    }

    const QString defaultName =
        tr("WeeklyReport-%1-%2.md")
            .arg(m_weekStart.toString(QStringLiteral("yyyyMMdd")),
                 m_weekEnd.toString(QStringLiteral("yyyyMMdd")));
    const QString path = QFileDialog::getSaveFileName(this, tr("保存周报"), defaultName,
                                                      tr("Markdown 文件 (*.md);;文本文件 (*.txt)"));
    if (path.isEmpty())
        return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QMessageBox::warning(this, tr("保存周报"), tr("无法写入文件：%1").arg(path));
        return;
    }
    file.write(content.toUtf8());
    file.close();
    QMessageBox::information(this, tr("保存周报"), tr("已保存至：\n%1").arg(path));
}

void WeeklyReportDialog::onSelectAll()
{
    for (int i = 0; i < m_taskList->count(); ++i) {
        if (QListWidgetItem *item = m_taskList->item(i))
            item->setCheckState(Qt::Checked);
    }
}

void WeeklyReportDialog::onSelectNone()
{
    for (int i = 0; i < m_taskList->count(); ++i) {
        if (QListWidgetItem *item = m_taskList->item(i))
            item->setCheckState(Qt::Unchecked);
    }
}

void WeeklyReportDialog::onSelectCompleted()
{
    for (int i = 0; i < m_taskList->count(); ++i) {
        QListWidgetItem *item = m_taskList->item(i);
        if (!item)
            continue;
        item->setCheckState(item->text().contains(tr("[已完成")) ? Qt::Checked
                                                                            : Qt::Unchecked);
    }
}

void WeeklyReportDialog::onGenerationStarted(const QDate &weekStart, const QDate &weekEnd)
{
    Q_UNUSED(weekStart);
    Q_UNUSED(weekEnd);
    setBusyUi(true);
}

void WeeklyReportDialog::onGenerationFinished(const WeeklyReportRecord &record,
                                              const WeeklyReportResult &result)
{
    setBusyUi(false);

    if (!isVisible())
        return;

    refreshHistoryList();

    if (!result.success) {
        m_reportEdit->setPlainText(result.errorMessage);
        m_statusLabel->setText(tr("生成失败"));
        return;
    }

    if (record.id > 0)
        loadRecordIntoDetail(record);

    if (!result.usedLlm && !result.errorMessage.isEmpty()) {
        const bool templateFallback = result.errorMessage.contains(QStringLiteral("模型输出结构"));
        QMessageBox::information(
            this, tr("生成周报"),
            templateFallback
                ? tr("模型输出未通过校验，已使用规则模板补全周报。\n%1").arg(result.errorMessage)
                : tr("AI 不可用，已使用规则模板生成周报。\n%1").arg(result.errorMessage));
    } else {
        m_statusLabel->setText(tr("已生成（%1）").arg(modelDisplayText(record)));
    }
}
