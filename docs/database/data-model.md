# 数据模型

## TaskItem ↔ tasks 表

| 字段 | 列名 | 类型 |
|------|------|------|
| id | id | INTEGER PK |
| title | title | TEXT |
| dueAt | due_at | ISO8601 TEXT |
| quadrant | quadrant | INTEGER 1-4 |
| completed | completed | 0/1 |
| ruleScore | rule_score | REAL |
| aiRank | ai_rank | INTEGER |
| aiReason | ai_reason | TEXT |
| createdAt | created_at | TEXT |
| updatedAt | updated_at | TEXT |

## QSettings 键（ai/）

| 键 | 说明 | 默认 |
|----|------|------|
| ai/provider | int (LlmProviderType) | 0 Ollama |
| ai/baseUrl | QString | http://127.0.0.1:11434 |
| ai/apiKey | QString | 空 |
| ai/model | QString | qwen2.5:3b |

## 数据库路径

`QStandardPaths::AppDataLocation/tasks.db`

## 线程

MVP：TaskRepository 主线程单连接；后续可迁 Worker + QueuedConnection。
