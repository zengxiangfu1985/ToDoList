#ifndef PRIORITY_ENGINE_H
#define PRIORITY_ENGINE_H

#include "task_types.h"

#include <QVector>

// Scenario: S02 Step 2 — 规则层 RICE-like 评分
class PriorityEngine
{
public:
    static double computeRuleScore(const TaskItem &task);
    static QVector<TaskItem> applyRuleScores(const QVector<TaskItem> &tasks);
    static QVector<PriorityRecommendation> top3FromRules(const QVector<TaskItem> &tasks);
    static QString buildLlmPrompt(const QVector<TaskItem> &tasks);
};

#endif // PRIORITY_ENGINE_H
