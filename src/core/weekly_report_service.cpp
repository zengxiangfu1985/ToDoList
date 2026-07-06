#include "weekly_report_service.h"

#include "task_archive.h"
#include "task_repository.h"
#include "ai/llm_provider.h"
#include "ai/ai_prompts.h"

#include "../utils/app_logger.h"

#include <QFutureWatcher>
#include <QtConcurrent>
#include <QDateTime>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QSet>
#include <QRegularExpression>
#include <algorithm>

const QStringList kReportCategoryOrder = {
    QStringLiteral("技术项目"),
    QStringLiteral("个人健康"),
    QStringLiteral("生活管理"),
    QStringLiteral("个人发展"),
};

bool titleContainsAny(const QString &title, const QStringList &keywords)
{
    for (const QString &kw : keywords) {
        if (title.contains(kw, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

QString reportCategoryForTask(const TaskItem &t)
{
    const QString &title = t.title;

    const QStringList personalDev = {QStringLiteral("skills"),  QStringLiteral("提炼"),
                                     QStringLiteral("回顾"),    QStringLiteral("总结文档")};
    const QStringList tech = {QStringLiteral("代码"),   QStringLiteral("AI"),     QStringLiteral("软件"),
                              QStringLiteral("MVP"),    QStringLiteral("算法"),   QStringLiteral("fitness"),
                              QStringLiteral("备份"),   QStringLiteral("文档"),   QStringLiteral("轨迹"),
                              QStringLiteral("跟踪"),   QStringLiteral("干扰")};
    const QStringList health = {QStringLiteral("艾灸"), QStringLiteral("俯卧撑"), QStringLiteral("握力"),
                                QStringLiteral("视力"), QStringLiteral("节食"),   QStringLiteral("远眺"),
                                QStringLiteral("眨眼"), QStringLiteral("晚餐")};

    if (titleContainsAny(title, personalDev))
        return QStringLiteral("个人发展");
    if (titleContainsAny(title, tech)
        || t.quadrant == EisenhowerQuadrant::Q2_NotUrgentImportant)
        return QStringLiteral("技术项目");
    if (titleContainsAny(title, health))
        return QStringLiteral("个人健康");
    if (t.quadrant == EisenhowerQuadrant::Q1_UrgentImportant && titleContainsAny(title, tech))
        return QStringLiteral("技术项目");
    return QStringLiteral("生活管理");
}

QMap<QString, QStringList> groupUniqueTaskTitles(const QVector<TaskItem> &tasks, bool completedOnly)
{
    QMap<QString, QStringList> grouped;
    QSet<QString> seen;
    for (const TaskItem &t : tasks) {
        if (completedOnly && !t.completed)
            continue;
        if (!completedOnly && t.completed)
            continue;
        if (seen.contains(t.title))
            continue;
        seen.insert(t.title);
        grouped[reportCategoryForTask(t)].append(t.title);
    }
    return grouped;
}

QString stripLeadingChars(const QString &title, const QString &prefix)
{
    return title.startsWith(prefix) ? title.mid(prefix.size()).trimmed() : title;
}

QString completedBulletForTitle(const QString &title, const QString &category)
{
    Q_UNUSED(category);

    if (title.contains(QStringLiteral("编写"))) {
        const QString obj = stripLeadingChars(title, QStringLiteral("编写"));
        return QStringLiteral("   - 编写了%1。").arg(obj);
    }
    if (title.startsWith(QStringLiteral("清理")))
        return QStringLiteral("   - 清理了%1。").arg(title.mid(2));
    if (title.endsWith(QStringLiteral("查看"))) {
        const QString obj = title.left(title.size() - 2).trimmed();
        return QStringLiteral("   - 查看了%1。").arg(obj);
    }
    if (title.startsWith(QStringLiteral("做")))
        return QStringLiteral("   - 进行了%1。").arg(title.mid(1));
    if (title.contains(QStringLiteral("算法")) || title.contains(QStringLiteral("排除")))
        return QStringLiteral("   - 完成了%1。").arg(title);
    if (title.contains(QStringLiteral("习惯")) || title.contains(QStringLiteral("手机")))
        return QStringLiteral("   - 实现了%1。").arg(title);
    if (title.contains(QStringLiteral("测试")) || title.contains(QStringLiteral("训练"))
        || title.contains(QStringLiteral("艾灸")) || title.contains(QStringLiteral("俯卧撑"))
        || title.contains(QStringLiteral("节食")) || title.contains(QStringLiteral("晚餐")))
        return QStringLiteral("   - 进行了%1。").arg(title);
    if (title.contains(QStringLiteral("保护")) || title.contains(QStringLiteral("远眺"))
        || title.contains(QStringLiteral("眨眼")))
        return QStringLiteral("   - 实施了%1。").arg(title);
    return QStringLiteral("   - 完成了%1。").arg(title);
}

QString buildCategorizedCompletedSection(const QVector<TaskItem> &tasks)
{
    const QMap<QString, QStringList> grouped = groupUniqueTaskTitles(tasks, true);
    QString md = QStringLiteral("## 本周完成工作\n\n");
    int sectionNum = 1;
    for (const QString &category : kReportCategoryOrder) {
        const QStringList titles = grouped.value(category);
        if (titles.isEmpty())
            continue;
        md += QStringLiteral("%1. **%2**\n").arg(sectionNum++).arg(category);
        for (const QString &title : titles)
            md += completedBulletForTitle(title, category) + QLatin1Char('\n');
        md += QLatin1Char('\n');
    }
    if (sectionNum == 1)
        md += QStringLiteral("1. **工作事项**\n   - （本周无已完成任务）\n\n");
    return md;
}

QString buildPendingSection(const QVector<TaskItem> &tasks)
{
    QString md = QStringLiteral("## 进行中 / 未完成事项\n\n");
    bool anyPending = false;
    for (const TaskItem &t : tasks) {
        if (t.completed)
            continue;
        anyPending = true;
        md += QStringLiteral("- %1").arg(t.title);
        if (t.dueAt.isValid())
            md += QStringLiteral("（截止 %1）").arg(t.dueAt.toLocalTime().toString(QStringLiteral("MM-dd")));
        md += QLatin1Char('\n');
    }
    if (!anyPending)
        md += QStringLiteral("本周所有计划内的工作均已完成，没有未完成事项。\n");
    md += QLatin1Char('\n');
    return md;
}

QString nextWeekLineForCategory(const QString &category, const QStringList &titles)
{
    if (titles.isEmpty())
        return {};
    if (category == QStringLiteral("技术项目")) {
        const QString primary = titles.first();
        if (primary.contains(QStringLiteral("编写")) || primary.contains(QStringLiteral("MVP"))) {
            QString name = primary;
            name.replace(QStringLiteral("编写"), QStringLiteral(""));
            return QStringLiteral("   - 继续开发%1，优化功能。").arg(name.trimmed());
        }
        if (primary.contains(QStringLiteral("算法")))
            return QStringLiteral("   - 跟进%1的稳定性与优化。").arg(primary);
        return QStringLiteral("   - 继续推进%1的后续工作。").arg(primary);
    }
    if (category == QStringLiteral("个人健康"))
        return QStringLiteral("   - 持续进行健康相关训练与养护，保持健康管理。");
    if (category == QStringLiteral("生活管理"))
        return QStringLiteral("   - 继续维持良好的生活管理习惯与日常事务节奏。");
    if (category == QStringLiteral("个人发展"))
        return QStringLiteral("   - 继续推进个人技能总结与知识沉淀。");
    return QStringLiteral("   - 继续跟进相关事项的后续安排。");
}

QString buildCategorizedNextWeekSection(const QVector<TaskItem> &tasks)
{
    const QMap<QString, QStringList> grouped = groupUniqueTaskTitles(tasks, true);
    QString md = QStringLiteral("## 下周工作计划\n\n");
    int sectionNum = 1;
    bool anyPending = false;
    for (const TaskItem &t : tasks) {
        if (!t.completed) {
            anyPending = true;
            break;
        }
    }

    if (anyPending) {
        const QMap<QString, QStringList> pendingGrouped = groupUniqueTaskTitles(tasks, false);
        for (const QString &category : kReportCategoryOrder) {
            const QStringList titles = pendingGrouped.value(category);
            if (titles.isEmpty())
                continue;
            md += QStringLiteral("%1. **%2**\n").arg(sectionNum++).arg(category);
            for (const QString &title : titles)
                md += QStringLiteral("   - 推进「%1」。").arg(title) + QLatin1Char('\n');
            md += QLatin1Char('\n');
        }
        return md;
    }

    for (const QString &category : kReportCategoryOrder) {
        const QStringList titles = grouped.value(category);
        if (titles.isEmpty())
            continue;
        md += QStringLiteral("%1. **%2**\n").arg(sectionNum++).arg(category);
        md += nextWeekLineForCategory(category, titles) + QLatin1Char('\n');
        if (category == QStringLiteral("技术项目") && titles.size() > 1) {
            for (const QString &title : titles.mid(1, 2)) {
                if (title.contains(QStringLiteral("算法")))
                    md += QStringLiteral("   - 跟进%1的稳定性。").arg(title) + QLatin1Char('\n');
            }
        }
        md += QLatin1Char('\n');
    }
    if (sectionNum == 1)
        md += QStringLiteral("1. **工作事项**\n   - 本周任务已全部完成，可根据新目标规划下周重点。\n\n");
    return md;
}

QString joinNaturalList(const QStringList &items)
{
    if (items.isEmpty())
        return {};
    if (items.size() == 1)
        return items.first();
    if (items.size() == 2)
        return items.at(0) + QStringLiteral("和") + items.at(1);
    QString joined = items.first();
    for (int i = 1; i < items.size() - 1; ++i)
        joined += QStringLiteral("、") + items.at(i);
    joined += QStringLiteral("和") + items.last();
    return joined;
}

QString buildKimiStyleOverview(const QVector<TaskItem> &tasks)
{
    const QMap<QString, QStringList> grouped = groupUniqueTaskTitles(tasks, true);
    QStringList themes;
    for (const QString &category : kReportCategoryOrder) {
        if (!grouped.value(category).isEmpty())
            themes.append(category);
    }
    if (themes.isEmpty())
        return QStringLiteral("本周暂无已完成任务记录。");

    QString text;
    if (themes.size() >= 3)
        text = QStringLiteral("本周工作主要集中在%1三个方面。").arg(joinNaturalList(themes));
    else
        text = QStringLiteral("本周工作主要集中在%1等方面。").arg(joinNaturalList(themes));

    const QStringList tech = grouped.value(QStringLiteral("技术项目"));
    if (!tech.isEmpty()) {
        bool hasBackup = false;
        bool hasDoc = false;
        bool hasMvp = false;
        for (const QString &t : tech) {
            if (t.contains(QStringLiteral("备份")))
                hasBackup = true;
            if (t.contains(QStringLiteral("总结")) || t.contains(QStringLiteral("文档")))
                hasDoc = true;
            if (t.contains(QStringLiteral("MVP")) || t.contains(QStringLiteral("编写")))
                hasMvp = true;
        }
        QStringList doneParts;
        if (hasBackup)
            doneParts.append(QStringLiteral("代码备份"));
        if (hasDoc)
            doneParts.append(QStringLiteral("项目总结文档的查看"));
        if (doneParts.isEmpty() && hasMvp)
            text += QStringLiteral("在技术领域，推进了AI todolist软件的MVP开发。");
        else if (hasMvp && !doneParts.isEmpty())
            text += QStringLiteral("在技术领域，完成了%1，同时对AI todolist软件进行了MVP开发。")
                        .arg(joinNaturalList(doneParts));
        else if (!doneParts.isEmpty())
            text += QStringLiteral("在技术领域，完成了%1。").arg(joinNaturalList(doneParts));
    }

    const QStringList health = grouped.value(QStringLiteral("个人健康"));
    if (!health.isEmpty()) {
        QStringList keywords;
        auto addKw = [&](const QString &kw, const QString &match) {
            for (const QString &t : health) {
                if (t.contains(match) && !keywords.contains(kw)) {
                    keywords.append(kw);
                    break;
                }
            }
        };
        addKw(QStringLiteral("视力保护"), QStringLiteral("视力"));
        addKw(QStringLiteral("握力训练"), QStringLiteral("握力"));
        addKw(QStringLiteral("艾灸"), QStringLiteral("艾灸"));
        addKw(QStringLiteral("体能锻炼"), QStringLiteral("俯卧撑"));
        addKw(QStringLiteral("饮食管理"), QStringLiteral("节食"));
        if (!keywords.isEmpty())
            text += QStringLiteral("个人健康方面，注重了%1等健康管理。").arg(joinNaturalList(keywords));
    }

    const QStringList life = grouped.value(QStringLiteral("生活管理"));
    if (!life.isEmpty()) {
        bool desk = false;
        bool ear = false;
        bool laundry = false;
        for (const QString &t : life) {
            if (t.contains(QStringLiteral("办公桌")))
                desk = true;
            if (t.contains(QStringLiteral("耳朵")))
                ear = true;
            if (t.contains(QStringLiteral("洗衣服")))
                laundry = true;
        }
        if (desk || ear || laundry) {
            text += QStringLiteral("生活管理上，");
            if (desk && ear)
                text += QStringLiteral("进行了办公桌和耳朵的清理");
            else if (desk)
                text += QStringLiteral("进行了办公桌的清理");
            else if (ear)
                text += QStringLiteral("进行了耳朵的清理");
            if (laundry) {
                if (desk || ear)
                    text += QStringLiteral("，以及衣物的清洗");
                else
                    text += QStringLiteral("完成了衣物的清洗");
            }
            text += QStringLiteral("。");
        }
    }

    const QStringList dev = grouped.value(QStringLiteral("个人发展"));
    if (!dev.isEmpty())
        text += QStringLiteral("个人发展方面，完成了技能提炼与项目回顾。");

    return text;
}

QString buildRuleOverviewParagraph(const QVector<TaskItem> &tasks)
{
    return buildKimiStyleOverview(tasks);
}

QString assembleWeeklyReportMarkdown(const QString &overviewSection, const QVector<TaskItem> &tasks)
{
    QString overview = overviewSection.trimmed();
    if (!overview.startsWith(QStringLiteral("## 本周工作概述"))) {
        const QString body = overview;
        overview = QStringLiteral("## 本周工作概述\n\n") + body;
    }
    if (!overview.endsWith(QStringLiteral("\n\n")))
        overview += QStringLiteral("\n\n");

    return overview + buildCategorizedCompletedSection(tasks) + buildPendingSection(tasks)
           + buildCategorizedNextWeekSection(tasks);
}

QString extractOverviewSection(const QString &normalized)
{
    const QString marker = QStringLiteral("## 本周工作概述");
    int start = normalized.indexOf(marker);
    if (start < 0)
        return normalized.trimmed();

    start += marker.size();
    const int nextSection = normalized.indexOf(QStringLiteral("\n## "), start);
    const QString body =
        (nextSection > start ? normalized.mid(start, nextSection - start) : normalized.mid(start)).trimmed();
    if (body.isEmpty())
        return {};
    return marker + QStringLiteral("\n\n") + body;
}

bool weeklyReportCoversTaskTitles(const QString &markdown, const QVector<TaskItem> &tasks)
{
    QSet<QString> required;
    for (const TaskItem &t : tasks) {
        if (t.completed)
            required.insert(t.title);
    }
    for (const QString &title : required) {
        if (!markdown.contains(title))
            return false;
    }
    return true;
}

bool titleMentionedInMarkdown(const QString &title, const QString &markdown)
{
    if (title.isEmpty())
        return true;
    if (markdown.contains(title))
        return true;

    const int minLen = qMin(4, title.size());
    if (minLen <= 0)
        return false;
    for (int len = qMin(8, title.size()); len >= minLen; --len) {
        for (int i = 0; i <= title.size() - len; ++i) {
            const QString sub = title.mid(i, len);
            if (markdown.contains(sub))
                return true;
        }
    }
    return false;
}

bool weeklyReportCoversTaskTitlesRelaxed(const QString &markdown, const QVector<TaskItem> &tasks)
{
    int required = 0;
    int matched = 0;
    for (const TaskItem &t : tasks) {
        if (!t.completed)
            continue;
        ++required;
        if (titleMentionedInMarkdown(t.title, markdown))
            ++matched;
    }
    if (required == 0)
        return true;
    const int threshold = required <= 2 ? 1 : (required + 1) / 2;
    return matched >= threshold;
}

bool overviewSectionLooksLikePromptLeak(const QString &section)
{
    static const QStringList markers = {
        QStringLiteral("【固定章节】"),
        QStringLiteral("【写作风格】"),
        QStringLiteral("【格式示例"),
        QStringLiteral("selected_tasks"),
        QStringLiteral("week_start"),
        QStringLiteral("week_end"),
        QStringLiteral("本地模型硬性要求"),
        QStringLiteral("JSON 中的"),
        QStringLiteral("禁止输出任何思考"),
        QStringLiteral("有四个固定二级标题"),
        QStringLiteral("completed=true"),
        QStringLiteral("completed=false"),
        QStringLiteral("用户JSON"),
        QStringLiteral("系统提示"),
    };
    for (const QString &m : markers) {
        if (section.contains(m))
            return true;
    }
    return false;
}

bool lineLooksLikeWeeklyReportThinking(const QString &line)
{
    const QString t = line.trimmed();
    if (t.isEmpty())
        return false;

    static const QStringList substrings = {
        QStringLiteral("completed=true"),
        QStringLiteral("completed=false"),
        QStringLiteral("selected_tasks"),
        QStringLiteral("week_start"),
        QStringLiteral("week_end"),
        QStringLiteral("task_titles"),
        QStringLiteral("用户JSON"),
        QStringLiteral("JSON中"),
        QStringLiteral("JSON数据"),
        QStringLiteral("【固定章节】"),
        QStringLiteral("【写作风格】"),
        QStringLiteral("【格式示例"),
        QStringLiteral("本地模型硬性要求"),
        QStringLiteral("系统提示"),
        QStringLiteral("有四个固定二级标题"),
        QStringLiteral("构建周报"),
        QStringLiteral("我数一下"),
        QStringLiteral("为了准确"),
        QStringLiteral("为了归类"),
        QStringLiteral("从JSON"),
        QStringLiteral("From JSON"),
        QStringLiteral("To save time"),
        QStringLiteral("For simplicity"),
        QStringLiteral("Let's "),
        QStringLiteral("I need to"),
        QStringLiteral("Actually,"),
        QStringLiteral("Wait,"),
        QStringLiteral("id:"),
        QStringLiteral("id："),
        QStringLiteral("quadrant"),
    };
    for (const QString &s : substrings) {
        if (t.contains(s, Qt::CaseInsensitive))
            return true;
    }

    static const QRegularExpression metaStart(
        QStringLiteral("^(?:首先|关键点|我需要|用户要求|分析一下|先分析|归纳|注意：|"
                         "我决定|目标是|具体：|标准分组|Key:|Better to|Confusing)"),
        QRegularExpression::CaseInsensitiveOption);
    if (metaStart.match(t).hasMatch())
        return true;

    // 把「要求/规则」类 bullet 去掉，保留真正的周报条目
    if (t.startsWith(QStringLiteral("-")) && t.contains(QStringLiteral("## 本周")))
        return true;
    if (t.startsWith(QStringLiteral("-")) && t.contains(QStringLiteral("禁止输出")))
        return true;
    if (t.startsWith(QStringLiteral("-")) && t.contains(QStringLiteral("第一行必须是")))
        return true;

    return false;
}

QString stripThinkingLines(const QString &body)
{
    const QStringList lines = body.split(QLatin1Char('\n'));
    QStringList kept;
    kept.reserve(lines.size());
    for (const QString &line : lines) {
        if (!lineLooksLikeWeeklyReportThinking(line))
            kept.append(line);
    }
    return kept.join(QStringLiteral("\n")).trimmed();
}

struct WeeklyReportSectionDef
{
    QString heading;
    QString title;
};

const QVector<WeeklyReportSectionDef> &weeklyReportSectionDefs()
{
    static const QVector<WeeklyReportSectionDef> defs = {
        {QStringLiteral("## 本周工作概述"), QStringLiteral("本周工作概述")},
        {QStringLiteral("## 本周完成工作"), QStringLiteral("本周完成工作")},
        {QStringLiteral("## 进行中 / 未完成事项"), QStringLiteral("进行中 / 未完成事项")},
        {QStringLiteral("## 下周工作计划"), QStringLiteral("下周工作计划")},
    };
    return defs;
}

bool lineIsWeeklyReportHeading(const QString &line, const QString &title)
{
    static const QRegularExpression rx(
        QStringLiteral("^##\\s+(.+?)\\s*$"));
    const QRegularExpressionMatch m = rx.match(line.trimmed());
    if (!m.hasMatch())
        return false;
    QString got = m.captured(1).trimmed();
    got.replace(QRegularExpression(QStringLiteral("\\s+")), QStringLiteral(" "));
    QString want = title;
    want.replace(QRegularExpression(QStringLiteral("\\s+")), QStringLiteral(" "));
    return got == want;
}

QString extractStructuredWeeklyReport(const QString &raw)
{
    const QStringList lines = raw.split(QLatin1Char('\n'));
    const auto &defs = weeklyReportSectionDefs();

    struct HeadingHit
    {
        int lineIndex = -1;
        int defIndex = -1;
    };
    QVector<HeadingHit> hits;

    for (int i = 0; i < lines.size(); ++i) {
        for (int d = 0; d < defs.size(); ++d) {
            if (lineIsWeeklyReportHeading(lines.at(i), defs.at(d).title)) {
                HeadingHit hit;
                hit.lineIndex = i;
                hit.defIndex = d;
                hits.append(hit);
                break;
            }
        }
    }

    if (hits.isEmpty())
        return {};

    // 取按章节顺序排列、且正文不像思考过程的最长连续链
    QVector<HeadingHit> bestChain;
    for (int start = 0; start < hits.size(); ++start) {
        if (hits.at(start).defIndex != 0)
            continue;
        QVector<HeadingHit> chain;
        chain.append(hits.at(start));
        int expect = 1;
        for (int j = start + 1; j < hits.size() && expect < defs.size(); ++j) {
            if (hits.at(j).defIndex == expect) {
                chain.append(hits.at(j));
                ++expect;
            }
        }
        if (chain.size() > bestChain.size())
            bestChain = chain;
    }

    if (bestChain.size() < defs.size())
        return {};

    QString out;
    for (int s = 0; s < bestChain.size(); ++s) {
        const int bodyStart = bestChain.at(s).lineIndex + 1;
        const int bodyEnd =
            (s + 1 < bestChain.size()) ? bestChain.at(s + 1).lineIndex : lines.size();
        QString body;
        for (int i = bodyStart; i < bodyEnd; ++i)
            body += lines.at(i) + QLatin1Char('\n');
        body = stripThinkingLines(body.trimmed());
        if (overviewSectionLooksLikePromptLeak(body))
            body.clear();

        out += defs.at(bestChain.at(s).defIndex).heading + QStringLiteral("\n\n");
        if (!body.isEmpty())
            out += body + QStringLiteral("\n\n");
    }
    return out.trimmed();
}

QString stripThinkingPreamble(const QString &text)
{
    QString out = text;
    static const QRegularExpression metaLine(
        QStringLiteral("^(?:首先|关键点|我需要|用户要求|分析|先分析|归纳|JSON|json|"
                         "task_titles|week_start|week_end|本地模型硬性要求|"
                         "写作风格|归纳成|目标是|我决定|注意：|JSON中|JSON数据).*$"),
        QRegularExpression::MultilineOption);
    out.remove(metaLine);
    static const QRegularExpression jsonBlock(QStringLiteral("\\{[\\s\\S]*?\\}"));
    out.remove(jsonBlock);
    return out.trimmed();
}

namespace {

QString quadrantShort(EisenhowerQuadrant q)
{
    switch (q) {
    case EisenhowerQuadrant::Q1_UrgentImportant:
        return QStringLiteral("Q1");
    case EisenhowerQuadrant::Q2_NotUrgentImportant:
        return QStringLiteral("Q2");
    case EisenhowerQuadrant::Q3_UrgentNotImportant:
        return QStringLiteral("Q3");
    case EisenhowerQuadrant::Q4_NotUrgentNotImportant:
        return QStringLiteral("Q4");
    default:
        return QStringLiteral("-");
    }
}

bool taskTouchesWeek(const TaskItem &t, const QDate &weekStart, const QDate &weekEnd)
{
    const auto inRange = [&](const QDate &d) {
        return d.isValid() && d >= weekStart && d <= weekEnd;
    };

    if (inRange(t.dueAt.isValid() ? t.dueAt.toLocalTime().date() : QDate()))
        return true;
    if (inRange(t.completedAt.isValid() ? t.completedAt.toLocalTime().date() : QDate()))
        return true;
    if (inRange(t.createdAt.isValid() ? t.createdAt.toLocalTime().date() : QDate()))
        return true;
    return false;
}

QJsonObject taskToPromptJson(const TaskItem &t)
{
    QJsonObject o;
    o.insert(QStringLiteral("id"), static_cast<double>(t.id));
    o.insert(QStringLiteral("title"), t.title);
    o.insert(QStringLiteral("completed"), t.completed);
    o.insert(QStringLiteral("quadrant"), quadrantShort(t.quadrant));
    if (t.completedAt.isValid())
        o.insert(QStringLiteral("completed_at"), t.completedAt.toString(Qt::ISODate));
    if (t.dueAt.isValid())
        o.insert(QStringLiteral("due_at"), t.dueAt.toString(Qt::ISODate));
    if (t.createdAt.isValid())
        o.insert(QStringLiteral("created_at"), t.createdAt.toString(Qt::ISODate));
    return o;
}

QString buildWeeklyReportPrompt(const QVector<TaskItem> &tasks, const QDate &weekStart,
                                const QDate &weekEnd, LlmProviderType provider)
{
    QJsonObject root;
    root.insert(QStringLiteral("week_start"), weekStart.toString(Qt::ISODate));
    root.insert(QStringLiteral("week_end"), weekEnd.toString(Qt::ISODate));

    QJsonArray selected;
    for (const TaskItem &t : tasks)
        selected.append(taskToPromptJson(t));
    root.insert(QStringLiteral("selected_tasks"), selected);

    return AiPrompts::weeklyReportUserInstruction(provider)
           + QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

QString buildRuleWeeklyReport(const QVector<TaskItem> &tasks, const QDate &weekStart,
                              const QDate &weekEnd)
{
    Q_UNUSED(weekStart);
    Q_UNUSED(weekEnd);
    return assembleWeeklyReportMarkdown(
        QStringLiteral("## 本周工作概述\n\n") + buildKimiStyleOverview(tasks), tasks);
}

QString stripMarkdownFence(const QString &text)
{
    QString out = text.trimmed();
    if (out.startsWith(QStringLiteral("```"))) {
        const int firstNl = out.indexOf(QLatin1Char('\n'));
        const int lastFence = out.lastIndexOf(QStringLiteral("```"));
        if (firstNl > 0 && lastFence > firstNl)
            out = out.mid(firstNl + 1, lastFence - firstNl - 1).trimmed();
    }
    return out;
}

QString stripThinkingArtifacts(const QString &text)
{
    QString out = text;
    static const QRegularExpression closedThink(
        QStringLiteral("<think(?:ing)?>[\\s\\S]*?</think(?:ing)?>"),
        QRegularExpression::CaseInsensitiveOption);
    out.remove(closedThink);
    static const QRegularExpression openThink(
        QStringLiteral("<think(?:ing)?>[\\s\\S]*$"),
        QRegularExpression::CaseInsensitiveOption);
    out.remove(openThink);
    return out.trimmed();
}

bool weeklyReportHasRequiredSections(const QString &markdown)
{
    const QStringList lines = markdown.split(QLatin1Char('\n'));
    int expect = 0;
    const auto &defs = weeklyReportSectionDefs();
    for (const QString &line : lines) {
        if (expect < defs.size() && lineIsWeeklyReportHeading(line, defs.at(expect).title))
            ++expect;
    }
    return expect >= defs.size();
}

bool weeklyReportHasSubstantiveBody(const QString &markdown)
{
    const auto &defs = weeklyReportSectionDefs();
    const QStringList lines = markdown.split(QLatin1Char('\n'));
    int section = -1;
    int substantiveLines = 0;
    for (const QString &line : lines) {
        bool isHeading = false;
        for (int d = 0; d < defs.size(); ++d) {
            if (lineIsWeeklyReportHeading(line, defs.at(d).title)) {
                section = d;
                isHeading = true;
                break;
            }
        }
        if (isHeading)
            continue;
        const QString t = line.trimmed();
        if (section >= 0 && !t.isEmpty() && !lineLooksLikeWeeklyReportThinking(line))
            ++substantiveLines;
    }
    return substantiveLines >= 4;
}

QString normalizeWeeklyReportMarkdown(const QString &raw)
{
    QString out = stripThinkingPreamble(stripThinkingArtifacts(stripMarkdownFence(raw.trimmed())));

    const QString structured = extractStructuredWeeklyReport(out);
    if (!structured.isEmpty())
        return structured;

    const QString marker = QStringLiteral("## 本周工作概述");
    const int idx = out.indexOf(marker);
    if (idx > 0)
        out = out.mid(idx).trimmed();
    else if (idx < 0) {
        const int alt = out.indexOf(QStringLiteral("本周工作概述"));
        if (alt >= 0) {
            if (alt > 0 && out.at(alt - 1) == QLatin1Char('#'))
                out = out.mid(alt - 2).trimmed();
            else
                out = QStringLiteral("## ") + out.mid(alt).trimmed();
        }
    }

    return stripThinkingLines(out.trimmed());
}

void fillReportMeta(WeeklyReportResult &result, const LlmConfig &config, const QDate &weekStart,
                    const QDate &weekEnd)
{
    result.weekStart = weekStart;
    result.weekEnd = weekEnd;
    result.llmProvider = config.provider;
    result.llmModel = config.model;
}

QString taskIdsToJson(const QVector<TaskItem> &tasks)
{
    QJsonArray arr;
    for (const TaskItem &t : tasks)
        arr.append(static_cast<double>(t.id));
    return QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
}

} // namespace

QVector<TaskItem> collectWeekCandidateTasks(TaskRepository *repo, const QDate &weekStart,
                                            const QDate &weekEnd)
{
    QHash<qint64, TaskItem> byId;
    if (!repo)
        return {};

    const auto merge = [&](const QVector<TaskItem> &items) {
        for (const TaskItem &t : items) {
            if (taskTouchesWeek(t, weekStart, weekEnd))
                byId.insert(t.id, t);
        }
    };

    for (QDate d = weekStart; d <= weekEnd; d = d.addDays(1)) {
        merge(repo->tasksCompletedOnDate(d));
        merge(repo->tasksDueOnDate(d));
        merge(TaskArchive::loadSnapshot(d));
    }

    merge(repo->allTasks());

    QVector<TaskItem> result;
    result.reserve(byId.size());
    for (const TaskItem &t : byId)
        result.append(t);
    std::sort(result.begin(), result.end(), [](const TaskItem &a, const TaskItem &b) {
        if (a.completed != b.completed)
            return a.completed > b.completed;
        return a.title.localeAwareCompare(b.title) < 0;
    });
    return result;
}

WeeklyReportResult generateWeeklyReport(const LlmConfig &config, const QVector<TaskItem> &selectedTasks,
                                        const QDate &weekStart, const QDate &weekEnd)
{
    WeeklyReportResult result;
    fillReportMeta(result, config, weekStart, weekEnd);
    if (selectedTasks.isEmpty()) {
        result.errorMessage = QStringLiteral("未选择任何任务");
        return result;
    }

    auto provider = LlmProviderFactory::create(config);
    if (!provider || !provider->isConfigured()) {
        result.markdown = buildRuleWeeklyReport(selectedTasks, weekStart, weekEnd);
        result.success = true;
        result.usedLlm = false;
        return result;
    }

    LlmRequest req;
    req.systemPrompt = AiPrompts::weeklyReportSystemPrompt(config.provider);
    req.userPrompt = buildWeeklyReportPrompt(selectedTasks, weekStart, weekEnd, config.provider);
    req.temperature = config.provider == LlmProviderType::Ollama
        ? AiPrompts::weeklyReportOllamaTemperature()
        : AiPrompts::weeklyReportTemperature();
    req.maxTokens = AiPrompts::weeklyReportMaxTokens(config.provider);

    int effectiveTimeoutMs = config.timeoutMs;
    if (config.provider == LlmProviderType::Ollama) {
        req.timeoutMsOverride = AiPrompts::weeklyReportOllamaTimeoutMs();
        effectiveTimeoutMs = req.timeoutMsOverride;
    }

    const QString providerLabel = config.provider == LlmProviderType::Ollama
        ? QStringLiteral("Ollama")
        : (config.provider == LlmProviderType::DeepSeek ? QStringLiteral("DeepSeek")
           : config.provider == LlmProviderType::Kimi ? QStringLiteral("Kimi")
                                                      : QStringLiteral("CustomOpenAI"));
    AppLogger::info("AI",
                    QStringLiteral("周报 LLM 请求开始 provider=%1 model=%2 tasks=%3 week=%4~%5 maxTokens=%6 timeout=%7ms userPromptLen=%8")
                        .arg(providerLabel, config.model)
                        .arg(selectedTasks.size())
                        .arg(weekStart.toString(Qt::ISODate), weekEnd.toString(Qt::ISODate))
                        .arg(req.maxTokens)
                        .arg(effectiveTimeoutMs)
                        .arg(req.userPrompt.size()));

    const LlmResponse llm = provider->complete(req);
    if (!llm.success) {
        AppLogger::warn("AI", QStringLiteral("周报 LLM 失败: %1").arg(llm.errorMessage));
        result.markdown = buildRuleWeeklyReport(selectedTasks, weekStart, weekEnd);
        result.success = true;
        result.usedLlm = false;
        result.errorMessage = llm.errorMessage;
        return result;
    }

    result.markdown = normalizeWeeklyReportMarkdown(llm.content);
    const bool sectionsOk = weeklyReportHasRequiredSections(result.markdown);
    const bool substantiveOk = weeklyReportHasSubstantiveBody(result.markdown);
    const bool titlesStrict = weeklyReportCoversTaskTitles(result.markdown, selectedTasks);
    const bool titlesRelaxed = weeklyReportCoversTaskTitlesRelaxed(result.markdown, selectedTasks);
    const bool llmOutputOk = !result.markdown.isEmpty() && sectionsOk && substantiveOk;
    if (!llmOutputOk) {
        AppLogger::warn("AI",
                        QStringLiteral("周报 LLM 输出不合规（rawLen=%1 normLen=%2 sections=%3 substantive=%4 titlesStrict=%5 titlesRelaxed=%6），"
                                       "使用 Kimi 对齐模板")
                            .arg(llm.content.size())
                            .arg(result.markdown.size())
                            .arg(sectionsOk)
                            .arg(substantiveOk)
                            .arg(titlesStrict)
                            .arg(titlesRelaxed));
        QString overview = extractOverviewSection(result.markdown);
        if (overview.isEmpty() || overviewSectionLooksLikePromptLeak(overview))
            overview = QStringLiteral("## 本周工作概述\n\n") + buildKimiStyleOverview(selectedTasks);
        result.markdown = assembleWeeklyReportMarkdown(overview, selectedTasks);
        result.usedLlm = false;
        if (!result.errorMessage.isEmpty())
            result.errorMessage += QStringLiteral("；");
        result.errorMessage += QStringLiteral("模型输出结构不完整，已使用标准模板补全");
    } else {
        result.usedLlm = true;
        if (!titlesStrict && titlesRelaxed) {
            AppLogger::info("AI",
                            QStringLiteral("周报 LLM 已接受（任务标题为概括表述，未逐字匹配）"));
        }
        AppLogger::info("AI",
                        QStringLiteral("周报 LLM 成功 provider=%1 markdownLen=%2 rawLen=%3")
                            .arg(providerLabel)
                            .arg(result.markdown.size())
                            .arg(llm.content.size()));
    }
    result.success = true;
    return result;
}

class WeeklyReportService::Private
{
public:
    QFutureWatcher<WeeklyReportResult> *watcher = nullptr;
};

WeeklyReportService::WeeklyReportService(TaskRepository *repo, QObject *parent)
    : QObject(parent)
    , m_repo(repo)
    , m_config(LlmProviderFactory::loadFromSettings())
    , d(new Private)
{
    qRegisterMetaType<WeeklyReportRecord>("WeeklyReportRecord");
    d->watcher = new QFutureWatcher<WeeklyReportResult>(this);
    connect(d->watcher, &QFutureWatcher<WeeklyReportResult>::finished, this,
            &WeeklyReportService::onFutureFinished);
}

WeeklyReportService::~WeeklyReportService()
{
    delete d;
}

void WeeklyReportService::setLlmConfig(const LlmConfig &config)
{
    m_config = config;
    if (m_config.provider == LlmProviderType::Ollama && m_config.timeoutMs < 180000)
        m_config.timeoutMs = 180000;
}

bool WeeklyReportService::isBusy() const
{
    return m_busy;
}

QDate WeeklyReportService::pendingWeekStart() const
{
    return m_pendingWeekStart;
}

QDate WeeklyReportService::pendingWeekEnd() const
{
    return m_pendingWeekEnd;
}

void WeeklyReportService::generateAsync(const QVector<TaskItem> &selectedTasks, const QDate &weekStart,
                                        const QDate &weekEnd)
{
    if (m_busy || selectedTasks.isEmpty())
        return;

    m_busy = true;
    m_pendingWeekStart = weekStart;
    m_pendingWeekEnd = weekEnd;
    m_pendingTaskIdsJson = taskIdsToJson(selectedTasks);
    emit generationStarted(weekStart, weekEnd);

    const LlmConfig cfg = m_config;
    QFuture<WeeklyReportResult> future =
        QtConcurrent::run(generateWeeklyReport, cfg, selectedTasks, weekStart, weekEnd);
    d->watcher->setFuture(future);
}

void WeeklyReportService::onFutureFinished()
{
    const WeeklyReportResult result = d->watcher->result();
    m_busy = false;

    WeeklyReportRecord record;
    record.weekStart = m_pendingWeekStart;
    record.weekEnd = m_pendingWeekEnd;
    record.selectedTaskIdsJson = m_pendingTaskIdsJson;
    record.createdAt = QDateTime::currentDateTimeUtc();

    if (result.success) {
        record.markdown = result.markdown;
        record.usedLlm = result.usedLlm;
        record.llmProvider = result.llmProvider;
        record.llmModel = result.llmModel;
        QString err;
        if (!m_repo->saveWeeklyReport(record, &err))
            AppLogger::error("AI", QStringLiteral("保存周报失败: %1").arg(err));
        else
            record = m_repo->weeklyReportForWeek(record.weekStart, record.weekEnd);
    }

    emit generationFinished(record, result);
}
