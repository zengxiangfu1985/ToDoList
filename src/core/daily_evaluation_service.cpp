#include "daily_evaluation_service.h"
#include "task_repository.h"
#include "task_archive.h"
#include "ai/llm_provider.h"
#include "ai/ai_prompts.h"
#include "../utils/app_logger.h"
#include "../utils/llm_json_utils.h"

#include <QDateTime>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QtConcurrent>
#include <algorithm>

namespace {

struct DayEvalContext {
    QDate date;
    QVector<TaskItem> completedOnDay;
    QVector<TaskItem> dueOnDay;
    QVector<TaskItem> pendingAtEnd;
    QVector<TaskItem> plannedTasks;
    QVector<TaskItem> adhocTasks;
    QDateTime firstBatchAt;
    FocusDayStats focusStats;
};

void enrichCreatedAtFromRepo(TaskRepository *repo, QVector<TaskItem> &tasks)
{
    QHash<qint64, TaskItem> byId;
    for (const TaskItem &t : repo->allTasks())
        byId.insert(t.id, t);
    for (TaskItem &t : tasks) {
        if (t.createdAt.isValid())
            continue;
        const auto it = byId.constFind(t.id);
        if (it != byId.constEnd() && it->createdAt.isValid())
            t.createdAt = it->createdAt;
    }
}

void classifyTaskBatches(DayEvalContext &ctx)
{
    ctx.plannedTasks.clear();
    ctx.adhocTasks.clear();
    ctx.firstBatchAt = QDateTime();

    QVector<TaskItem> withTime;
    for (const TaskItem &t : ctx.dueOnDay) {
        if (t.createdAt.isValid())
            withTime.append(t);
    }
    if (withTime.isEmpty()) {
        ctx.plannedTasks = ctx.dueOnDay;
        return;
    }

    std::sort(withTime.begin(), withTime.end(), [](const TaskItem &a, const TaskItem &b) {
        return a.createdAt < b.createdAt;
    });
    ctx.firstBatchAt = withTime.first().createdAt.toLocalTime();
    const QDateTime cutoff = ctx.firstBatchAt.addSecs(3600);

    for (const TaskItem &t : ctx.dueOnDay) {
        if (!t.createdAt.isValid() || t.createdAt.toLocalTime() <= cutoff)
            ctx.plannedTasks.append(t);
        else
            ctx.adhocTasks.append(t);
    }
}

DayEvalContext buildDayContext(TaskRepository *repo, const QDate &date)
{
    DayEvalContext ctx;
    ctx.date = date;
    ctx.completedOnDay = repo->tasksCompletedOnDate(date);
    ctx.dueOnDay = repo->tasksDueOnDate(date);

    QString snapErr;
    const QVector<TaskItem> snapshot = TaskArchive::loadSnapshot(date, &snapErr);
    if (!snapshot.isEmpty())
        ctx.dueOnDay = snapshot;

    enrichCreatedAtFromRepo(repo, ctx.dueOnDay);
    classifyTaskBatches(ctx);

    if (!snapshot.isEmpty()) {
        for (const TaskItem &t : snapshot) {
            if (!t.completed)
                ctx.pendingAtEnd.append(t);
        }
    } else {
        const QDateTime endOfDayLocal(date, QTime(23, 59, 59));
        const QDateTime endUtc = endOfDayLocal.toUTC();
        for (const TaskItem &t : repo->allTasks()) {
            if (t.completed)
                continue;
            if (t.createdAt.isValid() && t.createdAt > endUtc)
                continue;
            if (t.dueAt.isValid() && t.dueAt.toLocalTime().date() <= date)
                ctx.pendingAtEnd.append(t);
            else if (!t.dueAt.isValid() && t.createdAt.isValid()
                     && t.createdAt.toLocalTime().date() <= date)
                ctx.pendingAtEnd.append(t);
        }
    }
    repo->focusStatsForDate(date, &ctx.focusStats);
    return ctx;
}

QJsonObject taskToPromptJson(const TaskItem &t)
{
    QJsonObject o;
    o.insert(QStringLiteral("id"), static_cast<double>(t.id));
    o.insert(QStringLiteral("title"), t.title);
    o.insert(QStringLiteral("completed"), t.completed);
    o.insert(QStringLiteral("quadrant"), static_cast<int>(t.quadrant));
    if (t.createdAt.isValid())
        o.insert(QStringLiteral("created_at"), t.createdAt.toString(Qt::ISODate));
    return o;
}

QJsonObject buildDayEvalJson(const DayEvalContext &ctx)
{
    QJsonObject root;
    root.insert(QStringLiteral("date"), ctx.date.toString(Qt::ISODate));

    QJsonArray completed;
    for (const TaskItem &t : ctx.completedOnDay) {
        QJsonObject o;
        o.insert(QStringLiteral("id"), static_cast<double>(t.id));
        o.insert(QStringLiteral("title"), t.title);
        o.insert(QStringLiteral("completed_at"),
                  t.completedAt.isValid() ? t.completedAt.toString(Qt::ISODate) : QString());
        completed.append(o);
    }
    root.insert(QStringLiteral("completed_today"), completed);

    QJsonArray due;
    for (const TaskItem &t : ctx.dueOnDay) {
        QJsonObject o;
        o.insert(QStringLiteral("id"), static_cast<double>(t.id));
        o.insert(QStringLiteral("title"), t.title);
        o.insert(QStringLiteral("completed"), t.completed);
        due.append(o);
    }
    root.insert(QStringLiteral("due_today"), due);

    QJsonArray pending;
    for (const TaskItem &t : ctx.pendingAtEnd) {
        QJsonObject o;
        o.insert(QStringLiteral("id"), static_cast<double>(t.id));
        o.insert(QStringLiteral("title"), t.title);
        pending.append(o);
    }
    root.insert(QStringLiteral("still_pending"), pending);

    QJsonArray dayTasks;
    for (const TaskItem &t : ctx.dueOnDay)
        dayTasks.append(taskToPromptJson(t));
    root.insert(QStringLiteral("daily_tasks"), dayTasks);

    QJsonArray planned;
    for (const TaskItem &t : ctx.plannedTasks)
        planned.append(taskToPromptJson(t));
    root.insert(QStringLiteral("planned_batch"), planned);

    QJsonArray adhoc;
    for (const TaskItem &t : ctx.adhocTasks)
        adhoc.append(taskToPromptJson(t));
    root.insert(QStringLiteral("adhoc_tasks"), adhoc);

    if (ctx.firstBatchAt.isValid())
        root.insert(QStringLiteral("first_batch_at"), ctx.firstBatchAt.toString(Qt::ISODate));

    if (ctx.focusStats.sessionCount > 0) {
        QJsonObject focus;
        focus.insert(QStringLiteral("sessions"), ctx.focusStats.sessionCount);
        focus.insert(QStringLiteral("completed_sessions"), ctx.focusStats.completedSessions);
        focus.insert(QStringLiteral("abandoned_sessions"), ctx.focusStats.abandonedSessions);
        focus.insert(QStringLiteral("total_focus_minutes"), ctx.focusStats.totalFocusMinutes);
        root.insert(QStringLiteral("focus_stats"), focus);
    }

    return root;
}

QString buildEvalPrompt(const DayEvalContext &ctx)
{
    return QStringLiteral(
               "请根据以下 JSON 对当日任务做整体评估，严格遵循系统提示中的字段说明与输出格式。\n")
           + QString::fromUtf8(QJsonDocument(buildDayEvalJson(ctx)).toJson(QJsonDocument::Compact));
}

QString formatAiFeedbackFromJson(const QJsonObject &o, const DayEvalContext &ctx)
{
    QString text;
    const int score = o.value(QStringLiteral("score")).toInt(-1);
    if (score >= 0)
        text += QStringLiteral("完成度评分：%1/100\n\n").arg(score);

    const QString summary = o.value(QStringLiteral("summary")).toString();
    if (!summary.isEmpty())
        text += QStringLiteral("总评：%1\n\n").arg(summary);

    const QString planReview = o.value(QStringLiteral("task_plan_review")).toString();
    if (!planReview.isEmpty())
        text += QStringLiteral("当日任务评价：%1\n\n").arg(planReview.left(200));

    auto appendList = [&](const QString &title, const QString &key) {
        const QJsonArray arr = o.value(key).toArray();
        if (arr.isEmpty())
            return;
        text += title + QStringLiteral("\n");
        for (const QJsonValue &v : arr)
            text += QStringLiteral("  • %1\n").arg(v.toString());
        text += QStringLiteral("\n");
    };
    appendList(QStringLiteral("今日亮点"), QStringLiteral("highlights"));
    appendList(QStringLiteral("待改进"), QStringLiteral("improvements"));

    const QString tip = o.value(QStringLiteral("tomorrow_tip")).toString();
    if (!tip.isEmpty())
        text += QStringLiteral("明日建议：%1\n").arg(tip);

    if (text.isEmpty())
        text = o.value(QStringLiteral("summary")).toString();
    if (text.isEmpty()) {
        text = QStringLiteral("完成 %1 项，当日到期 %2 项，未完结 %3 项。")
                   .arg(ctx.completedOnDay.size())
                   .arg(ctx.dueOnDay.size())
                   .arg(ctx.pendingAtEnd.size());
    }
    return text.trimmed();
}

QJsonObject buildRuleEvalJson(const DayEvalContext &ctx)
{
    int dueCompleted = 0;
    for (const TaskItem &t : ctx.dueOnDay) {
        if (t.completed)
            ++dueCompleted;
    }

    const int dueTotal = ctx.dueOnDay.size();
    const int completedCount = ctx.completedOnDay.size();
    const int pendingCount = ctx.pendingAtEnd.size();
    const int score = dueTotal > 0 ? qBound(0, static_cast<int>(100.0 * dueCompleted / dueTotal), 100)
                                   : (completedCount > 0 ? 80 : 50);

    QString summary = QStringLiteral("完成 %1 项").arg(completedCount);
    if (dueTotal > 0)
        summary += QStringLiteral("，到期任务完成 %1/%2").arg(dueCompleted).arg(dueTotal);
    if (ctx.focusStats.sessionCount > 0) {
        summary += QStringLiteral("，专注 %1 分钟（完成 %2/%3 轮）")
                       .arg(ctx.focusStats.totalFocusMinutes)
                       .arg(ctx.focusStats.completedSessions)
                       .arg(ctx.focusStats.sessionCount);
    }

    QString planReview;
    if (!ctx.dueOnDay.isEmpty()) {
        QStringList plannedTitles;
        for (const TaskItem &t : ctx.plannedTasks)
            plannedTitles.append(t.title);
        planReview = QStringLiteral("计划任务 %1 项").arg(ctx.plannedTasks.size());
        if (!plannedTitles.isEmpty())
            planReview += QStringLiteral("（%1）").arg(plannedTitles.join(QStringLiteral("、")));

        if (!ctx.adhocTasks.isEmpty()) {
            QStringList adhocTitles;
            int adhocDone = 0;
            for (const TaskItem &t : ctx.adhocTasks) {
                adhocTitles.append(t.title);
                if (t.completed)
                    ++adhocDone;
            }
            planReview += QStringLiteral("；临时新增 %1 项（%2），完成 %3 项。")
                              .arg(ctx.adhocTasks.size())
                              .arg(adhocTitles.join(QStringLiteral("、")))
                              .arg(adhocDone);
            planReview += QStringLiteral(" 临时任务增加了当日负荷，建议明日控制计划外插入。");
        } else if (dueTotal > 0) {
            planReview += QStringLiteral(" 完成 %1/%2 项，").arg(dueCompleted).arg(dueTotal);
            if (dueCompleted == dueTotal)
                planReview += QStringLiteral("任务清单已全部完成。");
            else if (dueCompleted >= dueTotal / 2)
                planReview += QStringLiteral("过半已完成。");
            else
                planReview += QStringLiteral("完成率偏低。");
        }
        planReview = planReview.left(200);
    } else {
        planReview = QStringLiteral("当日无明确到期任务清单。");
    }

    QJsonArray highlights;
    if (dueTotal > 0 && dueCompleted == dueTotal)
        highlights.append(QStringLiteral("到期任务全部完成"));
    else if (completedCount > 0)
        highlights.append(QStringLiteral("完成 %1 项任务").arg(completedCount));
    if (ctx.adhocTasks.isEmpty() && dueTotal > 0)
        highlights.append(QStringLiteral("无计划外临时插入，执行节奏稳定"));
    if (ctx.focusStats.completedSessions > 0)
        highlights.append(QStringLiteral("专注 %1 分钟，完成 %2 轮")
                              .arg(ctx.focusStats.totalFocusMinutes)
                              .arg(ctx.focusStats.completedSessions));

    QJsonArray improvements;
    if (!ctx.adhocTasks.isEmpty())
        improvements.append(QStringLiteral("控制计划外临时任务的插入频率"));
    if (pendingCount > 0)
        improvements.append(QStringLiteral("优先处理 %1 项未完结任务").arg(pendingCount));
    if (ctx.focusStats.abandonedSessions > 0)
        improvements.append(QStringLiteral("减少专注中断，今日有 %1 轮未完成")
                              .arg(ctx.focusStats.abandonedSessions));
    if (dueTotal > 0 && dueCompleted < dueTotal / 2)
        improvements.append(QStringLiteral("提高到期任务的完成率"));

    QString tomorrowTip;
    if (pendingCount > 0)
        tomorrowTip = QStringLiteral("优先处理遗留的 %1 项未完结任务").arg(pendingCount);
    else if (dueTotal > 0 && dueCompleted == dueTotal)
        tomorrowTip = QStringLiteral("保持当前节奏，合理规划明日任务清单");
    else
        tomorrowTip = QStringLiteral("设定明确的当日任务清单并按时执行");

    QJsonObject o;
    o.insert(QStringLiteral("summary"), summary);
    o.insert(QStringLiteral("score"), score);
    o.insert(QStringLiteral("task_plan_review"), planReview);
    o.insert(QStringLiteral("highlights"), highlights);
    o.insert(QStringLiteral("improvements"), improvements);
    o.insert(QStringLiteral("tomorrow_tip"), tomorrowTip);
    return o;
}

DailyEvaluation buildRuleEvaluation(const DayEvalContext &ctx)
{
    DailyEvaluation eval;
    eval.evalDate = ctx.date;
    eval.tasksCompleted = ctx.completedOnDay.size();
    eval.tasksDue = ctx.dueOnDay.size();
    eval.tasksPending = ctx.pendingAtEnd.size();
    eval.usedLlm = false;
    eval.createdAt = QDateTime::currentDateTimeUtc();

    const QJsonObject ruleJson = buildRuleEvalJson(ctx);
    const QString summary = ruleJson.value(QStringLiteral("summary")).toString();
    eval.summary = ctx.date.toString(QStringLiteral("yyyy-MM-dd")) + QStringLiteral("：") + summary;
    eval.taskPlanReview = ruleJson.value(QStringLiteral("task_plan_review")).toString();
    eval.aiFeedback = formatAiFeedbackFromJson(ruleJson, ctx);
    return eval;
}

DailyEvaluation buildLlmEvaluation(const DayEvalContext &ctx, const LlmConfig &config)
{
    DailyEvaluation eval = buildRuleEvaluation(ctx);
    eval.llmProvider = config.provider;
    eval.llmModel = config.model;

    auto provider = LlmProviderFactory::create(config);
    if (!provider || !provider->isConfigured()) {
        eval.summary += QStringLiteral("（LLM 未配置，规则评估）");
        return eval;
    }

    LlmRequest req;
    req.systemPrompt = AiPrompts::dailyEvaluationSystemPrompt();
    req.userPrompt = buildEvalPrompt(ctx);
    req.temperature = AiPrompts::dailyEvalTemperature();
    req.maxTokens = AiPrompts::dailyEvalMaxTokens(config.provider);

    const LlmResponse llm = provider->complete(req);
    if (!llm.success) {
        AppLogger::warn("AI", QStringLiteral("每日评估 LLM 失败: %1").arg(llm.errorMessage));
        eval.summary += QStringLiteral("（AI 不可用，已用规则评估）");
        return eval;
    }

    const QByteArray jsonBytes = LlmJsonUtils::extractPayload(llm.content);
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(jsonBytes, &err);
    if (!doc.isObject()) {
        AppLogger::warn("AI", QStringLiteral("每日评估 JSON 解析失败: %1").arg(err.errorString()));
        eval.aiFeedback = llm.content.trimmed() + QStringLiteral("\n\n") + eval.aiFeedback;
        eval.summary += QStringLiteral("（AI 响应非 JSON，已保留原文）");
        return eval;
    }

    const QJsonObject o = doc.object();
    eval.usedLlm = true;
    const QString summary = o.value(QStringLiteral("summary")).toString();
    if (!summary.isEmpty())
        eval.summary = ctx.date.toString(QStringLiteral("yyyy-MM-dd")) + QStringLiteral("：") + summary;
    const QString planReview = o.value(QStringLiteral("task_plan_review")).toString();
    if (!planReview.isEmpty())
        eval.taskPlanReview = planReview.left(200);
    eval.aiFeedback = formatAiFeedbackFromJson(o, ctx);
    return eval;
}

DailyEvaluation runDailyEvaluationRegenerate(TaskRepository *repo, const QDate &date, const LlmConfig &config)
{
    const DayEvalContext ctx = buildDayContext(repo, date);
    DailyEvaluation eval = buildLlmEvaluation(ctx, config);
    eval.evalDate = date;
    eval.createdAt = QDateTime::currentDateTimeUtc();

    QString err;
    if (!repo->saveDailyEvaluation(eval, &err))
        AppLogger::error("AI", QStringLiteral("保存每日评估失败: %1").arg(err));

    AppLogger::info("AI",
                    QStringLiteral("每日评估完成 date=%1 completed=%2 llm=%3 model=%4")
                        .arg(date.toString(Qt::ISODate))
                        .arg(eval.tasksCompleted)
                        .arg(eval.usedLlm)
                        .arg(eval.llmModel));
    return eval;
}

DailyEvaluation runDailyEvaluation(TaskRepository *repo, const QDate &date, const LlmConfig &config)
{
    if (repo->hasDailyEvaluation(date))
        return repo->dailyEvaluationForDate(date);

    return runDailyEvaluationRegenerate(repo, date, config);
}

} // namespace

