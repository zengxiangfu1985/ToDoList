#ifndef PRIORITY_ENGINE_H
#define PRIORITY_ENGINE_H

#include "task_types.h"

#include <QVector>

class PriorityEngine
{
public:
    static double computeRuleScore(const TaskItem &task, const ScoringWeights &weights = ScoringWeights());
    static QVector<TaskItem> applyRuleScores(const QVector<TaskItem> &tasks,
                                             const ScoringWeights &weights = ScoringWeights());
    static QVector<PriorityRecommendation> top3FromRules(const QVector<TaskItem> &tasks,
                                                         const ScoringWeights &weights = ScoringWeights());
    static QString buildLlmPrompt(const QVector<TaskItem> &tasks, int maxCandidates = 0);
    static QString buildQuadrantClassificationPrompt(const QVector<TaskItem> &tasks);
    static EisenhowerQuadrant guessQuadrantForUnassigned(const TaskItem &task);
};

#endif // PRIORITY_ENGINE_H
