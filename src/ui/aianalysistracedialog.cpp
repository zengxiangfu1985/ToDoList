#include "aianalysistracedialog.h"

#include "../utils/ai_analysis_trace.h"
#include "../utils/app_theme.h"

#include <QDesktopServices>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QUrl>
#include <QVBoxLayout>

AiAnalysisTraceDialog::AiAnalysisTraceDialog(const PriorityAnalysisResult &result, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("AI 分析过程"));
    resize(880, 620);
    AppTheme::styleDialog(this);

    auto *root = new QVBoxLayout(this);

    QString summary = tr("象限: %1 | Top3: %2")
                          .arg(result.usedLlmForQuadrants ? tr("AI 划分") : tr("规则推断"),
                               result.usedLlm ? tr("AI 推荐") : tr("规则层"));
    if (result.analysisStartedAt.isValid()) {
        summary += tr(" | 时间: %1").arg(result.analysisStartedAt.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
    }
    auto *summaryLabel = new QLabel(summary, this);
    summaryLabel->setWordWrap(true);
    root->addWidget(summaryLabel);

    auto *browser = new QTextBrowser(this);
    QFont mono = browser->font();
    mono.setFamily(QStringLiteral("Consolas"));
    mono.setStyleHint(QFont::Monospace);
    browser->setFont(mono);
    browser->setPlainText(AiAnalysisTrace::formatAsText(result));
    root->addWidget(browser, 1);

    auto *btnRow = new QHBoxLayout;
    auto *btnOpenFile = new QPushButton(tr("打开日志文件"), this);
    auto *btnClose = new QPushButton(tr("关闭"), this);
    btnRow->addWidget(btnOpenFile);
    btnRow->addStretch();
    btnRow->addWidget(btnClose);
    root->addLayout(btnRow);

    connect(btnOpenFile, &QPushButton::clicked, this, [result]() {
        QString err;
        AiAnalysisTrace::saveToFile(result, &err);
        QDesktopServices::openUrl(QUrl::fromLocalFile(AiAnalysisTrace::latestTraceFilePath()));
    });
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
}
