#ifndef QUICK_CAPTURE_SERVICE_H
#define QUICK_CAPTURE_SERVICE_H

#include "task_types.h"

#include <QMetaType>
#include <QVector>

class TaskRepository;

struct QuickCaptureDraft
{
    QString title;
    QDateTime dueAt;
    EisenhowerQuadrant quadrant = EisenhowerQuadrant::Unassigned;
    QString notes;
};

struct QuickCaptureParseResult
{
    bool success = false;
    bool usedLlm = false;
    QString errorMessage;
    QVector<QuickCaptureDraft> tasks;
};

class QuickCaptureService
{
public:
    static QuickCaptureParseResult parse(const QString &text, const LlmConfig &config);
    static QuickCaptureParseResult parseWithRules(const QString &text);
    static int saveTasks(TaskRepository *repo, const QVector<QuickCaptureDraft> &drafts, QString *errorMessage = nullptr);
};

Q_DECLARE_METATYPE(QuickCaptureParseResult)

#endif // QUICK_CAPTURE_SERVICE_H
