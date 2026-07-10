#include "ai_prompts.h"

#include <QRegularExpression>
#include <QtGlobal>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>

namespace AiPrompts {

QString quadrantClassificationSystemPrompt()
{
    return QStringLiteral(
        "你是 Eisenhower 四象限任务分类器。根据用户 JSON 中 tasks 的 title 与 due_at，"
        "为每项未分配象限的任务分配 quadrant(1-4)，所有 LLM 输出格式保持一致。\n\n"
        "【象限定义】\n"
        "1 = 重要且紧急（Q1）\n"
        "2 = 重要不紧急（Q2）\n"
        "3 = 紧急不重要（Q3）\n"
        "4 = 不重要不紧急（Q4）\n\n"
        "【输出要求】\n"
        "1. 只输出 JSON 数组，每项含 id、quadrant、reason 三个字段。\n"
        "2. reason 用 10～40 字中文说明分类依据，须提及重要性与紧迫性。\n"
        "3. 必须为 tasks 中每一项都输出一条记录，id 与输入一致。\n"
        "4. 不要 markdown，不要 ``` 代码围栏，不要附加解释。\n\n"
        "【格式示例】\n"
        "[{\"id\":1,\"quadrant\":2,\"reason\":\"核心项目开发，重要但截止尚充裕\"}]");
}

QString top3SystemPrompt()
{
    return QStringLiteral(
        "你是待办优先级助手。根据用户 JSON 中 tasks 的 title、quadrant、due_at、rule_score，"
        "选出今天最应优先完成的 3 项，所有 LLM 输出格式保持一致。\n\n"
        "【选择原则】\n"
        "1. 综合任务重要性与截止日期紧迫性（重要且紧急 > 重要不紧急 > 紧急不重要 > 不重要不紧急）。\n"
        "2. 优先选综合评分高且重要的任务；同分时优先截止更近者。\n"
        "3. 只从 tasks 列表中选择，不得编造 id。\n\n"
        "【输出要求】\n"
        "1. 只输出 JSON 数组，每项含 id、rank(1-3)、reason 三个字段。\n"
        "2. rank 从 1 到 3 不重复。\n"
        "3. reason 必须用 15～50 字中文，面向普通用户说明「为什么今天应优先做这件事」。\n"
        "4. reason 禁止出现：rule_score、JSON 字段名、Q1/Q2/Q3/Q4、象限编号、"
        "「次高」「最高」等排序术语；请用自然表述，如「重要且紧急」「今日截止」「建议优先推进」。\n"
        "5. 恰好输出 3 项（若候选不足 3 项则输出全部）。\n"
        "6. 不要 markdown，不要 ``` 代码围栏，不要附加解释。\n\n"
        "【格式示例】\n"
        "[{\"id\":3,\"rank\":1,\"reason\":\"工作重要且今天必须提交，建议优先完成\"}]");
}

QString sanitizeTop3Reason(const QString &raw)
{
    QString text = raw.trimmed();
    if (text.isEmpty())
        return text;

    text.replace(QRegularExpression(QStringLiteral("rule_score"), QRegularExpression::CaseInsensitiveOption),
                 QStringLiteral("综合评分"));
    text.replace(QRegularExpression(QStringLiteral("rule\\s*score"), QRegularExpression::CaseInsensitiveOption),
                 QStringLiteral("综合评分"));
    text.replace(QStringLiteral("规则评分（学习权重）："), QString());
    text.replace(QStringLiteral("规则评分（学习权重）:"), QString());
    text.replace(QStringLiteral("规则推断("), QStringLiteral("系统推断("));
    text.replace(QStringLiteral("象限 "), QString());
    text.replace(QStringLiteral("综合得分"), QStringLiteral("综合评分"));

    text.replace(QStringLiteral("Q1"), QStringLiteral("重要且紧急"));
    text.replace(QStringLiteral("Q2"), QStringLiteral("重要不紧急"));
    text.replace(QStringLiteral("Q3"), QStringLiteral("紧急不重要"));
    text.replace(QStringLiteral("Q4"), QStringLiteral("不重要不紧急"));

    text.replace(QStringLiteral("重要且紧急 任务"), QStringLiteral("重要且紧急"));
    text.replace(QStringLiteral("重要且紧急任务"), QStringLiteral("重要且紧急"));
    text.replace(QStringLiteral("重要不紧急 任务"), QStringLiteral("重要不紧急"));
    text.replace(QStringLiteral("重要不紧急任务"), QStringLiteral("重要不紧急"));

    text.replace(QStringLiteral("次高"), QStringLiteral("较高"));
    text.replace(QStringLiteral("最高"), QStringLiteral("较高"));

    text.replace(QRegularExpression(QStringLiteral("\\s*,\\s*")), QStringLiteral("，"));
    text = text.simplified();
    while (text.contains(QStringLiteral("，，")))
        text.replace(QStringLiteral("，，"), QStringLiteral("，"));
    if (text.startsWith(QStringLiteral("，")))
        text.remove(0, 1);
    if (text.endsWith(QStringLiteral("，")))
        text.chop(1);

    return text.trimmed();
}

QString dailyEvaluationSystemPrompt()
{
    return QStringLiteral(
        "你是待办效率教练。根据用户提供的当日任务 JSON 数据，给出简洁、鼓励性的中文评估，"
        "所有 LLM 输出格式保持一致。\n\n"
        "【字段说明】\n"
        "planned_batch：当日首批计划任务；adhoc_tasks：距首批添加超过 1 小时的临时新增任务。\n"
        "需评价临时新增对当日整体计划与执行的影响。\n\n"
        "【输出要求】\n"
        "只输出一个 JSON 对象，包含以下字段：\n"
        "- summary：50 字以内总评，客观简洁\n"
        "- score：0-100 整数，反映当日完成度\n"
        "- task_plan_review：200 字以内，对当日任务清单的整体评价（含临时新增影响）\n"
        "- highlights：字符串数组，1～3 条今日亮点\n"
        "- improvements：字符串数组，1～3 条待改进点\n"
        "- tomorrow_tip：一句明日建议\n\n"
        "【写作风格】\n"
        "语气专业、客观、鼓励；只基于 JSON 数据归纳，不编造未提供的事实。\n"
        "不要 markdown，不要 ``` 代码围栏，不要附加解释或模型署名。\n\n"
        "【格式示例】\n"
        "{\"summary\":\"完成 5 项到期任务，执行节奏良好\","
        "\"score\":82,\"task_plan_review\":\"计划任务全部完成；临时新增 1 项已处理，"
        "对整体节奏影响较小。\",\"highlights\":[\"到期任务完成率 100%\"],"
        "\"improvements\":[\"控制计划外插入\"],\"tomorrow_tip\":\"优先处理遗留高优先级任务\"}");
}

QString weeklyReportSystemPrompt(LlmProviderType provider)
{
    QString prompt = QStringLiteral(
        "你是专业的工作周报写作助手。根据用户提供的任务 JSON（selected_tasks）撰写中文周报。"
        "必须严格遵循以下 Markdown 结构与写作风格，所有 LLM 输出格式保持一致。\n\n"
        "【固定章节】依次输出四个二级标题（##），不要使用一级标题 #：\n"
        "## 本周工作概述\n"
        "## 本周完成工作\n"
        "## 进行中 / 未完成事项\n"
        "## 下周工作计划\n\n"
        "【写作风格】\n"
        "1. 概述：1～3 段连贯中文，按主题归纳（如技术项目、个人健康、生活管理、习惯养成等），"
        "说明本周重心与整体进展，语气专业、客观、简洁。\n"
        "2. 完成工作：使用「1. **分类名**」编号 + 加粗分类标题，其下用「- 」条目列出具体事项；"
        "根据任务标题语义自行归纳 2～5 个合理分类，不要全部堆在一个分类下。"
        "selected_tasks 中每一项的 title 均须在本节条目中有所体现（可归类合并，不可遗漏）。\n"
        "3. 未完成：若有未完成/进行中任务，同样用「- 」条目列出；"
        "若所选任务均已在本周期内完成，写一句：「本周所有计划内的工作均已完成，没有未完成事项。」\n"
        "4. 下周计划：使用「1. **分类名**」编号 + 加粗分类，其下「- 」条目；"
        "结合未完成项与已完成项的延续性给出可执行安排，不要编造 JSON 中不存在的新项目。\n"
        "5. 只基于 selected_tasks 中的 title、completed、due_at、completed_at、quadrant 等信息归纳，"
        "可合理概括措辞，但不得虚构未提供的具体事实、数字或事件。\n"
        "6. 直接输出 Markdown 正文，不要 JSON，不要 ``` 代码围栏，不要附加解释或模型署名。\n"
        "7. 篇幅适中：概述 1～3 段；完成工作与下周计划各 2～5 个分类；勿冗长复述 JSON 字段或逐条分析任务。\n\n"
        "【格式示例（仅作结构参考，内容须替换为用户任务）】\n"
        "## 本周工作概述\n\n"
        "本周工作主要集中在……（按主题归纳的段落）。\n\n"
        "## 本周完成工作\n\n"
        "1. **技术项目**\n"
        "   - 完成了……\n"
        "   - 查看了……\n\n"
        "2. **个人健康**\n"
        "   - ……\n\n"
        "## 进行中 / 未完成事项\n\n"
        "本周所有计划内的工作均已完成，没有未完成事项。\n\n"
        "## 下周工作计划\n\n"
        "1. **技术项目**\n"
        "   - 继续优化……\n"
        "2. **个人健康**\n"
        "   - ……");

    if (provider == LlmProviderType::Ollama) {
        prompt += QStringLiteral(
            "\n\n【输出约束 — 必须遵守】\n"
            "1. 第一行必须是「## 本周工作概述」，此前不得有任何文字。\n"
            "2. 禁止输出思考过程、写作计划、JSON 解读、任务 id 计数、completed 字段说明、"
            "quadrant 说明或任何 meta 分析。\n"
            "3. 四个章节必须各自以「## 标题」单独成行开头，随后直接写正文。\n"
            "4. 不要复述本提示中的规则或示例；只输出周报 Markdown 正文。");
    }
    return prompt;
}

QString weeklyReportOverviewSystemPrompt()
{
    return QStringLiteral(
        "你是工作周报写作助手。根据用户提供的任务分类摘要，"
        "仅撰写「## 本周工作概述」及其下 1～3 段中文概述。\n\n"
        "要求：\n"
        "1. 第一行必须是「## 本周工作概述」，随后空一行再写正文。\n"
        "2. 只输出概述章节，不要输出「本周完成工作」「进行中」「下周工作计划」。\n"
        "3. 按技术项目、个人健康、生活管理等主题归纳，语气专业、客观、简洁。\n"
        "4. 禁止思考过程、JSON 解读、任务列表复述或写作计划。\n"
        "5. 不要 markdown 代码围栏，不要附加解释。");
}

QString weeklyReportOverviewUserPrompt(const QVector<TaskItem> &tasks, const QDate &weekStart,
                                       const QDate &weekEnd)
{
    QJsonObject root;
    root.insert(QStringLiteral("week_start"), weekStart.toString(Qt::ISODate));
    root.insert(QStringLiteral("week_end"), weekEnd.toString(Qt::ISODate));

    QJsonArray titles;
    QSet<QString> seen;
    for (const TaskItem &t : tasks) {
        if (seen.contains(t.title))
            continue;
        seen.insert(t.title);
        titles.append(t.title);
    }
    root.insert(QStringLiteral("task_titles"), titles);

    return QStringLiteral("请根据以下 JSON 撰写「## 本周工作概述」（1～3 段），不要输出其他章节。\n")
           + QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

QString weeklyReportUserInstruction(LlmProviderType provider)
{
    QString instruction = QStringLiteral(
        "请根据以下 JSON 中的 selected_tasks 撰写本周周报，严格遵循系统提示中的章节结构与写作风格。");
    if (provider == LlmProviderType::Ollama) {
        instruction += QStringLiteral(
            " 直接输出 Markdown 周报正文，第一行必须是「## 本周工作概述」。"
            "禁止输出分析过程、任务计数或 JSON 字段说明；四个 ## 章节须各自独立成行。");
    }
    return instruction + QStringLiteral("\n");
}

int quadrantMaxTokens(LlmProviderType provider, int taskCount)
{
    if (provider == LlmProviderType::Ollama)
        return qBound(128, taskCount * 48 + 64, 384);
    return 1024;
}

LlmOutputFormat quadrantOutputFormat(LlmProviderType provider)
{
    return provider == LlmProviderType::Ollama ? LlmOutputFormat::JsonQuadrantArray
                                               : LlmOutputFormat::Default;
}

int top3MaxTokens(LlmProviderType provider)
{
    return provider == LlmProviderType::Ollama ? 384 : 512;
}

LlmOutputFormat top3OutputFormat(LlmProviderType provider)
{
    return provider == LlmProviderType::Ollama ? LlmOutputFormat::JsonTop3Array
                                               : LlmOutputFormat::Default;
}

int dailyEvalMaxTokens(LlmProviderType provider)
{
    return provider == LlmProviderType::Ollama ? 768 : 1024;
}

double dailyEvalTemperature()
{
    return 0.35;
}

int weeklyReportMaxTokens(LlmProviderType provider)
{
    return provider == LlmProviderType::Ollama ? 2048 : 2560;
}

double weeklyReportTemperature()
{
    return 0.35;
}

double weeklyReportOllamaTemperature()
{
    return 0.2;
}

int weeklyReportOverviewMaxTokens()
{
    return 512;
}

int weeklyReportOllamaTimeoutMs()
{
    return 600000;
}

int quadrantBatchSize(LlmProviderType provider)
{
    return provider == LlmProviderType::Ollama ? 5 : 10000;
}

QString quickCaptureSystemPrompt()
{
    return QStringLiteral(
        "你是待办任务速记解析器。用户会输入一段自然语言、清单或会议纪要片段，"
        "你需要拆成 1～20 条可执行待办。\n\n"
        "【输出要求】\n"
        "1. 只输出 JSON 数组，每项含 title、due、quadrant、notes 四个字段。\n"
        "2. title 为简洁中文任务标题，不要编号前缀。\n"
        "3. due 使用 ISO 8601 本地时间，如 2026-07-11T15:00:00；无明确时间则当天 23:59:00。\n"
        "4. quadrant 为 0～4：0=不确定，1=Q1，2=Q2，3=Q3，4=Q4；不确定时用 0。\n"
        "5. notes 可留空字符串。\n"
        "6. 不要 markdown，不要 ``` 代码围栏，不要附加解释。\n\n"
        "【格式示例】\n"
        "[{\"title\":\"完成月报\",\"due\":\"2026-07-11T15:00:00\",\"quadrant\":2,\"notes\":\"\"}]");
}

QString quickCaptureUserPrompt(const QString &text)
{
    const QString now = QDateTime::currentDateTime().toString(Qt::ISODateWithMs);
    QJsonObject root;
    root.insert(QStringLiteral("now"), now);
    root.insert(QStringLiteral("text"), text);
    return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

int quickCaptureMaxTokens(LlmProviderType provider)
{
    return provider == LlmProviderType::Ollama ? 1024 : 768;
}

LlmOutputFormat quickCaptureOutputFormat(LlmProviderType provider)
{
    return provider == LlmProviderType::Ollama ? LlmOutputFormat::JsonQuickCaptureArray
                                               : LlmOutputFormat::Default;
}

int quickCaptureTimeoutMs(LlmProviderType provider)
{
    return provider == LlmProviderType::Ollama ? 90000 : 30000;
}

} // namespace AiPrompts
