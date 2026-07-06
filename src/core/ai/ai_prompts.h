#ifndef AI_PROMPTS_H
#define AI_PROMPTS_H

#include "../task_types.h"

namespace AiPrompts {

// ---- 系统提示（所有 Provider 共用） ----
QString quadrantClassificationSystemPrompt();
QString top3SystemPrompt();
QString dailyEvaluationSystemPrompt();
QString weeklyReportSystemPrompt(LlmProviderType provider = LlmProviderType::Kimi);
QString weeklyReportUserInstruction(LlmProviderType provider);
QString weeklyReportOverviewSystemPrompt();
QString weeklyReportOverviewUserPrompt(const QVector<TaskItem> &tasks, const QDate &weekStart,
                                       const QDate &weekEnd);

// ---- 请求参数（仅 token / format 按 Provider 微调） ----
int quadrantMaxTokens(LlmProviderType provider, int taskCount);
LlmOutputFormat quadrantOutputFormat(LlmProviderType provider);

int top3MaxTokens(LlmProviderType provider);
LlmOutputFormat top3OutputFormat(LlmProviderType provider);

int dailyEvalMaxTokens(LlmProviderType provider);
double dailyEvalTemperature();

int weeklyReportMaxTokens(LlmProviderType provider);
double weeklyReportTemperature();
double weeklyReportOllamaTemperature();
int weeklyReportOverviewMaxTokens();
int weeklyReportOllamaTimeoutMs();

int quadrantBatchSize(LlmProviderType provider);

} // namespace AiPrompts

#endif // AI_PROMPTS_H
