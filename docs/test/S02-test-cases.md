## UT-S02-01 — 规则评分 200ms 内

**输入**：50 条 TaskItem  
**预期**：applyRuleScores < 200ms

## UT-S02-02 — LLM 失败降级

**输入**：Mock Provider 返回 error  
**预期**：top3 来自规则层，usedLlm=false
