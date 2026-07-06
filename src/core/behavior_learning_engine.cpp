#include "behavior_learning_engine.h"
#include "task_repository.h"

#include <QDateTime>

BehaviorLearningEngine::BehaviorLearningEngine(TaskRepository *repo, QObject *parent)
    : QObject(parent)
    , m_repo(repo)
{
    recomputeWeights();
}

void BehaviorLearningEngine::recordEvent(BehaviorEventType type, qint64 taskId, EisenhowerQuadrant quadrant)
{
    if (!m_repo)
        return;

    QString err;
    m_repo->recordBehaviorEvent(type, taskId, quadrant, QDateTime::currentDateTimeUtc(), &err);
    recomputeWeights();
    emit weightsUpdated(m_weights);
}

ScoringWeights BehaviorLearningEngine::currentWeights() const
{
    return m_weights;
}

void BehaviorLearningEngine::refreshWeights()
{
    recomputeWeights();
}

void BehaviorLearningEngine::recomputeWeights()
{
    ScoringWeights w;
    if (!m_repo || !m_repo->isOpen()) {
        m_weights = w;
        return;
    }

    const auto stats = m_repo->behaviorStatsLast30Days();
    // 用户频繁将任务移入 Q1/Q2 → 提高 importance/urgency
    const int moveBoost = stats.quadrantMoveCounts.value(static_cast<int>(EisenhowerQuadrant::Q1_UrgentImportant), 0)
                        + stats.quadrantMoveCounts.value(static_cast<int>(EisenhowerQuadrant::Q2_NotUrgentImportant), 0);
    w.importance += qMin(0.08, moveBoost * 0.005);
    w.urgency += qMin(0.05, stats.quadrantMoveCounts.value(static_cast<int>(EisenhowerQuadrant::Q1_UrgentImportant), 0) * 0.008);

    // 高完成率象限 → 提高 business 权重
    const int completions = stats.completionCount;
    if (completions > 5)
        w.business += qMin(0.06, completions * 0.003);

    // 接受 AI 推荐次数多 → 略增 urgency（用户倾向追赶截止）
    w.urgency += qMin(0.04, stats.aiAcceptedCount * 0.01);

    w.normalize();
    m_weights = w;
}
