#!/usr/bin/env python3
"""Fill English translations in translations/todolist_en.ts from TRANSLATIONS dict."""

import re
import sys
from pathlib import Path

TS_PATH = Path(__file__).resolve().parent.parent / "translations" / "todolist_en.ts"

TRANSLATIONS = {
    "添加任务": "Add Task",
    "标题": "Title",
    "截止日期": "Due Date",
    "象限": "Quadrant",
    "确定": "OK",
    "取消": "Cancel",
    "不确定 (由 AI 划分)": "Undecided (AI classified)",
    "Q1 重要且紧急": "Q1 Urgent & Important",
    "Q2 重要不紧急": "Q2 Important, Not Urgent",
    "Q3 紧急不重要": "Q3 Urgent, Not Important",
    "Q4 不重要不紧急": "Q4 Not Urgent, Not Important",
    "编辑任务": "Edit Task",
    "校验": "Validation",
    "标题不能为空": "Title cannot be empty",
    "AI 分析过程": "AI Analysis Trace",
    "象限: %1 | Top3: %2": "Quadrant: %1 | Top3: %2",
    "AI 划分": "AI classified",
    "规则推断": "Rule inferred",
    "AI 推荐": "AI recommended",
    "规则层": "Rule layer",
    " | 时间: %1": " | Time: %1",
    "打开日志文件": "Open Log File",
    "关闭": "Close",
    "分析中请稍后": "Analyzing, please wait…",
    "设置": "Settings",
    "appDialog": "appDialog",
    "安全": "Security",
    "当前密码": "Current Password",
    "修改密码时填写": "Required when changing password",
    "新密码": "New Password",
    "至少 4 位，留空表示不修改": "At least 4 characters; leave blank to keep unchanged",
    "确认新密码": "Confirm New Password",
    "系统锁屏后锁定": "Lock after system screen lock",
    " 分钟": " min",
    "0 = 立即锁定": "0 = lock immediately",
    "空闲自动锁定": "Auto-lock when idle",
    "0 = 不启用": "0 = disabled",
    "启动时需密码": "Require password at startup",
    "程序启动时要求输入密码": "Prompt for password when the app starts",
    "关闭行为": "Close Behavior",
    "关闭窗口时最小化到托盘": "Minimize to tray when closing the window",
    "快捷键": "Shortcuts",
    "今日任务": "Today's Tasks",
    "Top 3 弹窗": "Top 3 Popup",
    "全局快捷键需包含 Win / Ctrl / Alt / Shift 之一。点击输入框后按下组合键即可录制。": (
        "Global shortcuts must include Win, Ctrl, Alt, or Shift. "
        "Click the field and press a key combination to record."
    ),
    "工具": "Tools",
    "查看最近一次 AI 分析的详细步骤与 LLM 请求/响应": (
        "View detailed steps and LLM request/response from the most recent AI analysis"
    ),
    "Microsoft 365": "Microsoft 365",
    "登录并同步 Outlook 旗标邮件": "Sign in and sync flagged Outlook emails",
    "关于": "About",
    "版本号": "Version",
    "-": "-",
    "立即锁定": "Lock Now",
    "清除密码": "Clear Password",
    "保存": "Save",
    "btnOk": "btnOk",
    "当前密码不正确": "Current password is incorrect",
    "两次输入的新密码不一致": "New passwords do not match",
    "今日任务：%1": "Today's Tasks: %1",
    "Top 3 弹窗：%1": "Top 3 Popup: %1",
    "两个快捷键不能相同": "The two shortcuts cannot be the same",
    "确定清除登录密码并关闭所有锁定功能？": (
        "Clear the login password and disable all lock features?"
    ),
    "请先输入正确的当前密码": "Enter the correct current password first",
    "密码已清除": "Password cleared",
    "请先设置登录密码": "Set a login password first",
    "每日评估历史": "Daily Evaluation History",
    "查看、编辑或重新生成每日评估。23:59 自动生成，启动时补评遗漏日期；可手动选择日期重新评估（标注当前 LLM 设置中的模型）。": (
        "View, edit, or regenerate daily evaluations. Auto-generated at 23:59; "
        "missed dates are backfilled at startup. Select a date to regenerate "
        "(uses the model from current LLM settings)."
    ),
    "评估日期：": "Evaluation date:",
    "重新生成": "Regenerate",
    "日期": "Date",
    "AI 模型": "AI Model",
    "任务统计": "Task Stats",
    "评估方式": "Evaluation Method",
    "摘要": "Summary",
    "当日任务评价": "Today's Task Review",
    "详细评估": "Detailed Evaluation",
    "删除选中": "Delete Selected",
    "保存编辑": "Save Edits",
    "规则模板": "Rule template",
    "完成 %1 / 到期 %2 / 未完结 %3": "Done %1 / Due %2 / Open %3",
    "AI 评估": "AI evaluation",
    "规则评估": "Rule evaluation",
    "尚无评估记录。选择日期后点击「重新生成」，或等待 23:59 自动生成。": (
        "No evaluations yet. Select a date and click Regenerate, or wait for auto-generation at 23:59."
    ),
    "日期：%1  |  完成 %2 项  |  到期 %3 项  |  未完结 %4 项\n评估方式：%5  |  AI 模型：%6  |  生成时间：%7": (
        "Date: %1  |  Completed %2  |  Due %3  |  Open %4\n"
        "Method: %5  |  AI Model: %6  |  Generated: %7"
    ),
    "正在调用 AI 生成评估…": "Calling AI to generate evaluation…",
    "每日评估": "Daily Evaluation",
    "服务未就绪": "Service not ready",
    "AI 正在处理其他任务，请稍候": "AI is busy with another task, please wait",
    "请选择有效日期": "Select a valid date",
    "日期 %1 已有评估记录，重新生成将覆盖现有内容（含手动编辑）。是否继续？": (
        "An evaluation for %1 already exists. Regenerating will overwrite it "
        "(including manual edits). Continue?"
    ),
    "删除评估": "Delete Evaluation",
    "确定删除 %1 的评估记录吗？此操作不可恢复。": (
        "Delete the evaluation for %1? This cannot be undone."
    ),
    "已保存编辑": "Edits saved",
    "评估失败": "Evaluation failed",
    "已重新生成（%1）": "Regenerated (%1)",
    "LLM 设置": "LLM Settings",
    "提供商": "Provider",
    "已保存模型": "Saved Models",
    "切换当前提供商下已保存的模型配置": "Switch saved model profiles for the current provider",
    "Base URL": "Base URL",
    "API Key": "API Key",
    "模型": "Model",
    "测试连接": "Test Connection",
    "Ollama (本地 Qwen)": "Ollama (Local Qwen)",
    "DeepSeek": "DeepSeek",
    "Kimi (Moonshot)": "Kimi (Moonshot)",
    "Custom OpenAI": "Custom OpenAI",
    "测试": "Test",
    "无法创建 Provider": "Unable to create Provider",
    "请先填写必要配置（云端需 API Key）": "Fill in required settings first (cloud providers need an API Key)",
    "连接成功": "Connection successful",
    "请输入密码": "Enter password",
    "解锁": "Unlock",
    "退出": "Quit",
    "版本 %1": "Version %1",
    "ToDoList": "ToDoList",
    "ToDoList 已锁定": "ToDoList Locked",
    "请输入登录密码以继续使用": "Enter your login password to continue",
    "输入登录密码以继续使用": "Enter login password to continue",
    "未设置密码，请联系管理员": "No password set. Contact your administrator.",
    "Tenant ID": "Tenant ID",
    "common 或组织租户 GUID": "common or organization tenant GUID",
    "Client ID": "Client ID",
    "状态": "Status",
    "设备码登录": "Device Code Sign-In",
    "确认已授权": "Confirm Authorization",
    "验证 URL": "Verification URL",
    "用户码": "User Code",
    "复制": "Copy",
    "打开浏览器": "Open in Browser",
    "同步 Outlook 旗标邮件为任务": "Sync flagged Outlook emails as tasks",
    "Microsoft 365 集成": "Microsoft 365 Integration",
    "已登录": "Signed in",
    "未登录": "Not signed in",
    "登录": "Sign In",
    "Microsoft 365 授权成功": "Microsoft 365 authorization successful",
    "尚未完成授权，请在浏览器中完成登录后重试": (
        "Authorization not complete. Finish sign-in in your browser and try again."
    ),
    "同步": "Sync",
    "已导入 %1 封 flagged 邮件，跳过 %2 条重复项": (
        "Imported %1 flagged email(s), skipped %2 duplicate(s)"
    ),
    "批量输入今日任务，可导入昨日未完成项": (
        "Enter today's tasks in bulk; import yesterday's incomplete items"
    ),
    "添加单条任务，可手动指定象限": "Add a single task and optionally set the quadrant",
    "删除任务": "Delete Task",
    "勾选多条任务后确认删除；单条可右键删除": (
        "Select multiple tasks and confirm delete; right-click a single task to delete"
    ),
    "退出批量删除模式": "Exit Bulk Delete Mode",
    "AI 分析优先级": "AI Priority Analysis",
    "使用 LLM 生成 Top 3 推荐": "Use LLM to generate Top 3 recommendations",
    "重置象限": "Reset Quadrants",
    "将所有任务的象限设为「不确定」，以便重新 AI 划分": (
        "Set all task quadrants to Undecided for re-classification by AI"
    ),
    "设置 LLM": "Configure LLM",
    "历史记录": "History",
    "查看 data 目录中的每日任务归档": "View daily task archives in the data folder",
    "查看 AI 每日完成情况评估历史": "View AI daily completion evaluation history",
    "生成周报": "Generate Weekly Report",
    "勾选最近一周任务，AI 生成工作周报": (
        "Select tasks from the past week; AI generates a work weekly report"
    ),
    "密码、锁屏、版本号等常用设置": "Password, lock screen, version, and other settings",
    "Provider: -": "Provider: -",
    "Eisenhower 四象限": "Eisenhower Matrix",
    "AI Top 3 推荐": "AI Top 3 Recommendations",
    "ToDoList — AI 智能待办": "ToDoList — AI Smart To-Do",
    "数据目录": "Data Folder",
    "数据库": "Database",
    "视图": "View",
    "最小化": "Minimize",
    "最大化 / 还原": "Maximize / Restore",
    "帮助": "Help",
    "打开日志目录": "Open Log Folder",
    "打开今日日志": "Open Today's Log",
    "打开 AI 分析记录文件": "Open AI Analysis Log",
    "尚未设置密码": "No password set yet",
    "密码错误，请重试": "Incorrect password. Try again.",
    "显示主窗口": "Show Main Window",
    "全局快捷键未生效：%1": "Global shortcut not active: %1",
    "快捷键已更新": "Shortcuts updated",
    "程序已最小化到系统托盘": "Minimized to system tray",
    "请选择关闭方式：": "Choose how to close:",
    "最小化到系统托盘": "Minimize to System Tray",
    "Top 3": "Top 3",
    "暂无待办任务": "No pending tasks",
    "完成状态": "Completion Status",
    "任务已完成": "Task completed",
    "任务标记为未完成": "Task marked incomplete",
    "周报生成失败": "Weekly report generation failed",
    "周报已生成（%1）": "Weekly report generated (%1)",
    "已使用规则模板": "Rule template used",
    "%1 每日评估已完成": "%1 daily evaluation completed",
    "确认删除": "Confirm Delete",
    "请勾选要删除的任务，然后点击「确认删除」": (
        "Select tasks to delete, then click Confirm Delete"
    ),
    "请先勾选要删除的任务": "Select tasks to delete first",
    "确定删除选中的 %1 条任务吗？此操作不可撤销。": (
        "Delete the selected %1 task(s)? This cannot be undone."
    ),
    "已取消删除": "Delete cancelled",
    "删除此任务": "Delete This Task",
    "确定删除任务「%1」吗？": "Delete task \"%1\"?",
    "已删除 %1 条任务": "Deleted %1 task(s)",
    "任务已移至 Q%1": "Task moved to Q%1",
    "没有新增任务（已有任务已保留）": "No new tasks added (existing tasks kept)",
    "已追加 %1 条今日任务": "Added %1 task(s) for today",
    "AI 分析": "AI Analysis",
    "请先添加任务": "Add tasks first",
    "暂无分析记录。请先点击「AI 分析优先级」执行一次分析。": (
        "No analysis records yet. Click AI Priority Analysis to run an analysis."
    ),
    "当前没有任务": "No tasks currently",
    "确定将所有任务的象限重置为「不确定 (由 AI 划分)」吗？\n重置后需再次执行 AI 分析才会重新划分象限。": (
        "Reset all task quadrants to \"Undecided (AI classified)\"?\n"
        "Run AI analysis again to re-classify quadrants."
    ),
    "已重置 %1 条任务象限为「不确定」": "Reset quadrant to Undecided for %1 task(s)",
    "AI 分析中…": "AI analyzing…",
    "#%1 %2 (%3)": "#%1 %2 (%3)",
    "点击或双击查看推荐理由": "Click or double-click to view recommendation reason",
    "AI 分析完成": "AI analysis complete",
    "已使用规则层 Top 3": "Rule-layer Top 3 used",
    "，已划分 %1 个待分配象限": ", classified %1 unassigned quadrant(s)",
    "（AI 划分）": "(AI classified)",
    "（规则推断）": "(Rule inferred)",
    "已切换 Provider: %1": "Switched Provider: %1",
    "选中: %1": "Selected: %1",
    "Provider: %1 | Model: %2": "Provider: %1 | Model: %2",
    "学习权重 U:%1 I:%2 B:%3 E:%4": "Learning weights U:%1 I:%2 B:%3 E:%4",
    "Ollama/Qwen": "Ollama/Qwen",
    "Kimi": "Kimi",
    "Custom": "Custom",
    "Unknown": "Unknown",
    "无法创建数据目录: %1": "Unable to create data folder: %1",
    "待AI": "AI Pending",
    "完成于 %1": "Completed at %1",
    "暂无 AI 分析记录。请先执行一次「AI 分析优先级」。": (
        "No AI analysis records yet. Run AI Priority Analysis once."
    ),
    "历史": "History",
    "无法创建日志目录": "Unable to create log folder",
    "HTTPS/TLS 不可用：未找到 OpenSSL 运行库 (libssl-1_1-x64.dll / libcrypto-1_1-x64.dll)。\n\n请任选其一：\n1. 运行 scripts/setup_openssl.ps1 自动部署 OpenSSL\n2. 将上述 DLL 复制到程序目录 (%1)\n3. 在 Qt 维护工具中安装 OpenSSL 1.1 组件，并设置 OPENSSL_BIN 环境变量": (
        "HTTPS/TLS unavailable: OpenSSL runtime libraries not found "
        "(libssl-1_1-x64.dll / libcrypto-1_1-x64.dll).\n\n"
        "Choose one:\n"
        "1. Run scripts/setup_openssl.ps1 to deploy OpenSSL automatically\n"
        "2. Copy the DLLs above to the application folder (%1)\n"
        "3. Install OpenSSL 1.1 in Qt Maintenance Tool and set OPENSSL_BIN"
    ),
    "按日期查看任务快照与已过期任务（含截止超时自动归档的任务）": (
        "Browse task snapshots and overdue tasks by date "
        "(includes tasks auto-archived after deadline)"
    ),
    "（暂无历史）": "(No history yet)",
    "选择": "Select",
    "序号": "#",
    "截止": "Due",
    "分数": "Score",
    "完成": "Done",
    "每行一条任务（截止今天 23:59，象限由 AI 划分）。已有任务会保留在列表中，本次仅追加新行。": (
        "One task per line (due today 23:59; quadrant assigned by AI). "
        "Existing tasks stay in the list; only new lines are appended."
    ),
    "例如：\n完成项目报告\n回复客户邮件\n健身 30 分钟": (
        "For example:\nFinish project report\nReply to client email\nExercise 30 minutes"
    ),
    "导入昨日未完成任务": "Import yesterday's incomplete tasks",
    "添加": "Add",
    "导入昨日任务": "Import Yesterday's Tasks",
    "检测到昨日有 %1 条未完成任务，是否导入到今日任务列表？": (
        "Found %1 incomplete task(s) from yesterday. Import into today's task list?"
    ),
    "导入": "Import",
    "没有可导入的昨日未完成任务。": "No incomplete tasks from yesterday to import.",
    "已导入 %1 条昨日未完成任务。": "Imported %1 incomplete task(s) from yesterday.",
    "今日 Top 3": "Today's Top 3",
    "AI 推荐今日优先完成的 3 项任务：": "AI recommends these 3 tasks to prioritize today:",
    "勾选任务生成周报；历史记录自动保存。打开本窗口时将显示最近一次生成的周报，可编辑、删除或重新生成。": (
        "Select tasks to generate a weekly report; history is saved automatically. "
        "The most recent report opens here for editing, deletion, or regeneration."
    ),
    "周期": "Period",
    "方式": "Method",
    "生成时间": "Generated At",
    "全选": "Select All",
    "全不选": "Select None",
    "仅选已完成": "Completed Only",
    "周报内容（可编辑）": "Report Content (editable)",
    "生成的周报将显示在这里…": "Generated report will appear here…",
    "后台运行": "Run in Background",
    "导出文件": "Export File",
    "正在后台生成周报（%1 ~ %2），请稍候…": (
        "Generating weekly report in background (%1 ~ %2), please wait…"
    ),
    "当前为本地模型（Ollama / %1），生成周报可能需要数分钟。可点击「后台运行」关闭窗口后在后台继续生成，完成后会通知您。": (
        "Using a local model (Ollama / %1); report generation may take several minutes. "
        "Click Run in Background to close this window and continue; you'll be notified when done."
    ),
    "未命名": "Untitled",
    "当前 AI 模型：%1 / %2": "Current AI model: %1 / %2",
    "任务统计周期：%1 至 %2（最近 7 天）": "Task stats period: %1 to %2 (last 7 days)",
    "（该周期内暂无任务记录）": "(No task records in this period)",
    "AI 生成": "AI generated",
    "尚无已保存的周报。勾选任务后点击「生成周报」。": (
        "No saved weekly reports yet. Select tasks and click Generate Weekly Report."
    ),
    "周期：%1 ~ %2  |  方式：%3  |  AI 模型：%4  |  生成时间：%5": (
        "Period: %1 ~ %2  |  Method: %3  |  AI Model: %4  |  Generated: %5"
    ),
    "生成中…": "Generating…",
    "正在生成周报…": "Generating weekly report…",
    "周报正在生成中，请稍候或使用「后台运行」关闭窗口": (
        "Weekly report is generating. Please wait or use Run in Background to close this window."
    ),
    "AI 正在执行优先级分析，请稍后再试": "AI is running priority analysis. Try again later.",
    "请至少勾选一项任务": "Select at least one task",
    "该周期（%1 ~ %2）已有周报，重新生成将覆盖现有内容（含手动编辑）。是否继续？": (
        "A report for %1 ~ %2 already exists. Regenerating will overwrite it "
        "(including manual edits). Continue?"
    ),
    "正在调用 AI 生成周报，请稍候…": "Calling AI to generate weekly report, please wait…",
    "删除周报": "Delete Weekly Report",
    "确定删除 %1 ~ %2 的周报记录吗？": "Delete the weekly report for %1 ~ %2?",
    "请先在左侧选择一条周报记录": "Select a weekly report on the left first",
    "请先生成或选择周报": "Generate or select a weekly report first",
    "保存周报": "Save Weekly Report",
    "Markdown 文件 (*.md);;文本文件 (*.txt)": "Markdown Files (*.md);;Text Files (*.txt)",
    "无法写入文件：%1": "Unable to write file: %1",
    "已保存至：\n%1": "Saved to:\n%1",
    "生成失败": "Generation failed",
    "AI 不可用，已使用规则模板生成周报。\n%1": (
        "AI unavailable; weekly report generated with rule template.\n%1"
    ),
    "已生成（%1）": "Generated (%1)",
    "最大化 / 还原 (F11)": "Maximize / Restore (F11)",
    "还原 (F11)": "Restore (F11)",
    "最大化 (F11)": "Maximize (F11)",
    "界面": "Appearance",
    "语言": "Language",
    "中文": "Chinese",
    "  [已完成": "  [Done",
    "  [未完成": "  [Open",
    " 截止%1": " due %1",
    " %1": " %1",
    "WeeklyReport-%1-%2.md": "WeeklyReport-%1-%2.md",
    "密码至少 4 位": "Password must be at least 4 characters",
    "无法写入密码文件": "Cannot write password file",
}

