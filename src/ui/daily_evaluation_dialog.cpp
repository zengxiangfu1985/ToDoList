#include "daily_evaluation_dialog.h"

#include "../core/daily_evaluation_service.h"
#include "../core/task_repository.h"
#include "../utils/app_theme.h"

#include <QDateEdit>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

namespace {

enum EvalTableColumn {
    ColDate = 0,
    ColModel,
    ColStats,
    ColType,
    ColCount
};

} // namespace

DailyEvaluationDialog::DailyEvaluationDialog(TaskRepository *repo, DailyEvaluationService *evalService,
                                             QWidget *parent)
    : QDialog(parent)
    , m_repo(repo)
    , m_evalService(evalService)
{
    setWindowTitle(tr("每日评估历史"));
    resize(960, 620);
    AppTheme::styleDialog(this);
    setWindowFlag(Qt::WindowMaximizeButtonHint, true);

    auto *root = new QVBoxLayout(this);
    root->addWidget(new QLabel(tr(
        "查看、编辑或重新生成每日评估。23:59 自动生成，启动时补评遗漏日期；可手动选择日期重新评估（标注当前 LLM 设置中的模型）。")));

    auto *toolbar = new QHBoxLayout;
    toolbar->addWidget(new QLabel(tr("评估日期："), this));
    m_dateEdit = new QDateEdit(QDate::currentDate(), this);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    toolbar->addWidget(m_dateEdit);
    m_btnRegenerate = new QPushButton(tr("重新生成"), this);
    toolbar->addWidget(m_btnRegenerate);
    toolbar->addStretch();
    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet(QStringLiteral("color: #888;"));
    toolbar->addWidget(m_statusLabel);
    root->addLayout(toolbar);

    auto *split = new QHBoxLayout;
    m_table = new QTableWidget(this);
    m_table->setColumnCount(ColCount);
    m_table->setHorizontalHeaderLabels(
        {tr("日期"), tr("AI 模型"), tr("任务统计"), tr("评估方式")});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(ColDate, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(ColModel, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(ColStats, QHeaderView::ResizeToContents);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setMinimumWidth(380);

    auto *detailPanel = new QVBoxLayout;
    m_statsLabel = new QLabel(this);
    m_statsLabel->setWordWrap(true);

    detailPanel->addWidget(m_statsLabel);
    detailPanel->addWidget(new QLabel(tr("摘要"), this));
    m_summaryEdit = new QTextEdit(this);
    m_summaryEdit->setMaximumHeight(72);
    detailPanel->addWidget(m_summaryEdit);

    detailPanel->addWidget(new QLabel(tr("当日任务评价"), this));
    m_planReviewEdit = new QTextEdit(this);
    m_planReviewEdit->setMaximumHeight(100);
    detailPanel->addWidget(m_planReviewEdit);

    detailPanel->addWidget(new QLabel(tr("详细评估"), this));
    m_feedbackEdit = new QTextEdit(this);
    detailPanel->addWidget(m_feedbackEdit, 1);

    split->addWidget(m_table);
    split->addLayout(detailPanel, 1);
    root->addLayout(split, 1);

    auto *bottom = new QHBoxLayout;
    m_btnDelete = new QPushButton(tr("删除选中"), this);
    m_btnSave = new QPushButton(tr("保存编辑"), this);
    auto *btnClose = new QPushButton(tr("关闭"), this);
    bottom->addWidget(m_btnDelete);
    bottom->addWidget(m_btnSave);
    bottom->addStretch();
    bottom->addWidget(btnClose);
    root->addLayout(bottom);

    connect(m_btnRegenerate, &QPushButton::clicked, this, &DailyEvaluationDialog::onRegenerate);
    connect(m_btnDelete, &QPushButton::clicked, this, &DailyEvaluationDialog::onDelete);
    connect(m_btnSave, &QPushButton::clicked, this, &DailyEvaluationDialog::onSaveEdit);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, &DailyEvaluationDialog::onTableSelectionChanged);
    connect(m_table, &QTableWidget::cellDoubleClicked, this, [this](int row, int) {
        if (row >= 0 && m_table->item(row, ColDate))
            m_dateEdit->setDate(QDate::fromString(m_table->item(row, ColDate)->text(), QStringLiteral("yyyy-MM-dd")));
    });

    if (m_evalService) {
        connect(m_evalService, &DailyEvaluationService::evaluationStarted, this,
                &DailyEvaluationDialog::onEvaluationStarted);
        connect(m_evalService, &DailyEvaluationService::evaluationFinished, this,
                &DailyEvaluationDialog::onEvaluationFinished);
    }

    refreshList();
    setBusy(m_evalService && m_evalService->isBusy());
}

QString DailyEvaluationDialog::providerLabel(LlmProviderType type)
{
    switch (type) {
    case LlmProviderType::Ollama: return QStringLiteral("Ollama");
    case LlmProviderType::DeepSeek: return QStringLiteral("DeepSeek");
    case LlmProviderType::Kimi: return QStringLiteral("Kimi");
    case LlmProviderType::CustomOpenAI: return QStringLiteral("Custom");
    }
    return QStringLiteral("-");
}

QString DailyEvaluationDialog::modelDisplayText(const DailyEvaluation &eval)
{
    if (eval.llmModel.trimmed().isEmpty() && !eval.usedLlm)
        return tr("规则模板");
    const QString provider = providerLabel(eval.llmProvider);
    if (eval.llmModel.trimmed().isEmpty())
        return provider;
    return provider + QStringLiteral(" / ") + eval.llmModel;
}

void DailyEvaluationDialog::refreshList()
{
    m_table->setRowCount(0);
    if (!m_repo)
        return;

    const QVector<DailyEvaluation> list = m_repo->allDailyEvaluations();
    m_table->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i) {
        const DailyEvaluation &eval = list.at(i);
        m_table->setItem(i, ColDate, new QTableWidgetItem(eval.evalDate.toString(QStringLiteral("yyyy-MM-dd"))));
        m_table->setItem(i, ColModel, new QTableWidgetItem(modelDisplayText(eval)));
        m_table->setItem(i, ColStats,
                         new QTableWidgetItem(tr("完成 %1 / 到期 %2 / 未完结 %3")
                                                  .arg(eval.tasksCompleted)
                                                  .arg(eval.tasksDue)
                                                  .arg(eval.tasksPending)));
        const QString typeText = eval.usedLlm ? tr("AI 评估") : tr("规则评估");
        m_table->setItem(i, ColType, new QTableWidgetItem(typeText));
    }

    if (list.isEmpty()) {
        m_statsLabel->setText(tr("尚无评估记录。选择日期后点击「重新生成」，或等待 23:59 自动生成。"));
        m_summaryEdit->clear();
        m_planReviewEdit->clear();
        m_feedbackEdit->clear();
        m_btnDelete->setEnabled(false);
        m_btnSave->setEnabled(false);
        return;
    }

    m_table->selectRow(0);
}

