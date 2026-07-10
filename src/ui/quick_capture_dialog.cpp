#include "quick_capture_dialog.h"
#include "ui_quick_capture_dialog.h"

#include "../core/quick_capture_service.h"
#include "../utils/app_logger.h"
#include "../utils/app_theme.h"
#include "../utils/window_fit.h"

#include <QFutureWatcher>
#include <QMessageBox>
#include <QMetaType>
#include <QShowEvent>
#include <QTimer>
#include <QtConcurrent>

QuickCaptureDialog::QuickCaptureDialog(TaskRepository *repo, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QuickCaptureDialog)
    , m_repo(repo)
{
    static const bool metaRegistered = []() {
        qRegisterMetaType<QuickCaptureParseResult>("QuickCaptureParseResult");
        return true;
    }();

    ui->setupUi(this);
    AppTheme::styleDialog(this);
    setWindowTitle(tr("闪记"));
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    Q_UNUSED(metaRegistered);

    connect(ui->btnSave, &QPushButton::clicked, this, &QuickCaptureDialog::onSave);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

QuickCaptureDialog::~QuickCaptureDialog()
{
    delete ui;
}

void QuickCaptureDialog::setLlmConfig(const LlmConfig &config)
{
    m_llmConfig = config;
}

void QuickCaptureDialog::setAutoAnalyzeDefault(bool enabled)
{
    m_autoAnalyzeRequested = enabled;
    ui->checkAutoAnalyze->setChecked(enabled);
}

int QuickCaptureDialog::savedCount() const
{
    return m_savedCount;
}

bool QuickCaptureDialog::autoAnalyzeRequested() const
{
    return m_autoAnalyzeRequested;
}

bool QuickCaptureDialog::usedLlm() const
{
    return m_usedLlm;
}

void QuickCaptureDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    WindowFit::fitDialog(this, 480);
    ui->plainInput->setFocus();
}

void QuickCaptureDialog::setBusy(bool busy, const QString &status)
{
    ui->plainInput->setEnabled(!busy);
    ui->checkAutoAnalyze->setEnabled(!busy);
    ui->btnSave->setEnabled(!busy);
    ui->btnCancel->setEnabled(!busy);
    ui->labelStatus->setText(status);
}

void QuickCaptureDialog::showSuccessAndClose(const QString &message)
{
    ui->plainInput->setEnabled(false);
    ui->checkAutoAnalyze->setEnabled(false);
    ui->btnSave->setEnabled(false);
    ui->btnSave->setVisible(false);
    ui->btnCancel->setText(tr("关闭"));
    ui->btnCancel->setEnabled(true);
    ui->labelStatus->setText(message);
    disconnect(ui->btnCancel, nullptr, this, nullptr);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::accept);
    QTimer::singleShot(1000, this, &QDialog::accept);
}

void QuickCaptureDialog::onSave()
{
    const QString text = ui->plainInput->toPlainText().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, tr("闪记"), tr("请输入待办内容"));
        return;
    }

    m_autoAnalyzeRequested = ui->checkAutoAnalyze->isChecked();
    setBusy(true, tr("正在解析…"));
    AppLogger::info("QUICK_CAPTURE", QStringLiteral("开始解析，长度=%1").arg(text.size()));

    const LlmConfig config = m_llmConfig;
    auto *watcher = new QFutureWatcher<QuickCaptureParseResult>(this);
    connect(watcher, &QFutureWatcher<QuickCaptureParseResult>::finished, this, [this, watcher]() {
        const QuickCaptureParseResult result = watcher->result();
        watcher->deleteLater();

        AppLogger::info("QUICK_CAPTURE",
                        QStringLiteral("解析完成 tasks=%1 usedLlm=%2")
                            .arg(result.tasks.size())
                            .arg(result.usedLlm ? 1 : 0));

        if (result.tasks.isEmpty()) {
            setBusy(false);
            QMessageBox::warning(this, tr("闪记"),
                               result.errorMessage.isEmpty() ? tr("未能解析出任务") : result.errorMessage);
            return;
        }

        QString err;
        m_savedCount = QuickCaptureService::saveTasks(m_repo, result.tasks, &err);
        m_usedLlm = result.usedLlm;

        if (m_savedCount <= 0) {
            setBusy(false);
            QMessageBox::warning(this, tr("闪记"), err.isEmpty() ? tr("保存失败") : err);
            return;
        }

        AppLogger::info("QUICK_CAPTURE", QStringLiteral("已保存 %1 条任务").arg(m_savedCount));

        QString successMsg = tr("已保存 %1 条任务").arg(m_savedCount);
        if (m_usedLlm)
            successMsg += tr("（AI 解析）");
        if (m_autoAnalyzeRequested)
            successMsg += tr("，即将分析优先级…");

        if (m_savedCount < result.tasks.size()) {
            successMsg = tr("已保存 %1 条（共 %2 条，部分失败：%3）")
                               .arg(m_savedCount)
                               .arg(result.tasks.size())
                               .arg(err);
        }

        showSuccessAndClose(successMsg);
    });

    watcher->setFuture(QtConcurrent::run([text, config]() {
        return QuickCaptureService::parse(text, config);
    }));
}
