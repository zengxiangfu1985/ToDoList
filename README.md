# ToDoList — AI 智能待办

> **不是又一个清单 App。**  
> 艾森豪威尔四象限 × 本地/云端大模型 × Top 3 决策 × 番茄专注 —— 一套专为 Windows 设计的 **AI 驱动工作流**，帮你从「任务堆积」走到「知道先做哪三件」。

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Windows%2010%2F11-blue" alt="Windows">
  <img src="https://img.shields.io/badge/运行方式-绿色便携版-success" alt="Portable">
  <img src="https://img.shields.io/badge/技术栈-Qt%205.15%20·%20C%2B%2B17-informational" alt="Tech">
  <img src="https://img.shields.io/badge/License-Apache%202.0-green" alt="License">
  <a href="https://github.com/zengxiangfu1985/ToDoList/releases"><img src="https://img.shields.io/github/v/release/zengxiangfu1985/ToDoList?label=Release" alt="Release"></a>
</p>

<p align="center">
  <a href="https://github.com/zengxiangfu1985/ToDoList/releases"><strong>⬇️ 下载最新便携版</strong></a>
  &nbsp;·&nbsp;
  <a href="#快速开始">快速开始</a>
  &nbsp;·&nbsp;
  <a href="#核心亮点">功能亮点</a>
  &nbsp;·&nbsp;
  <a href="#模型llm配置">配置 AI</a>
</p>

---

## 和常见待办工具有什么不同？

| | 普通待办 / Notion / Todoist | **ToDoList** |
|---|:---:|:---:|
| 艾森豪威尔四象限看板 | 需自己搭模板 | ✅ **内置，拖拽可调** |
| AI 自动划分优先级 | 少见或需插件 | ✅ **一键分析，带推荐理由** |
| 今日只做哪 3 件 | 靠自觉 | ✅ **AI Top 3 + 分数排序** |
| 自然语言快速录入 | 多为单条标题 | ✅ **闪记：一句话拆多条任务** |
| 专注与待办联动 | 另开番茄钟 App | ✅ **Focus 25 从 Top 3 一键启动** |
| 每日复盘 | 手动写总结 | ✅ **23:59 自动生成 AI 每日评估** |
| 本地大模型 | 几乎不支持 | ✅ **Ollama 离线可用，数据不出本机** |
| LLM 挂了还能用 | — | ✅ **规则层降级，待办不中断** |
| Windows 原生 + 便携 | 多为 Electron / 网页 | ✅ **Qt 原生，解压即用，`data/` 可迁移** |

**一句话：** 别人帮你记任务，ToDoList 帮你 **决定优先级、锁定今日重点、专注执行、复盘改进**。

---

## 核心亮点

### 🧠 AI 分析优先级 —— 不只打分，还告诉你「为什么」

添加任务后点击 **AI 分析优先级**，大模型会：

- 自动将任务划入 **Q1～Q4 四象限**
- 生成 **Top 3 推荐列表** 与优先级分数
- 为每条推荐附上 **可读的理由**（点击即可查看）

支持 **Ollama 本地模型**（推荐 `qwen2.5:3b`）或 **DeepSeek / Kimi / OpenAI 兼容 API**。无网络或无 API 时，程序自动 **降级为规则层评分**，基本功能不中断。

> 📷 **截图占位** · [`docs/screenshots/07-ai-analyze.png`](docs/screenshots/07-ai-analyze.png)  
> *AI 分析完成：四象限看板 + Top 3 推荐 + 推荐理由*

---

### 📊 艾森豪威尔四象限 —— 重要/紧急，一眼看清

| 象限 | 含义 | 行动 |
|------|------|------|
| **Q1** 重要且紧急 | 立刻处理 | 今日必做 |
| **Q2** 重要不紧急 | 计划安排 | 长期价值 |
| **Q3** 紧急不重要 | 快速处理或委托 | 少花时间 |
| **Q4** 不重要不紧急 | 可延后 | 考虑删除 |

任务可在四象限间 **拖拽调整**；列表、象限、Top 3 **三处勾选状态实时同步**。

> 📷 **截图占位** · [`docs/screenshots/02-main-window.png`](docs/screenshots/02-main-window.png)  
> *主界面：任务列表 + 四象限看板 + AI Top 3*

