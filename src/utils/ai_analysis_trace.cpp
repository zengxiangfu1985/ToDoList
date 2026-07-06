#include "ai_analysis_trace.h"
#include "../core/task_archive.h"
#include "app_logger.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

void AiAnalysisTrace::append(const QString &level, const QString &category, const QString &message,
                             const QString &detail)
{
    AnalysisTraceEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.level = level;
    entry.category = category;
    entry.message = message;
    entry.detail = detail;
    m_entries.append(entry);

    const QString logLine = detail.isEmpty() ? message : message + QStringLiteral("\n") + detail.left(2000);
    if (level == QStringLiteral("WARN"))
        AppLogger::warn(category, logLine);
    else if (level == QStringLiteral("ERROR"))
        AppLogger::error(category, logLine);
    else if (level == QStringLiteral("DEBUG"))
        AppLogger::debug(category, logLine);
    else
        AppLogger::info(category, logLine);
}

void AiAnalysisTrace::info(const QString &category, const QString &message, const QString &detail)
{
    append(QStringLiteral("INFO"), category, message, detail);
}

void AiAnalysisTrace::warn(const QString &category, const QString &message, const QString &detail)
{
    append(QStringLiteral("WARN"), category, message, detail);
}

void AiAnalysisTrace::error(const QString &category, const QString &message, const QString &detail)
{
    append(QStringLiteral("ERROR"), category, message, detail);
}

void AiAnalysisTrace::debug(const QString &category, const QString &message, const QString &detail)
{
    append(QStringLiteral("DEBUG"), category, message, detail);
}

QVector<AnalysisTraceEntry> AiAnalysisTrace::takeEntries()
{
    return m_entries;
}

QString AiAnalysisTrace::latestTraceFilePath()
{
    return TaskArchive::defaultDataDirectory() + QStringLiteral("/logs/ai-analysis-latest.log");
}

QString AiAnalysisTrace::formatAsText(const PriorityAnalysisResult &result)
{
    QString text;
    QTextStream out(&text);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#endif

    out << QStringLiteral("========== AI 分析过程 ==========\n");
    if (result.analysisStartedAt.isValid())
        out << QStringLiteral("开始: ") << result.analysisStartedAt.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")) << "\n";
    if (result.analysisFinishedAt.isValid())
        out << QStringLiteral("结束: ") << result.analysisFinishedAt.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")) << "\n";
    out << QStringLiteral("象限划分: ")
        << (result.usedLlmForQuadrants ? QStringLiteral("AI") : QStringLiteral("规则推断")) << "\n";
    out << QStringLiteral("Top3: ") << (result.usedLlm ? QStringLiteral("AI") : QStringLiteral("规则层")) << "\n";
    if (!result.quadrantClassifyMessage.isEmpty())
        out << QStringLiteral("象限说明: ") << result.quadrantClassifyMessage << "\n";
    if (!result.errorMessage.isEmpty())
        out << QStringLiteral("备注/错误: ") << result.errorMessage << "\n";
    out << QStringLiteral("----------------------------------\n");

    for (const AnalysisTraceEntry &e : result.trace) {
        out << e.timestamp.toString(QStringLiteral("HH:mm:ss.zzz"))
            << " [" << e.level << "] [" << e.category << "] " << e.message << "\n";
        if (!e.detail.isEmpty())
            out << e.detail << "\n";
    }

    if (!result.quadrantAssignments.isEmpty()) {
        out << QStringLiteral("\n--- 象限划分结果 ---\n");
        for (const QuadrantAssignment &a : result.quadrantAssignments) {
            out << QStringLiteral("  id=") << a.taskId << QStringLiteral(" -> Q")
                << static_cast<int>(a.quadrant) << QStringLiteral("  ") << a.reason << "\n";
        }
    }

    if (!result.top3.isEmpty()) {
        out << QStringLiteral("\n--- Top 3 推荐 ---\n");
        for (const PriorityRecommendation &rec : result.top3) {
            out << QStringLiteral("  #") << rec.rank << QStringLiteral(" id=") << rec.taskId
                << QStringLiteral(" (") << rec.score << QStringLiteral(") ") << rec.title << "\n";
            out << QStringLiteral("    ") << rec.reason << "\n";
        }
    }

    out << QStringLiteral("========== 结束 ==========\n");
    return text;
}

bool AiAnalysisTrace::saveToFile(const PriorityAnalysisResult &result, QString *errorMessage)
{
    const QString dir = TaskArchive::defaultDataDirectory() + QStringLiteral("/logs");
    if (!QDir().mkpath(dir)) {
        if (errorMessage)
            *errorMessage = QObject::tr("无法创建日志目录");
        return false;
    }

    const QString path = latestTraceFilePath();
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if (errorMessage)
            *errorMessage = file.errorString();
        return false;
    }

    QTextStream stream(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#endif
    stream << formatAsText(result);
    return true;
}
