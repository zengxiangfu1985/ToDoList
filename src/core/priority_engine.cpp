#include "priority_engine.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <algorithm>

static double importanceFromQuadrant(EisenhowerQuadrant q)
{
    switch (q) {
    case EisenhowerQuadrant::Unassigned: return 4.0;
    case EisenhowerQuadrant::Q1_UrgentImportant: return 10.0;
    case EisenhowerQuadrant::Q2_NotUrgentImportant: return 8.0;
    case EisenhowerQuadrant::Q3_UrgentNotImportant: return 5.0;
    case EisenhowerQuadrant::Q4_NotUrgentNotImportant: return 2.0;
    }
    return 5.0;
}

static QString quadrantUserLabel(EisenhowerQuadrant q)
{
    switch (q) {
    case EisenhowerQuadrant::Q1_UrgentImportant: return QStringLiteral("重要且紧急");
    case EisenhowerQuadrant::Q2_NotUrgentImportant: return QStringLiteral("重要不紧急");
    case EisenhowerQuadrant::Q3_UrgentNotImportant: return QStringLiteral("紧急不重要");
    case EisenhowerQuadrant::Q4_NotUrgentNotImportant: return QStringLiteral("不重要不紧急");
    case EisenhowerQuadrant::Unassigned: return QStringLiteral("待划分优先级");
    }
    return QStringLiteral("待划分优先级");
}

static double urgencyFromDue(const QDateTime &due)
{
    if (!due.isValid())
        return 3.0;

    const qint64 hours = QDateTime::currentDateTimeUtc().secsTo(due.toUTC()) / 3600;
    if (hours <= 0)
        return 10.0;
    if (hours <= 24)
        return 9.0;
    if (hours <= 72)
        return 7.0;
    if (hours <= 168)
        return 5.0;
    return 2.0;
}

double PriorityEngine::computeRuleScore(const TaskItem &task, const ScoringWeights &weights)
{
    if (task.completed)
        return 0.0;

    ScoringWeights w = weights;
    w.normalize();

    const double urgency = urgencyFromDue(task.dueAt);
    const double importance = importanceFromQuadrant(task.quadrant);
    const double businessImpact = importance * 0.8;
    const double effortCost = 5.0;

    return urgency * w.urgency + importance * w.importance + businessImpact * w.business
           + (10.0 - effortCost) * w.effort;
}

QVector<TaskItem> PriorityEngine::applyRuleScores(const QVector<TaskItem> &tasks, const ScoringWeights &weights)
{
    QVector<TaskItem> out = tasks;
    for (TaskItem &t : out)
        t.ruleScore = computeRuleScore(t, weights);
    return out;
}

QVector<PriorityRecommendation> PriorityEngine::top3FromRules(const QVector<TaskItem> &tasks,
                                                              const ScoringWeights &weights)
{
    QVector<TaskItem> scored = applyRuleScores(tasks, weights);
    std::sort(scored.begin(), scored.end(), [](const TaskItem &a, const TaskItem &t) {
        return a.ruleScore > t.ruleScore;
    });

    QVector<PriorityRecommendation> top;
    int rank = 1;
    for (const TaskItem &t : scored) {
        if (t.completed || t.ruleScore <= 0)
            continue;
        PriorityRecommendation rec;
        rec.taskId = t.id;
        rec.title = t.title;
        rec.rank = rank++;
        rec.score = t.ruleScore;
        rec.reason = QStringLiteral("%1，综合评分 %2")
                         .arg(quadrantUserLabel(t.quadrant))
                         .arg(t.ruleScore, 0, 'f', 1);
        top.append(rec);
        if (top.size() >= 3)
            break;
    }
    return top;
}