QDate DailyEvaluationDialog::selectedDate() const
{
    const int row = m_table->currentRow();
    if (row < 0 || !m_table->item(row, ColDate))
        return {};
    return QDate::fromString(m_table->item(row, ColDate)->text(), QStringLiteral("yyyy-MM-dd"));
}

DailyEvaluation DailyEvaluationDialog::currentEvaluation() const
{
    const QDate date = selectedDate();
    if (!date.isValid() || !m_repo)
        return {};
    return m_repo->dailyEvaluationForDate(date);
}

void DailyEvaluationDialog::loadEvaluationIntoDetail(const DailyEvaluation &eval)
{
    if (!eval.evalDate.isValid()) {
        m_statsLabel->clear();
        m_summaryEdit->clear();
        m_planReviewEdit->clear();
        m_feedbackEdit->clear();
        m_btnDelete->setEnabled(false);
        m_btnSave->setEnabled(false);
        return;
    }

    m_dateEdit->setDate(eval.evalDate);
    const QString created = eval.createdAt.isValid()
        ? eval.createdAt.toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm"))
        : QStringLiteral("-");
    m_statsLabel->setText(
        tr("日期：%1  |  完成 %2 项  |  到期 %3 项  |  未完结 %4 项\n"
           "评估方式：%5  |  AI 模型：%6  |  生成时间：%7")
            .arg(eval.evalDate.toString(QStringLiteral("yyyy-MM-dd")))
            .arg(eval.tasksCompleted)
            .arg(eval.tasksDue)
            .arg(eval.tasksPending)
            .arg(eval.usedLlm ? tr("AI 评估") : tr("规则评估"))
            .arg(modelDisplayText(eval))
            .arg(created));

    m_summaryEdit->setPlainText(eval.summary);
    m_planReviewEdit->setPlainText(eval.taskPlanReview);
    m_feedbackEdit->setPlainText(eval.aiFeedback);
    m_btnDelete->setEnabled(true);
    m_btnSave->setEnabled(true);
}

