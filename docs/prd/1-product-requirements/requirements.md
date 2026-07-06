# ToDoList — 产品需求文档

> 版本 v0.1 · 来源：`docs/todo_ai_analysis.docx` · Phase 01

## 一句话定位

面向 Windows 知识工作者的 **AI 原生待办应用**：Eisenhower 四象限 + RICE 规则评分 + LLM 动态优先级，默认本地 Qwen（Ollama），预留 DeepSeek/Kimi 等云端 API。

## 用户画像

- **小陈（28 岁，产品经理）**：每日 20+ 待办，需快速判断「今天先做哪 3 件」；希望 AI 解释推荐理由，而非黑盒自动排程。
- **李工（35 岁，开发/咨询）**：处理敏感客户数据，优先本地 LLM；网络可用时可选 DeepSeek 增强复杂推理。
- **Windows 重度用户**：期望系统托盘、快捷键、通知（V1.0）；MVP 先完成桌面主窗口核心流程。

## 痛点

| ID | 痛点链 |
|----|--------|
| P01 | 任务多且优先级标签静态（高/中/低）→ 无法随截止日期、业务语境动态调整 → 常误做低价值任务 |
| P02 | 竞品 Motion 全自动但贵且不可解释；Todoist/MS To Do AI 浅 → 缺少「可解释 + 本地隐私」中间地带 |
| P03 | 云端 LLM 有隐私与成本顾虑；纯本地小模型能力有限 → 需要混合架构与统一接口 |
| P04 | Windows 原生集成不足（托盘/快捷键）→ 切换成本高（V1.0 解决，MVP 预留） |

## 场景

### S01 — 任务创建与 Eisenhower 四象限管理

用户打开主窗口，快速添加任务（标题、截止日期、重要/紧急标签），在四象限视图中拖拽或切换象限，任务持久化到本地 SQLite。

**主路径**：启动 → 添加任务 → 设置截止日期/象限 → 列表刷新 → 重启后数据仍在。

#### 验收标准

- AC-S01-1：点击「添加任务」后 500ms 内任务出现在列表，且写入 SQLite。
- AC-S01-2：修改 Eisenhower 象限后 300ms 内 UI 与数据库一致。
- AC-S01-3：应用重启后 2 秒内加载全部历史任务，数量与关闭前一致。
- AC-S01-EX1：空标题提交时，不创建任务并提示「标题不能为空」。

### S02 — AI 动态优先级评分与 Top 3 推荐

用户点击「AI 分析优先级」，系统将任务列表（规则层 RICE-like 分数 + LLM 增强）异步计算，展示 Top 3 及每条推荐理由；LLM 不可用时降级为纯规则评分。

**主路径**：选择/加载任务 → 触发 AI 分析 → Worker 线程调用 LlmProvider → 主线程展示 Top 3 + 理由。

#### 验收标准

- AC-S02-1：50 条任务内，规则层评分在 200ms 内完成（不含 LLM）。
- AC-S02-2：本地 Ollama/Qwen 可用时，AI 分析完成后 5 秒内展示 Top 3（含理由文本）。
- AC-S02-3：每条推荐必须显示可读的「推荐理由」（≥10 字）。
- AC-S02-EX1：Ollama 未启动时，显示警告并仅展示规则层 Top 3，不崩溃。

### S03 — LLM 提供商配置与切换

用户在设置中选择 Provider：本地 Ollama（Qwen）、DeepSeek、Kimi（Moonshot）；配置 API Key/Base URL/模型名；测试连接；选择结果持久化到 QSettings。

**主路径**：打开设置 → 选择 Provider → 填写参数 → 测试连接 → 保存 → 后续 S02 使用新 Provider。

#### 验收标准

- AC-S03-1：支持 Provider 枚举：Ollama、DeepSeek、Kimi、CustomOpenAI。
- AC-S03-2：切换 Provider 并保存后，下次启动仍生效。
- AC-S03-3：DeepSeek/Kimi 未配置 API Key 时，测试连接返回明确错误，不发起无效请求。
- AC-S03-EX1：Ollama 连接失败时，错误信息包含主机与端口提示。

### S04 — 系统托盘与全局快捷键

用户关闭主窗口后应用驻留系统托盘；`Win+Shift+T` 快速添加任务，`Win+Shift+P` 弹出 Top 3 推荐窗口。

#### 验收标准

- AC-S04-1：关闭主窗口后应用不退出，托盘图标可见。
- AC-S04-2：`Win+Shift+T` 在任何前台应用下唤起快速添加对话框。
- AC-S04-3：`Win+Shift+P` 弹出 Top 3 窗口（优先上次 AI 结果，否则规则层）。

## 约束

- Qt 5.15.2 + Widgets + qmake + MinGW（Windows）
- 离线可用：任务 CRUD、规则优先级、Ollama 本地推理
- LLM 调用异步，主线程 UI 冻结 < 100ms
- 所有云端 API 经 HTTPS（Qt Network）

## 不做清单（MVP）

- Microsoft 365 / Outlook 集成
- 移动端同步
- 团队协作与 SSO
- Motion 式全自动日程重排

## 成功指标（MVP）

- S01+S02+S03 验收标准全部可测
- 本地 Qwen 可完成 Top 3 推荐
- DeepSeek/Kimi 接口类已实现并可通过 Mock/配置接入