class DailyEvaluationService::Private
{
public:
    QFutureWatcher<DailyEvaluation> *watcher = nullptr;
    QDate currentDate;
    int catchUpEvaluated = 0;
};

DailyEvaluationService::DailyEvaluationService(TaskRepository *repo, QObject *parent)
    : QObject(parent)
    , m_repo(repo)
    , m_config(LlmProviderFactory::loadFromSettings())
    , m_scheduleTimer(new QTimer(this))
    , d(new Private)
{
    qRegisterMetaType<DailyEvaluation>("DailyEvaluation");
    d->watcher = new QFutureWatcher<DailyEvaluation>(this);
    connect(d->watcher, &QFutureWatcher<DailyEvaluation>::finished, this,
            &DailyEvaluationService::onEvaluationFutureFinished);

    m_scheduleTimer->setInterval(30000);
    connect(m_scheduleTimer, &QTimer::timeout, this, &DailyEvaluationService::onScheduleTick);
}

DailyEvaluationService::~DailyEvaluationService()
{
    delete d;
}

void DailyEvaluationService::setLlmConfig(const LlmConfig &config)
{
    m_config = config;
    if (m_config.provider == LlmProviderType::Ollama && m_config.timeoutMs < 180000)
        m_config.timeoutMs = 180000;
}