---

### ⚡ 闪记 —— 按快捷键，说一句话，AI 帮你拆任务

`Alt+Shift+N` 唤起居中小窗，输入自然语言即可：

```
明天 9 点开会，之后写周报，晚上健身
```

保存后 AI 自动拆成多条任务，并可选择 **保存后立即分析优先级**。适合走路、开会间隙的 **零摩擦录入**。

> 📷 **截图占位** · [`docs/screenshots/05-quick-capture.png`](docs/screenshots/05-quick-capture.png)  
> *闪记居中小窗：自然语言 → 多条任务*

---

### 🍅 Focus 25 —— 从 Top 3 直接开始专注

不是孤立的番茄钟，而是和 **今日重点** 绑在一起：

- 在 Top 3 选中任务 → `Alt+Shift+F` 或点击 **Focus 25**
- 支持 **15 / 25 / 50 分钟** 可配置
- 结束后可选 **完成 / 再来一轮 / 跳过**
- 专注数据纳入 **每日评估**（专注分钟 · 完成轮数）

> 📷 **截图占位** · [`docs/screenshots/06-focus-25.png`](docs/screenshots/06-focus-25.png)  
> *Focus 25 专注倒计时（右下角极简浮窗）*

---

### 📅 每日 AI 评估 + 工作周报

- **每日评估**：每天 23:59 自动生成当日总结（含任务完成率 + 专注统计）
- **生成周报**：勾选本周任务，一键生成 AI 工作周报
- 启动时自动补评遗漏日期

> 📷 **截图占位** · [`docs/screenshots/12-daily-evaluation.png`](docs/screenshots/12-daily-evaluation.png)  
> *每日评估历史：任务统计 + 专注 X 分 · Y/Z 轮*

---

### ⌨️ 全局快捷键 + 托盘 —— 不打开主窗口也能干活

| 快捷键 | 功能 |
|--------|------|
| `Alt+Shift+J` | 今日任务批量录入 |
| `Alt+Shift+N` | 闪记（AI 拆任务） |
| `Alt+Shift+3` | Top 3 弹窗 |
| `Alt+Shift+F` | Focus 25 专注 |

关闭主窗口默认 **最小化到托盘**，右键菜单可快速添加任务、查看 Top 3、启动专注。

> 📷 **截图占位** · [`docs/screenshots/09-tray-menu.png`](docs/screenshots/09-tray-menu.png)  
> *系统托盘右键菜单*

---

## 截图预览

| | |
|:---:|:---:|
| 📷 [`01-quick-start.png`](docs/screenshots/01-quick-start.png)<br>*便携版目录，解压即用* | 📷 [`04-today-tasks.png`](docs/screenshots/04-today-tasks.png)<br>*今日任务批量录入* |
| 📷 [`03-add-task.png`](docs/screenshots/03-add-task.png)<br>*添加任务* | 📷 [`08-top3-popup.png`](docs/screenshots/08-top3-popup.png)<br>*Top 3 弹窗* |
| 📷 [`13-llm-settings.png`](docs/screenshots/13-llm-settings.png)<br>*LLM 设置* | 📷 [`10-settings.png`](docs/screenshots/10-settings.png)<br>*设置（快捷键 / Focus）* |

> 将 PNG 保存到 [`docs/screenshots/`](docs/screenshots/) 并 push 后，上表占位会自动变为可点击预览。完整清单见 [`docs/screenshots/README.md`](docs/screenshots/README.md)。

---

## 快速开始

```text
1. 打开 Releases 下载 ToDoList-Portable-x.y.z.zip
2. 解压到任意目录（桌面、U 盘均可）
3. 双击 ToDoList.exe 或 启动 ToDoList.bat
4. 添加任务 → 设置 LLM（可选）→ AI 分析优先级 → 查看 Top 3
```

