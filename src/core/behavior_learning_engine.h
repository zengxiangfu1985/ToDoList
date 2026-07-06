#ifndef BEHAVIOR_LEARNING_ENGINE_H
#define BEHAVIOR_LEARNING_ENGINE_H

#include "task_types.h"

#include <QObject>

class TaskRepository;

// 用户行为学习：根据象限变更/完成记录动态调整评分权重
class BehaviorLearningEngine : public QObject
{
    Q_OBJECT
public:
    explicit BehaviorLearningEngine(TaskRepository *repo, QObject *parent = nullptr);

    void recordEvent(BehaviorEventType type, qint64 taskId, EisenhowerQuadrant quadrant);
    ScoringWeights currentWeights() const;
    void refreshWeights();

signals:
    void weightsUpdated(const ScoringWeights &weights);

private:
    void recomputeWeights();

    TaskRepository *m_repo;
    ScoringWeights m_weights;
};

#endif // BEHAVIOR_LEARNING_ENGINE_H