QString PriorityEngine::buildLlmPrompt(const QVector<TaskItem> &tasks, int maxCandidates)
{
    QVector<TaskItem> candidates;
    for (const TaskItem &t : tasks) {
        if (!t.completed)
            candidates.append(t);
    }
    std::sort(candidates.begin(), candidates.end(), [](const TaskItem &a, const TaskItem &b) {
        return a.ruleScore > b.ruleScore;
    });
    if (maxCandidates > 0 && candidates.size() > maxCandidates)
        candidates.resize(maxCandidates);

    QJsonArray arr;
    for (const TaskItem &t : candidates) {
        QJsonObject obj;
        obj.insert(QStringLiteral("id"), static_cast<double>(t.id));
        obj.insert(QStringLiteral("title"), t.title);
        obj.insert(QStringLiteral("quadrant"), static_cast<int>(t.quadrant));
        obj.insert(QStringLiteral("due_at"), t.dueAt.isValid() ? t.dueAt.toString(Qt::ISODate) : QString());
        obj.insert(QStringLiteral("rule_score"), t.ruleScore);
        arr.append(obj);
    }

    QJsonObject root;
    root.insert(QStringLiteral("tasks"), arr);
    return QStringLiteral(
               "请根据以下 JSON 中的 tasks 选出 Top 3，严格遵循系统提示中的选择原则与输出格式。\n")
           + QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

QString PriorityEngine::buildQuadrantClassificationPrompt(const QVector<TaskItem> &tasks)
{
    QJsonArray arr;
    for (const TaskItem &t : tasks) {
        if (t.completed || isQuadrantAssigned(t.quadrant))
            continue;
        QJsonObject obj;
        obj.insert(QStringLiteral("id"), static_cast<double>(t.id));
        obj.insert(QStringLiteral("title"), t.title);
        obj.insert(QStringLiteral("due_at"), t.dueAt.isValid() ? t.dueAt.toString(Qt::ISODate) : QString());
        arr.append(obj);
    }

    QJsonObject root;
    root.insert(QStringLiteral("tasks"), arr);
    return QStringLiteral(
               "请根据以下 JSON 中的 tasks 分配象限，严格遵循系统提示中的象限定义与输出格式。\n")
           + QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

static double importanceFromTitle(const QString &title)
{
    const QString t = title.toLower();
    static const QStringList high = {
        QStringLiteral("mvp"), QStringLiteral("代码"), QStringLiteral("备份"),
        QStringLiteral("软件"), QStringLiteral("项目"), QStringLiteral("算法"),
        QStringLiteral("ai"), QStringLiteral("todolist"), QStringLiteral("fitness"),
        QStringLiteral("羽毛球"), QStringLiteral("skills"), QStringLiteral("skill"),
        QStringLiteral("回顾"), QStringLiteral("沟通"), QStringLiteral("习惯")
    };
    static const QStringList low = {
        QStringLiteral("清理"), QStringLiteral("握力"), QStringLiteral("俯卧撑"),
        QStringLiteral("耳朵"), QStringLiteral("抽屉"), QStringLiteral("取货"),
        QStringLiteral("木粉"), QStringLiteral("节食"), QStringLiteral("艾灸")
    };
    for (const QString &k : high) {
        if (t.contains(k))
            return 8.5;
    }
    for (const QString &k : low) {
        if (t.contains(k))
            return 3.0;
    }
    return 5.0;
}

EisenhowerQuadrant PriorityEngine::guessQuadrantForUnassigned(const TaskItem &task)
{
    const double urgency = urgencyFromDue(task.dueAt);
    const double importance = importanceFromTitle(task.title);

    if (importance >= 7.0 && urgency >= 7.0)
        return EisenhowerQuadrant::Q1_UrgentImportant;
    if (importance >= 7.0)
        return EisenhowerQuadrant::Q2_NotUrgentImportant;
    if (urgency >= 7.0)
        return EisenhowerQuadrant::Q3_UrgentNotImportant;
    if (importance >= 5.0)
        return EisenhowerQuadrant::Q2_NotUrgentImportant;
    return EisenhowerQuadrant::Q4_NotUrgentNotImportant;
}
