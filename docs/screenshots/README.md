# README 截图清单

将截图保存为本目录下的 PNG 文件，文件名与 [README.md](../../README.md) 中引用一致，提交到 `main` 分支后 GitHub 会自动显示。

## 自动截取（推荐）

```bat
scripts\capture-screenshots.bat
```

或：

```bash
pip install -r scripts/requirements-screenshots.txt
python scripts/capture-readme-screenshots.py --attach
```

已手动启动 ToDoList 时请加 **`--attach`**，脚本会附着到当前进程，不会再次启动或关闭应用。

常用参数：

| 参数 | 说明 |
|------|------|
| `--attach` | 附着已运行的 ToDoList（推荐：应用已提前打开时使用） |
| `--pid 12345` | 指定要附着的进程 PID |
| `--only 02,07` | 只截指定编号 |
| `--list` | 列出全部 14 项任务 |
| `--ollama-test` | 第 14 图尝试 Ollama 测试连接 |
| `--tray-wait 12` | 托盘菜单等待秒数（默认 8） |

脚本会复制程序到临时目录并写入演示数据，**不会修改你正在使用的 `data/`**。主窗口截图前会**自动最大化**；窗口定位按 **ToDoList.exe 进程**过滤，避免误截 IDE。第 01 图在启动应用前截取；第 09 图请在倒计时内右键托盘，检测到菜单后**立即**截取。

## 已有（01–04）

| 文件 | 内容 |
|------|------|
| `01-quick-start.png` | 便携版解压目录 |
| `02-main-window.png` | 主界面全貌 |
| `03-add-task.png` | 添加任务对话框 |
| `04-today-tasks.png` | 今日任务批量录入 |

## 待补充（05–14）

| 文件 | 建议截取内容 |
|------|----------------|
| `05-quick-capture.png` | 闪记居中小窗（含 AI 拆分结果） |
| `06-focus-25.png` | Focus 25 倒计时对话框 |
| `07-ai-analyze.png` | AI 分析后的四象限与 Top 3 |
| `08-top3-popup.png` | Top 3 弹窗（`Alt+Shift+3`） |
| `09-tray-menu.png` | 托盘右键菜单（含快捷键标注） |
| `10-settings.png` | 设置对话框（快捷键 + Focus 25） |
| `11-about-update.png` | 关于 · 检查更新 |
| `12-daily-evaluation.png` | 每日评估历史（含专注统计列） |
| `13-llm-settings.png` | LLM 设置对话框 |
| `14-llm-ollama.png` | Ollama 本地配置（测试连接成功） |

## 截取建议

- 分辨率：宽度 1280–1920 px，PNG 格式
- 使用浅色/深色主题任选一种，与 README 其他图保持一致
- 敏感信息（API Key、真实任务标题）请打码或改用示例数据