void DailyEvaluationService::start()
{
    armScheduleTimer();
    m_scheduleTimer->start();
    QTimer::singleShot(3000, this, &DailyEvaluationService::runCatchUp);
}

bool DailyEvaluationService::isBusy() const
{
    return m_busy;
}

QVector<QDate> DailyEvaluationService::pendingCatchUpDates() const
{
    return missedEvaluationDates();
}

QDate DailyEvaluationService::lastEvaluableDate() const
{
    const QDate today = QDate::currentDate();
    const QTime now = QTime::currentTime();
    if (now >= QTime(23, 59, 0))
        return today;
    return today.addDays(-1);
}

bool DailyEvaluationService::shouldEvaluateDate(const QDate &date) const
{
    if (!date.isValid() || m_repo->hasDailyEvaluation(date))
        return false;

    const DayEvalContext ctx = buildDayContext(m_repo, date);
    if (!ctx.completedOnDay.isEmpty() || !ctx.dueOnDay.isEmpty())
        return true;

    QString err;
    return !TaskArchive::loadSnapshot(date, &err).isEmpty();
}

QVector<QDate> DailyEvaluationService::missedEvaluationDates() const
{
    QVector<QDate> missed;
    QDate start = m_repo->earliestTaskActivityDate();

    for (const QString &ds : TaskArchive::availableHistoryDates()) {
        const QDate d = QDate::fromString(ds, QStringLiteral("yyyy-MM-dd"));
        if (d.isValid() && (!start.isValid() || d < start))
            start = d;
    }
    if (!start.isValid())
        start = QDate::currentDate().addDays(-30);

    const QDate end = lastEvaluableDate();
    for (QDate d = start; d <= end; d = d.addDays(1)) {
        if (shouldEvaluateDate(d))
            missed.append(d);
    }
    return missed;
}

