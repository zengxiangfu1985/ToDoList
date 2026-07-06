# 场景总览

| ID | 名称 | 核心类 | 状态 |
|----|------|--------|------|
| S01 | 任务创建与四象限 | TaskRepository, TaskTableModel, MainWindow | MVP |
| S02 | AI 优先级 Top 3 | PriorityEngine, LlmService | MVP |
| S03 | LLM Provider 配置 | LlmProviderFactory, LlmSettingsDialog | MVP |
| S04 | 托盘/快捷键 | WindowsHotkeyManager, QSystemTrayIcon | ✅ 已实现 |

## 信号总线

| 信号 | 发射者 | 订阅者 |
|------|--------|--------|
| `tasksChanged()` | TaskRepository | TaskTableModel |
| `analysisStarted()` | LlmService | MainWindow |
| `analysisFinished(PriorityResult)` | LlmService | MainWindow |
| `providerChanged(LlmProviderType)` | LlmSettingsDialog | LlmService, statusBar |
