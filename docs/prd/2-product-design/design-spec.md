# ToDoList — 产品设计规格

> Phase 02 · 技术路线：Qt Widgets 5.15

## 主窗口布局

```
┌─────────────────────────────────────────────────────────────┐
│ 菜单：文件 | 任务 | AI | 设置                                  │
├──────────────────────────┬──────────────────────────────────┤
│ 任务列表 (QTableView)     │  Eisenhower 四象限 (2×2 Grid)      │
│ - 标题 / 截止 / 分数      │  Q1 重要且紧急 | Q2 重要不紧急       │
│ - 勾选完成               │  Q3 紧急不重要 | Q4 不重要不紧急     │
├──────────────────────────┴──────────────────────────────────┤
│ AI 面板：Top 3 推荐 (QListWidget) + 推荐理由 (QTextBrowser)    │
│ [添加任务] [AI 分析优先级] [设置 LLM]          状态栏：Provider │
└─────────────────────────────────────────────────────────────┘
```

## 控件 objectName 契约

| objectName | 类型 | 信号 | 说明 |
|------------|------|------|------|
| `tableTasks` | QTableView | selectionChanged | 任务列表 |
| `btnAddTask` | QPushButton | clicked | 打开添加对话框 |
| `btnAiAnalyze` | QPushButton | clicked | 触发 S02 |
| `btnLlmSettings` | QPushButton | clicked | 打开 LLM 设置对话框 |
| `listTop3` | QListWidget | itemClicked | Top 3 列表 |
| `textAiReason` | QTextBrowser | — | 推荐理由详情 |
| `labelProviderStatus` | QLabel | — | 当前 Provider 状态 |
| `quadrantQ1`..`Q4` | QGroupBox | — | 四象限容器 |

## 对话框

### AddTaskDialog

- `editTitle` (QLineEdit)
- `dateDue` (QDateEdit)
- `comboQuadrant` (QComboBox: Q1-Q4)
- `btnOk` / `btnCancel`

### LlmSettingsDialog

- `comboProvider` (Ollama / DeepSeek / Kimi / Custom)
- `editBaseUrl`, `editApiKey`, `editModel`
- `btnTestConnection`, `btnSave`

## 交互规格

1. **添加任务**：模态对话框，确认后 `TaskRepository::addTask`，刷新 Model。
2. **AI 分析**：按钮禁用 + 状态栏「分析中…」，`LlmService` 完成后恢复。
3. **设置 LLM**：保存至 QSettings `ai/provider`, `ai/baseUrl`, `ai/apiKey`, `ai/model`。

## 页面级验收

- PD-01：主窗口最小尺寸 960×640，四象限与列表同屏可见。
- PD-02：所有按钮有 tooltip 与 tr() 中文文案。
