# ToDoList API 规格

## 线程约定

| 类 | 线程 | 说明 |
|----|------|------|
| TaskRepository | 主线程 | 同步 SQLite |
| PriorityEngine | 任意 | 无状态纯计算 |
| LlmProvider | Worker | HTTP 在 LlmService 线程 |
| LlmService | 主线程创建 | 内部 QThread 执行 complete |
| TaskTableModel | 主线程 | 绑定 View |

## 错误处理

- `TaskRepository`：`bool` + `QString *errorMsg`
- `LlmProvider`：`LlmResponse.success` + `errorMessage`
- UI 层：QMessageBox 或 statusBar

## 文件清单

- `task_types.h` — 共享类型
- `task_repository.h`
- `priority_engine.h`
- `llm_provider.h`
- `llm_service.h`