void DailyEvaluationService::armScheduleTimer()
{
    onScheduleTick();
}

void DailyEvaluationService::onScheduleTick()
{
    const QDate today = QDate::currentDate();
    const QTime now = QTime::currentTime();
    if (now < QTime(23, 59, 0))
        return;
    if (m_lastScheduledRunDate == today)
        return;
    if (m_busy)
        return;

    m_lastScheduledRunDate = today;
    if (shouldEvaluateDate(today))
        evaluateDateAsync(today);
}

void DailyEvaluationService::runCatchUp()
{
    if (m_busy)
        return;
    d->catchUpEvaluated = 0;
    enqueueCatchUpDates(missedEvaluationDates());
    processNextCatchUp();
}

void DailyEvaluationService::enqueueCatchUpDates(const QVector<QDate> &dates)
{
    for (const QDate &d : dates) {
        if (!m_catchUpQueue.contains(d))
            m_catchUpQueue.append(d);
    }
    std::sort(m_catchUpQueue.begin(), m_catchUpQueue.end());
}

void DailyEvaluationService::processNextCatchUp()
{
    if (m_busy || m_catchUpQueue.isEmpty())
        return;
    evaluateDateAsync(m_catchUpQueue.takeFirst());
}

void DailyEvaluationService::evaluateDateAsync(const QDate &date)
{
    if (!date.isValid() || m_busy)
        return;
    if (m_repo->hasDailyEvaluation(date))
        return;

    m_busy = true;
    d->currentDate = date;
    emit evaluationStarted(date);

    const LlmConfig cfg = m_config;
    TaskRepository *repo = m_repo;
    QFuture<DailyEvaluation> future = QtConcurrent::run([repo, date, cfg]() {
        return runDailyEvaluation(repo, date, cfg);
    });
    d->watcher->setFuture(future);
}

void DailyEvaluationService::regenerateDateAsync(const QDate &date)
{
    if (!date.isValid() || m_busy)
        return;

    m_busy = true;
    d->currentDate = date;
    emit evaluationStarted(date);

    const LlmConfig cfg = m_config;
    TaskRepository *repo = m_repo;
    QFuture<DailyEvaluation> future = QtConcurrent::run([repo, date, cfg]() {
        return runDailyEvaluationRegenerate(repo, date, cfg);
    });
    d->watcher->setFuture(future);
}

void DailyEvaluationService::onEvaluationFutureFinished()
{
    const QDate date = d->currentDate;
    const DailyEvaluation result = d->watcher->result();
    m_busy = false;
    ++d->catchUpEvaluated;
    emit evaluationFinished(date, result);

    if (!m_catchUpQueue.isEmpty()) {
        processNextCatchUp();
        return;
    }

    const int count = d->catchUpEvaluated;
    d->catchUpEvaluated = 0;
    emit catchUpFinished(count);
}