void DailyEvaluationDialog::onTableSelectionChanged()
{
    loadEvaluationIntoDetail(currentEvaluation());
}

void DailyEvaluationDialog::setBusy(bool busy)
{
    m_btnRegenerate->setEnabled(!busy);
    m_btnDelete->setEnabled(!busy && selectedDate().isValid());
    m_btnSave->setEnabled(!busy && selectedDate().isValid());
    m_dateEdit->setEnabled(!busy);
    m_table->setEnabled(!busy);
    if (busy)
        m_statusLabel->setText(tr("正在调用 AI 生成评估…"));
    else
        m_statusLabel->clear();
}

void DailyEvaluationDialog::onRegenerate()
{
    if (!m_repo || !m_evalService) {
        QMessageBox::warning(this, tr("每日评估"), tr("服务未就绪"));
        return;
    }
    if (m_evalService->isBusy()) {
        QMessageBox::information(this, tr("每日评估"), tr("AI 正在处理其他任务，请稍候"));
        return;
    }

    const QDate date = m_dateEdit->date();
    if (!date.isValid()) {
        QMessageBox::warning(this, tr("每日评估"), tr("请选择有效日期"));
        return;
    }

    if (m_repo->hasDailyEvaluation(date)) {
        const int ret = QMessageBox::question(
            this, tr("重新生成"),
            tr("日期 %1 已有评估记录，重新生成将覆盖现有内容（含手动编辑）。是否继续？")
                .arg(date.toString(QStringLiteral("yyyy-MM-dd"))),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret != QMessageBox::Yes)
            return;
    }

    setBusy(true);
    m_evalService->regenerateDateAsync(date);
}

void DailyEvaluationDialog::onDelete()
{
    const QDate date = selectedDate();
    if (!date.isValid() || !m_repo)
        return;

    const int ret = QMessageBox::question(
        this, tr("删除评估"),
        tr("确定删除 %1 的评估记录吗？此操作不可恢复。")
            .arg(date.toString(QStringLiteral("yyyy-MM-dd"))),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;

    QString err;
    if (!m_repo->deleteDailyEvaluation(date, &err)) {
        QMessageBox::warning(this, tr("删除评估"), err);
        return;
    }
    refreshList();
}

void DailyEvaluationDialog::onSaveEdit()
{
    const QDate date = selectedDate();
    if (!date.isValid() || !m_repo)
        return;

    DailyEvaluation eval = m_repo->dailyEvaluationForDate(date);
    if (!eval.evalDate.isValid())
        return;

    eval.summary = m_summaryEdit->toPlainText().trimmed();
    eval.taskPlanReview = m_planReviewEdit->toPlainText().trimmed();
    eval.aiFeedback = m_feedbackEdit->toPlainText().trimmed();

    QString err;
    if (!m_repo->saveDailyEvaluation(eval, &err)) {
        QMessageBox::warning(this, tr("保存编辑"), err);
        return;
    }

    refreshList();
    for (int row = 0; row < m_table->rowCount(); ++row) {
        if (m_table->item(row, ColDate)->text() == date.toString(QStringLiteral("yyyy-MM-dd"))) {
            m_table->selectRow(row);
            break;
        }
    }
    m_statusLabel->setText(tr("已保存编辑"));
}

void DailyEvaluationDialog::onEvaluationStarted(const QDate &date)
{
    Q_UNUSED(date);
    setBusy(true);
}

void DailyEvaluationDialog::onEvaluationFinished(const QDate &date, const DailyEvaluation &result)
{
    Q_UNUSED(result);
    setBusy(false);
    refreshList();

    for (int row = 0; row < m_table->rowCount(); ++row) {
        if (m_table->item(row, ColDate)->text() == date.toString(QStringLiteral("yyyy-MM-dd"))) {
            m_table->selectRow(row);
            break;
        }
    }

    if (!result.evalDate.isValid())
        m_statusLabel->setText(tr("评估失败"));
    else
        m_statusLabel->setText(tr("已重新生成（%1）").arg(modelDisplayText(result)));
}
