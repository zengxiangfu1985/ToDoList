# AI Assistant Instructions — ToDoList

本项目遵循 **Qt 项目规格链**（`docs/Qt项目规格链.md`）。开始工作前读取 `qt-project.yaml` 的 `resource_index` 与 `tech_stack`。

## 项目上下文

| 项 | 值 |
|----|-----|
| 配置索引 | `qt-project.yaml` |
| 构建 | `ToDoList.pro`（qmake） |
| UI | Qt Widgets 5.15 |
| 数据库 | SQLite |
| AI | 本地 Ollama/Qwen + 预留 DeepSeek/Kimi（OpenAI 兼容 API） |
| Qt Skills | `Qtskills/` → `.cursor/skills/` |

## 规格链 9 阶段

```
需求 → 原型 → 场景 → 接口 → 数据 → 测试 → 代码 → 验收 → 变更
 01     02     03     04     05      06     07     08     09
```

| Phase | 产物路径 | Skill |
|-------|---------|-------|
| 01 | `docs/prd/1-product-requirements/requirements.md` | qt-prd-writer |
| 02 | `docs/prd/2-product-design/` | qt-product-designer |
| — | `docs/prd/3-technical-plan/1-architecture/` | qt-architecture-designer |
| 03 | `docs/prd/3-technical-plan/2-scenario-implementation/` | qt-scenario-architect |
| 04 | `docs/api/*.h` | qt-api-designer |
| 05 | `docs/database/` | qt-db-designer |
| 06 | `docs/test/` | qt-test-writer |
| 07 | `src/` + `tests/` | qt-code-implementor |
| 08 | `docs/verify/` | qt-acceptance-verifier |
| 09 | `docs/changes/` | qt-change-writer |

## Phase 检测

| 条件 | 建议 Phase |
|------|-----------|
| 无 `requirements.md` | Phase 01 |
| 无产品设计/原型 | Phase 02 |
| 无时序图 | Phase 03 |
| 无 `docs/api/*.h` | Phase 04 |
| 无 DDL | Phase 05 |
| 无测试用例文档 | Phase 06 |
| 无业务实现 | Phase 07 |

## AI 模块约束

- 所有 LLM 调用经 `LlmProvider` 接口，由 `LlmService` 在 Worker 线程调度
- 新增云端模型（DeepSeek/Kimi）仅实现 `OpenAiCompatibleProvider` 子类或在 Factory 注册
- 本地 Qwen 默认经 `OllamaLlmProvider`（`http://127.0.0.1:11434`）
