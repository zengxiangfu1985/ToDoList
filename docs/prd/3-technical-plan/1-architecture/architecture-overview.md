# 架构概览 — ToDoList

> Phase 04 前置 · 模块分层与 AI 可插拔设计

## 分层

```
┌─────────────────────────────────────────┐
│  ui/     MainWindow, Dialogs            │
├─────────────────────────────────────────┤
│  models/ TaskTableModel                 │
├─────────────────────────────────────────┤
│  core/   TaskRepository, PriorityEngine │
│          LlmService + LlmProvider*      │
├─────────────────────────────────────────┤
│  utils/  (预留)                          │
└─────────────────────────────────────────┘
         │ SQLite          │ HTTP (Worker)
         ▼                 ▼
      tasks.db         Ollama / DeepSeek / Kimi
```

## AI Provider 抽象

| 类 | 职责 |
|----|------|
| `LlmProvider` | 抽象接口：`complete()`, `providerType()`, `isAvailable()` |
| `OllamaLlmProvider` | 本地 Qwen，Ollama `/api/chat` |
| `OpenAiCompatibleProvider` | OpenAI 兼容 REST；DeepSeek/Kimi/Custom 共用 |
| `LlmProviderFactory` | 从 `LlmConfig` 创建实例 |
| `LlmService` | QObject 门面，Worker 线程异步 `analyzePriorities()` |

## 线程模型

- **主线程**：UI、TaskRepository 同步读写（SQLite 单连接，MVP 量级可接受）
- **LlmWorker**：`QThread` + `LlmProvider::complete` 网络/本地 HTTP
- 跨线程：`LlmService::analysisFinished` 使用 `Qt::QueuedConnection`

## NFR

- UI 响应：AI 请求不阻塞主线程
- 隐私：默认 Provider = Ollama；API Key 存 QSettings，不写日志
- 可扩展：新增 Provider 仅注册 Factory，不改 PriorityEngine

## MVP 范围

- 实现：Ollama + OpenAiCompatible（DeepSeek/Kimi 预设 URL）
- 预留：S04 托盘 `QSystemTrayIcon` 占位注释
