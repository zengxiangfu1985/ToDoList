#ifndef AI_ANALYSIS_TRACE_H
#define AI_ANALYSIS_TRACE_H

#include "../core/task_types.h"

#include <QVector>

class AiAnalysisTrace
{
public:
    void info(const QString &category, const QString &message, const QString &detail = QString());
    void warn(const QString &category, const QString &message, const QString &detail = QString());
    void error(const QString &category, const QString &message, const QString &detail = QString());
    void debug(const QString &category, const QString &message, const QString &detail = QString());

    QVector<AnalysisTraceEntry> takeEntries();

    static QString formatAsText(const PriorityAnalysisResult &result);
    static bool saveToFile(const PriorityAnalysisResult &result, QString *errorMessage = nullptr);
    static QString latestTraceFilePath();

private:
    void append(const QString &level, const QString &category, const QString &message, const QString &detail);

    QVector<AnalysisTraceEntry> m_entries;
};

#endif // AI_ANALYSIS_TRACE_H