MESSAGE_PATTERN = re.compile(
    r"(<message>\s*(?:<location[^>]*/>\s*)*<source>)(.*?)(</source>\s*"
    r'<translation(?:\s+type="unfinished")?>\s*</translation>\s*</message>)',
    re.DOTALL,
)


def escape_xml(text: str) -> str:
    return text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def fill_ts(content: str) -> tuple[str, int, int, list[str]]:
    filled = 0
    skipped = 0
    missing: list[str] = []

    def replacer(match: re.Match[str]) -> str:
        nonlocal filled, skipped
        prefix, source, suffix = match.group(1), match.group(2), match.group(3)
        translation = TRANSLATIONS.get(source)
        if translation is None:
            missing.append(source)
            skipped += 1
            return match.group(0)
        filled += 1
        new_suffix = re.sub(
            r'<translation(?:\s+type="unfinished")?>\s*</translation>',
            f"<translation>{escape_xml(translation)}</translation>",
            suffix,
        )
        return prefix + source + new_suffix

    new_content = MESSAGE_PATTERN.sub(replacer, content)
    total_unfinished = len(MESSAGE_PATTERN.findall(content))
    return new_content, filled, total_unfinished, missing


def main() -> int:
    path = TS_PATH
    if not path.is_file():
        print(f"Error: file not found: {path}", file=sys.stderr)
        return 1

    content = path.read_text(encoding="utf-8")
    new_content, filled, total, missing = fill_ts(content)
    path.write_text(new_content, encoding="utf-8", newline="\n")

    unique_missing = list(dict.fromkeys(missing))
    print(f"Filled: {filled}")
    print(f"Missing: {len(missing)} message(s), {len(unique_missing)} unique source(s)")
    if unique_missing:
        print("\nMissing sources:")
        for src in unique_missing:
            preview = src.replace("\n", "\\n")
            if len(preview) > 80:
                preview = preview[:77] + "..."
            print(f"  - {preview!r}")
    return 0 if not unique_missing else 0


if __name__ == "__main__":
    raise SystemExit(main())