**[⬇️ 前往 Releases 下载](https://github.com/zengxiangfu1985/ToDoList/releases)**

> 📷 **截图占位** · [`docs/screenshots/01-quick-start.png`](docs/screenshots/01-quick-start.png)  
> *解压后的便携版目录（含 `ToDoList.exe`、`data/`、`使用说明.txt`）*

| 项目 | 说明 |
|------|------|
| 平台 | Windows 10 / 11（64 位） |
| 运行方式 | 绿色免安装便携版 |
| 数据目录 | 程序同级 `data/`（复制整个文件夹即可迁移备份） |

---

## 模型（LLM）配置

**入口：** 工具栏 → **设置 LLM**

| 提供商 | 场景 | 默认模型 |
|--------|------|----------|
| **Ollama（本地）** | 离线 / 隐私优先 | `qwen2.5:3b` |
| **DeepSeek** | 云端 API | `deepseek-chat` |
| **Kimi (Moonshot)** | 云端 API | `moonshot-v1-8k` |
| **Custom OpenAI** | 任意兼容接口 | `gpt-4o-mini` |

**本地 Ollama 三步上手：**

```bash
# 1. 安装 Ollama 并拉取模型
ollama pull qwen2.5:3b

# 2. ToDoList → 设置 LLM → 提供商选 Ollama
#    Base URL: http://127.0.0.1:11434  模型: qwen2.5:3b  API Key 留空

# 3. 测试连接 → 保存 → 点击「AI 分析优先级」
```

> 📷 **截图占位** · [`docs/screenshots/13-llm-settings.png`](docs/screenshots/13-llm-settings.png)  
> *LLM 设置对话框*

> 📷 **截图占位** · [`docs/screenshots/14-llm-ollama.png`](docs/screenshots/14-llm-ollama.png)  
> *Ollama 本地配置（测试连接成功）*

<details>
<summary><strong>云端 API 配置与常见问题</strong></summary>

1. 在 DeepSeek / Kimi / OpenAI 等平台创建 API Key  
2. **设置 LLM** 中选择对应提供商，填写 Base URL、Key、模型名  
3. **测试连接** → **保存**

| 现象 | 处理 |
|------|------|
| 测试连接失败 | 检查 Key、Base URL、模型名与网络 |
| Ollama 连不上 | 确认服务已启动，浏览器访问 `http://127.0.0.1:11434` |
| 分析较慢 | 换更小本地模型，或改用云端 API |
| 无 AI 仍可用 | 自动降级规则层 Top 3，待办功能不受影响 |

</details>

---

## 更多功能

<details>
<summary><strong>任务管理</strong></summary>

- **单条添加**：标题、截止时间、备注，象限可选手动或交给 AI  
- **今日任务**（`Alt+Shift+J`）：批量录入、编辑、删除，支持导入昨日未完成  
- **双击**编辑，**勾选**完成（列表 / 四象限 / Top 3 同步）  
- **历史记录**：每日快照与过期归档  

</details>

<details>
<summary><strong>在线更新</strong></summary>

**关于 → 检查更新**：自动拉取清单、下载 zip、SHA256 校验后一键升级，`data/` 目录保留不动。  
也可 **导入离线更新包**（`.zip`）手动升级。

> 📷 **截图占位** · [`docs/screenshots/11-about-update.png`](docs/screenshots/11-about-update.png)

</details>

<details>
<summary><strong>隐私与数据</strong></summary>

**匿名使用统计**（默认开启，可在设置中关闭）：仅上报版本、系统、匿名 install_id，**不含任务内容与 API Key**。

| 路径 | 内容 |
|------|------|
| `data/tasks.db` | 任务、专注会话、每日评估 |
| `data/settings.ini` | 程序与 LLM 配置 |
| `data/top3-YYYY-MM-DD.json` | 当日 Top 3 缓存 |

</details>

---

## 技术栈

- **UI**：Qt 5.15 · C++17 · 深色 Cyber 主题  
- **存储**：SQLite（`data/tasks.db`）  
- **AI**：OpenAI 兼容 API + Ollama；规则层兜底  
- **更新**：`ToDoListUpdater.exe` 便携替换，保留用户数据  

---

## 参与与反馈

- 🐛 [提交 Issue](https://github.com/zengxiangfu1985/ToDoList/issues) — Bug、功能建议  
- ⭐ 如果 ToDoList 帮你减少了「今天先做啥」的焦虑，欢迎 Star 支持  

---

## 许可证

本项目采用 [Apache License 2.0](LICENSE) 发布。  
第三方组件（Qt、OpenSSL 等）遵循各自许可证，详见 [NOTICE](NOTICE)。
