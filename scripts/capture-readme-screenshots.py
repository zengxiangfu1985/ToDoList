#!/usr/bin/env python3
"""
自动截取 README 所需配图，保存到 docs/screenshots/。

依赖安装:
    pip install -r scripts/requirements-screenshots.txt

用法:
    python scripts/capture-readme-screenshots.py
    python scripts/capture-readme-screenshots.py --attach
    python scripts/capture-readme-screenshots.py --attach --app-dir build/.../release
    python scripts/capture-readme-screenshots.py --app-dir build/.../release
    python scripts/capture-readme-screenshots.py --only 02,05,07
    python scripts/capture-readme-screenshots.py --list
    python scripts/capture-readme-screenshots.py --ollama-test   # 14 图尝试真实测试连接

说明:
    - 加 --attach 时附着到**已启动**的 ToDoList，不会复制 staging、不会启动/结束进程。
    - 未加 --attach 时仍会复制到临时目录并自动启动（独立演示数据）。
    - 07 AI 分析：会点击「AI 分析优先级」并等待（无 LLM 时走规则层，通常数秒内完成）。
    - 14 Ollama 示例：默认只打开并选中 Ollama 配置；加 --ollama-test 且本机 Ollama 可用时会点「测试连接」。
"""

from __future__ import annotations

import argparse
import json
import os
import shutil
import sqlite3
import subprocess
import sys
import tempfile
import time
from dataclasses import dataclass
from datetime import date, datetime, timedelta
from pathlib import Path
from typing import Callable, Iterable, Optional

try:
    import pyautogui
    import pyperclip
    import win32con
    import win32gui
    import win32process
    import win32ui
    from PIL import Image, ImageDraw, ImageFont
    from pywinauto import Application, Desktop
    from pywinauto.keyboard import send_keys
except ImportError as exc:  # pragma: no cover
    print("缺少依赖，请先执行: pip install -r scripts/requirements-screenshots.txt", file=sys.stderr)
    raise SystemExit(1) from exc

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUTPUT = ROOT / "docs" / "screenshots"
DEFAULT_APP_CANDIDATES = [
    ROOT / "build" / "Desktop_Qt_5_15_2_MinGW_64_bit-Release" / "release",
    ROOT / "build" / "Desktop_Qt_5_15_2_MinGW_64_bit-Release",
]

MAIN_WINDOW_TITLES = (
    "ToDoList — AI 智能待办",
    "ToDoList - AI 智能待办",
)
EXPLORER_CLASS = "CabinetWClass"
POPUP_MENU_CLASS = "#32768"
QUICK_CAPTURE_SAMPLE = "明天 9 点开会，之后写周报，晚上健身"
TODAY_TASKS_SAMPLE = [
    "今日转发客户报价单",
    "整理 Q2 项目文档",
    "回复行政通知",
    "浏览行业资讯",
]

PW_RENDERFULLCONTENT = 2


@dataclass(frozen=True)
class ShotSpec:
    code: str
    filename: str
    description: str


SHOTS: list[ShotSpec] = [
    ShotSpec("01", "01-quick-start.png", "便携版解压目录"),
    ShotSpec("02", "02-main-window.png", "主界面全貌"),
    ShotSpec("03", "03-add-task.png", "添加任务对话框"),
    ShotSpec("04", "04-today-tasks.png", "今日任务批量录入"),
    ShotSpec("05", "05-quick-capture.png", "闪记居中小窗"),
    ShotSpec("06", "06-focus-25.png", "Focus 25 专注浮窗"),
    ShotSpec("07", "07-ai-analyze.png", "AI 分析后的四象限与 Top 3"),
    ShotSpec("08", "08-top3-popup.png", "Top 3 弹窗"),
    ShotSpec("09", "09-tray-menu.png", "系统托盘右键菜单"),
    ShotSpec("10", "10-settings.png", "设置对话框"),
    ShotSpec("11", "11-about-update.png", "检查更新对话框"),
    ShotSpec("12", "12-daily-evaluation.png", "每日评估历史"),
    ShotSpec("13", "13-llm-settings.png", "LLM 设置"),
    ShotSpec("14", "14-llm-ollama.png", "Ollama 本地配置"),
]


def log(msg: str) -> None:
    print(f"[capture] {msg}", flush=True)


def now_iso() -> str:
    return datetime.now().strftime("%Y-%m-%dT%H:%M:%S")


def resolve_app_dir(explicit: Optional[str]) -> Path:
    if explicit:
        path = Path(explicit).resolve()
        if not (path / "ToDoList.exe").exists():
            raise SystemExit(f"未找到 ToDoList.exe: {path}")
        return path
    for candidate in DEFAULT_APP_CANDIDATES:
        if (candidate / "ToDoList.exe").exists():
            return candidate.resolve()
    raise SystemExit("请通过 --app-dir 指定包含 ToDoList.exe 的目录")


def is_pid_alive(pid: int) -> bool:
    import ctypes

    PROCESS_QUERY_LIMITED_INFORMATION = 0x1000
    STILL_ACTIVE = 259
    kernel32 = ctypes.windll.kernel32
    handle = kernel32.OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, False, pid)
    if not handle:
        return False
    exit_code = ctypes.c_ulong()
    try:
        if not kernel32.GetExitCodeProcess(handle, ctypes.byref(exit_code)):
            return False
        return int(exit_code.value) == STILL_ACTIVE
    finally:
        kernel32.CloseHandle(handle)


def find_running_todolist_pid(explicit_pid: Optional[int] = None) -> int:
    if explicit_pid is not None:
        if not is_pid_alive(explicit_pid):
            raise SystemExit(f"指定 PID 不存在或已退出: {explicit_pid}")
        return explicit_pid

    result = subprocess.run(
        ["tasklist", "/FI", "IMAGENAME eq ToDoList.exe", "/FO", "CSV", "/NH"],
        capture_output=True,
        text=True,
        encoding="gbk",
        errors="ignore",
        check=False,
    )
    pids: list[int] = []
    for line in result.stdout.splitlines():
        line = line.strip()
        if not line or "ToDoList.exe" not in line:
            continue
        parts = [part.strip().strip('"') for part in line.split(",")]
        if len(parts) >= 2 and parts[1].isdigit():
            pids.append(int(parts[1]))

    if not pids:
        raise SystemExit("未找到运行中的 ToDoList.exe。请先启动应用，或去掉 --attach 让脚本自动启动。")
    if len(pids) > 1:
        log(f"检测到多个 ToDoList 进程 {pids}，默认使用 {pids[0]}（可用 --pid 指定）")
    return pids[0]


def process_exe_dir(pid: int) -> Path:
    import ctypes
    from ctypes import wintypes

    PROCESS_QUERY_LIMITED_INFORMATION = 0x1000
    kernel32 = ctypes.windll.kernel32
    handle = kernel32.OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, False, pid)
    if not handle:
        raise SystemExit(f"无法查询进程路径，PID={pid}")
    try:
        buf = ctypes.create_unicode_buffer(32768)
        size = wintypes.DWORD(len(buf))
        if not kernel32.QueryFullProcessImageNameW(handle, 0, buf, ctypes.byref(size)):
            raise SystemExit(f"无法获取进程可执行文件路径，PID={pid}")
        return Path(buf.value).resolve().parent
    finally:
        kernel32.CloseHandle(handle)


def window_pid(hwnd: int) -> int:
    _, pid = win32process.GetWindowThreadProcessId(hwnd)
    return int(pid)


def enum_top_level_windows(class_name: Optional[str] = None) -> list[tuple[int, str, str]]:
    found: list[tuple[int, str, str]] = []

    def callback(hwnd, _):
        if not win32gui.IsWindowVisible(hwnd):
            return True
        cls = win32gui.GetClassName(hwnd)
        if class_name and cls != class_name:
            return True
        title = win32gui.GetWindowText(hwnd)
        found.append((hwnd, title, cls))
        return True

    win32gui.EnumWindows(callback, None)
    return found


def enum_windows(predicate: Callable[[int, str], bool]) -> list[tuple[int, str]]:
    found: list[tuple[int, str]] = []

    def callback(hwnd, _):
        if not win32gui.IsWindowVisible(hwnd):
            return True
        title = win32gui.GetWindowText(hwnd)
        if predicate(hwnd, title):
            found.append((hwnd, title))
        return True

    win32gui.EnumWindows(callback, None)
    return found


def is_main_window_title(title: str) -> bool:
    if title in MAIN_WINDOW_TITLES:
        return True
    return title.startswith("ToDoList") and "AI 智能待办" in title


def find_main_window(pid: int, timeout: float = 30.0) -> int:
    end = time.time() + timeout
    while time.time() < end:
        if not is_pid_alive(pid):
            raise RuntimeError("ToDoList 进程已退出")

        try:
            app = Application(backend="uia").connect(process=pid)
            for title in MAIN_WINDOW_TITLES:
                try:
                    win = app.window(title=title)
                    if win.exists(timeout=0.5):
                        return int(win.handle)
                except Exception:
                    pass
            for win in app.windows():
                text = win.window_text()
                if is_main_window_title(text):
                    return int(win.handle)
        except Exception:
            pass

        for hwnd, title, _cls in enum_top_level_windows():
            if window_pid(hwnd) != pid:
                continue
            if is_main_window_title(title):
                return hwnd
        time.sleep(0.25)
    raise TimeoutError("超时未找到 ToDoList 主窗口（已按进程 PID 过滤，避免误匹配 IDE）")


def find_window_for_process(
    pid: int,
    *,
    exact_title: Optional[str] = None,
    title_contains: Optional[tuple[str, ...]] = None,
    timeout: float = 8.0,
) -> int:
    end = time.time() + timeout
    while time.time() < end:
        if not is_pid_alive(pid):
            raise RuntimeError("ToDoList 进程已退出")
        for hwnd, title, _cls in enum_top_level_windows():
            if window_pid(hwnd) != pid:
                continue
            if exact_title and title == exact_title:
                return hwnd
            if title_contains and any(part in title for part in title_contains):
                return hwnd
        time.sleep(0.2)
    if exact_title:
        raise TimeoutError(f"超时未找到进程窗口: {exact_title!r}")
    raise TimeoutError(f"超时未找到进程窗口，标题需包含: {title_contains}")


def find_window_exact_title(
    title: str, timeout: float = 8.0, pid: Optional[int] = None
) -> int:
    end = time.time() + timeout
    while time.time() < end:
        for hwnd, win_title, _cls in enum_top_level_windows():
            if win_title != title:
                continue
            if pid is not None and window_pid(hwnd) != pid:
                continue
            return hwnd
        time.sleep(0.25)
    raise TimeoutError(f"超时未找到窗口: {title!r}")


def find_window_title_contains(
    *parts: str,
    timeout: float = 15.0,
    pid: Optional[int] = None,
) -> int:
    end = time.time() + timeout
    while time.time() < end:
        for hwnd, title, _cls in enum_top_level_windows():
            if not any(part in title for part in parts):
                continue
            if pid is not None and window_pid(hwnd) != pid:
                continue
            return hwnd
        time.sleep(0.25)
    raise TimeoutError(f"超时未找到窗口，标题需包含: {parts}")


def capture_hwnd(hwnd: int, path: Path) -> None:
    win32gui.SetForegroundWindow(hwnd)
    time.sleep(0.15)

    left, top, right, bottom = win32gui.GetWindowRect(hwnd)
    width = max(1, right - left)
    height = max(1, bottom - top)

    hwnd_dc = win32gui.GetWindowDC(hwnd)
    mfc_dc = win32ui.CreateDCFromHandle(hwnd_dc)
    save_dc = mfc_dc.CreateCompatibleDC()
    bitmap = win32ui.CreateBitmap()
    bitmap.CreateCompatibleBitmap(mfc_dc, width, height)
    save_dc.SelectObject(bitmap)

    try:
        import ctypes

        result = ctypes.windll.user32.PrintWindow(hwnd, save_dc.GetSafeHdc(), PW_RENDERFULLCONTENT)
        bmpinfo = bitmap.GetInfo()
        bmpstr = bitmap.GetBitmapBits(True)
        image = Image.frombuffer(
            "RGB",
            (bmpinfo["bmWidth"], bmpinfo["bmHeight"]),
            bmpstr,
            "raw",
            "BGRX",
            0,
            1,
        )
        if not result or image.getbbox() is None:
            raise RuntimeError("PrintWindow 返回空画面")
        path.parent.mkdir(parents=True, exist_ok=True)
        image.save(path, format="PNG", optimize=True)
    finally:
        win32gui.DeleteObject(bitmap.GetHandle())
        save_dc.DeleteDC()
        mfc_dc.DeleteDC()
        win32gui.ReleaseDC(hwnd, hwnd_dc)


def capture_screen_region(rect: tuple[int, int, int, int], path: Path) -> None:
    left, top, right, bottom = rect
    shot = pyautogui.screenshot(region=(left, top, right - left, bottom - top))
    path.parent.mkdir(parents=True, exist_ok=True)
    shot.save(path, format="PNG", optimize=True)


def wait_for_popup_menu(timeout: float = 15.0) -> int:
    end = time.time() + timeout
    last_hwnd = 0
    while time.time() < end:
        hwnd = win32gui.FindWindow(POPUP_MENU_CLASS, None)
        if hwnd and win32gui.IsWindowVisible(hwnd):
            last_hwnd = hwnd
            time.sleep(0.2)
            return hwnd
        time.sleep(0.08)
    if last_hwnd:
        return last_hwnd
    raise TimeoutError("未检测到弹出菜单（#32768）")


def render_directory_mockup(folder: Path, path: Path) -> None:
    """Explorer 截取失败时的合成目录示意图。"""
    entries = []
    for item in sorted(folder.iterdir(), key=lambda p: (not p.is_dir(), p.name.lower())):
        if item.name.startswith("."):
            continue
        kind = "文件夹" if item.is_dir() else "文件"
        size = "" if item.is_dir() else f"{item.stat().st_size:,} 字节"
        entries.append((item.name, kind, size))
    if not entries:
        entries = [("ToDoList.exe", "文件", ""), ("data", "文件夹", "")]

    width, height = 1100, 620
    image = Image.new("RGB", (width, height), (255, 255, 255))
    draw = ImageDraw.Draw(image)
    try:
        title_font = ImageFont.truetype("segoeui.ttf", 20)
        body_font = ImageFont.truetype("segoeui.ttf", 16)
    except OSError:
        title_font = ImageFont.load_default()
        body_font = ImageFont.load_default()

    draw.text((24, 18), f"便携版目录：{folder.name}", fill=(30, 30, 30), font=title_font)
    draw.line((24, 52, width - 24, 52), fill=(220, 220, 220), width=1)
    draw.text((36, 68), "名称", fill=(90, 90, 90), font=body_font)
    draw.text((620, 68), "类型", fill=(90, 90, 90), font=body_font)
    draw.text((760, 68), "大小", fill=(90, 90, 90), font=body_font)

    y = 104
    for name, kind, size in entries[:12]:
        draw.text((36, y), name, fill=(20, 20, 20), font=body_font)
        draw.text((620, y), kind, fill=(60, 60, 60), font=body_font)
        if size:
            draw.text((760, y), size, fill=(60, 60, 60), font=body_font)
        y += 34

    path.parent.mkdir(parents=True, exist_ok=True)
    image.save(path, format="PNG", optimize=True)


def find_explorer_window(folder: Path, timeout: float = 20.0) -> int:
    folder_name = folder.name
    folder_str = str(folder).lower()
    end = time.time() + timeout
    while time.time() < end:
        candidates: list[tuple[int, str]] = []
        for hwnd, title, cls in enum_top_level_windows(EXPLORER_CLASS):
            title_l = title.lower()
            if folder_name.lower() in title_l or folder_str in title_l:
                candidates.append((hwnd, title))
        if candidates:
            hwnd, _title = candidates[-1]
            win32gui.ShowWindow(hwnd, win32con.SW_RESTORE)
            win32gui.ShowWindow(hwnd, win32con.SW_MAXIMIZE)
            win32gui.SetForegroundWindow(hwnd)
            time.sleep(1.2)
            return hwnd
        time.sleep(0.25)
    raise TimeoutError(f"超时未找到资源管理器窗口: {folder}")


class ScreenshotSession:
    def __init__(
        self,
        app_dir: Path,
        output_dir: Path,
        attach: bool = False,
        attach_pid: Optional[int] = None,
        maximize_main: bool = True,
        width: int = 1280,
        height: int = 800,
        ai_timeout: int = 120,
        ollama_test: bool = False,
        tray_wait: int = 15,
        pause: float = 0.6,
    ) -> None:
        self.source_app_dir = app_dir
        self.output_dir = output_dir
        self.attach = attach
        self.attach_pid = attach_pid
        self.maximize_main = maximize_main
        self.width = width
        self.height = height
        self.ai_timeout = ai_timeout
        self.ollama_test = ollama_test
        self.tray_wait = tray_wait
        self.pause = pause

        self.staging_dir = app_dir if attach else Path(tempfile.mkdtemp(prefix="todolist-shot-"))
        self.app_pid: Optional[int] = attach_pid if attach else None
        self._popen: Optional[subprocess.Popen] = None
        self.app: Optional[Application] = None
        self.main_hwnd: Optional[int] = None

        pyautogui.FAILSAFE = True
        pyautogui.PAUSE = 0.15

    def cleanup(self) -> None:
        if self._popen and self._popen.poll() is None:
            log("结束 ToDoList 进程")
            self._popen.terminate()
            try:
                self._popen.wait(timeout=5)
            except subprocess.TimeoutExpired:
                self._popen.kill()
        if not self.attach and self.staging_dir.exists():
            shutil.rmtree(self.staging_dir, ignore_errors=True)

    def prepare_staging(self) -> Path:
        log(f"复制程序到 staging: {self.staging_dir}")
        ignore = shutil.ignore_patterns("*.pdb", "*.obj", "*.o", "moc_*", "ui_*")
        shutil.copytree(self.source_app_dir, self.staging_dir, dirs_exist_ok=True, ignore=ignore)
        data_dir = self.staging_dir / "data"
        if data_dir.exists():
            shutil.rmtree(data_dir)
        data_dir.mkdir(parents=True)
        seed_demo_data(data_dir)
        self._install_portable_marker_files()
        return self.staging_dir

    def _install_portable_marker_files(self) -> None:
        readme_src = ROOT / "scripts" / "portable-readme.txt"
        bat_src = ROOT / "scripts" / "portable-start.bat"
        if readme_src.exists():
            shutil.copy2(readme_src, self.staging_dir / "使用说明.txt")
        if bat_src.exists():
            shutil.copy2(bat_src, self.staging_dir / "启动 ToDoList.bat")

    def launch(self) -> None:
        exe = self.staging_dir / "ToDoList.exe"
        log(f"启动: {exe}")
        self._popen = subprocess.Popen([str(exe)], cwd=str(self.staging_dir))
        self.app_pid = self._popen.pid
        self._connect_app(timeout=30)

    def attach_to_running(self) -> None:
        self.app_pid = find_running_todolist_pid(self.attach_pid)
        log(f"附着到已运行 ToDoList，PID={self.app_pid}")
        self._connect_app(timeout=20)

    def _connect_app(self, timeout: float) -> None:
        assert self.app_pid is not None
        self.main_hwnd = find_main_window(self.app_pid, timeout=timeout)
        self.app = Application(backend="uia").connect(process=self.app_pid)
        self.maximize_and_focus_main()
        time.sleep(1.5)

    @staticmethod
    def _is_maximized(hwnd: int) -> bool:
        try:
            placement = win32gui.GetWindowPlacement(hwnd)
            return placement[1] == win32con.SW_SHOWMAXIMIZED
        except Exception:
            return False

    def _refresh_main_hwnd(self) -> None:
        if self.app_pid is None:
            return
        try:
            self.main_hwnd = find_main_window(self.app_pid, timeout=5)
        except TimeoutError:
            pass

    def maximize_and_focus_main(self) -> None:
        assert self.main_hwnd is not None
        self._refresh_main_hwnd()
        assert self.main_hwnd is not None

        win32gui.ShowWindow(self.main_hwnd, win32con.SW_RESTORE)
        time.sleep(0.15)
        win32gui.SetForegroundWindow(self.main_hwnd)

        if self.maximize_main:
            win32gui.ShowWindow(self.main_hwnd, win32con.SW_MAXIMIZE)
            time.sleep(0.35)
            if not self._is_maximized(self.main_hwnd):
                pyautogui.press("f11")
                time.sleep(0.6)
            win32gui.SetForegroundWindow(self.main_hwnd)
            time.sleep(max(self.pause, 0.8))
            return

        win32gui.SetWindowPos(
            self.main_hwnd,
            win32con.HWND_TOP,
            80,
            40,
            self.width,
            self.height,
            win32con.SWP_SHOWWINDOW,
        )
        time.sleep(self.pause)

    def resize_and_focus_main(self) -> None:
        """兼容旧调用名。"""
        self.maximize_and_focus_main()

    def save(self, filename: str) -> Path:
        path = self.output_dir / filename
        hwnd = win32gui.GetForegroundWindow()
        if hwnd:
            capture_hwnd(hwnd, path)
        else:
            raise RuntimeError("无前台窗口可截取")
        log(f"已保存: {path}")
        return path

    def save_hwnd(self, hwnd: int, filename: str) -> Path:
        path = self.output_dir / filename
        capture_hwnd(hwnd, path)
        log(f"已保存: {path}")
        return path

    def save_main(self, filename: str) -> Path:
        assert self.app_pid is not None
        self.dismiss_focus_overlay()
        self.main_hwnd = find_main_window(self.app_pid, timeout=10)
        self.maximize_and_focus_main()
        time.sleep(0.5)
        self.main_hwnd = find_main_window(self.app_pid, timeout=5)
        return self.save_hwnd(self.main_hwnd, filename)

    def dismiss_focus_overlay(self) -> None:
        """关闭可能遮挡主界面的 Focus 25 浮窗。"""
        if self.app_pid is None:
            return
        for _ in range(2):
            try:
                hwnd = find_window_for_process(
                    self.app_pid, title_contains=("Focus 25",), timeout=0.8
                )
            except TimeoutError:
                return
            win32gui.SetForegroundWindow(hwnd)
            send_keys("{ESC}")
            time.sleep(0.25)

    def click_main_button(self, title: str) -> None:
        assert self.app is not None and self.app_pid is not None
        self._refresh_main_hwnd()
        win = self.app.window(handle=self.main_hwnd)
        win.child_window(title=title, control_type="Button").click_input()
        time.sleep(self.pause)

    def hotkey(self, *keys: str) -> None:
        self.resize_and_focus_main()
        pyautogui.hotkey(*keys)
        time.sleep(self.pause)

    def close_foreground_dialog(self) -> None:
        send_keys("{ESC}")
        time.sleep(self.pause)

    def dismiss_message_boxes(self) -> None:
        for hwnd, title in enum_windows(lambda _h, t: t in {"ToDoList", "闪记", "Focus 25", "设置", "关于"}):
            if title in {"ToDoList"}:
                try:
                    app = Application(backend="uia").connect(handle=hwnd)
                    for win in app.windows():
                        if win.element_info.control_type == "Window" and win.window_text() != "ToDoList — AI 智能待办":
                            continue
                except Exception:
                    pass
        send_keys("{ESC}")
        time.sleep(0.3)

    # --- individual shots ---

    def shot_01_quick_start(self) -> None:
        exe_path = self.staging_dir / "ToDoList.exe"
        subprocess.Popen(["explorer", "/select,", str(exe_path)])
        try:
            hwnd = find_explorer_window(self.staging_dir, timeout=20)
            time.sleep(0.8)
            self.save_hwnd(hwnd, "01-quick-start.png")
            win32gui.PostMessage(hwnd, win32con.WM_CLOSE, 0, 0)
            time.sleep(0.4)
        except Exception as exc:
            log(f"Explorer 截取失败，改用合成目录图: {exc}")
            out = self.output_dir / "01-quick-start.png"
            render_directory_mockup(self.staging_dir, out)
            log(f"已保存: {out}")

    def shot_02_main_window(self) -> None:
        self.save_main("02-main-window.png")

    def shot_03_add_task(self) -> None:
        self.click_main_button("添加任务")
        hwnd = find_window_for_process(self.app_pid, exact_title="添加任务", timeout=8)
        time.sleep(self.pause)
        self.save_hwnd(hwnd, "03-add-task.png")
        send_keys("{ESC}")
        time.sleep(self.pause)

    def shot_04_today_tasks(self) -> None:
        self.hotkey("alt", "shift", "j")
        hwnd = find_window_for_process(self.app_pid, exact_title="今日任务", timeout=8)
        time.sleep(self.pause)
        self.save_hwnd(hwnd, "04-today-tasks.png")
        send_keys("{ESC}")
        time.sleep(self.pause)

    def shot_05_quick_capture(self) -> None:
        self.hotkey("alt", "shift", "n")
        hwnd = find_window_for_process(self.app_pid, exact_title="闪记", timeout=8)
        time.sleep(self.pause)
        pyperclip.copy(QUICK_CAPTURE_SAMPLE)
        pyautogui.hotkey("ctrl", "a")
        pyautogui.hotkey("ctrl", "v")
        time.sleep(self.pause)
        self.save_hwnd(hwnd, "05-quick-capture.png")
        send_keys("{ESC}")
        time.sleep(self.pause)

    def shot_06_focus_25(self) -> None:
        self.resize_and_focus_main()
        self.hotkey("alt", "shift", "f")
        time.sleep(1.0)
        try:
            hwnd = find_window_for_process(self.app_pid, title_contains=("Focus 25",), timeout=6)
        except TimeoutError:
            hwnd = find_window_for_process(self.app_pid, title_contains=("Focus",), timeout=4)
        rect = win32gui.GetWindowRect(hwnd)
        x = rect[0] + (rect[2] - rect[0]) // 2
        y = rect[1] + (rect[3] - rect[1]) // 2
        pyautogui.moveTo(x, y, duration=0.2)
        time.sleep(0.8)
        self.save_hwnd(hwnd, "06-focus-25.png")
        pyautogui.click(x, y)
        time.sleep(0.3)
        send_keys("{ESC}")
        time.sleep(self.pause)

    def shot_07_ai_analyze(self) -> None:
        self.resize_and_focus_main()
        self.click_main_button("AI 分析优先级")
        end = time.time() + self.ai_timeout
        while time.time() < end:
            try:
                assert self.app is not None
                win = self.app.window(handle=self.main_hwnd)
                btn = win.child_window(title="AI 分析优先级", control_type="Button")
                if btn.is_enabled():
                    break
            except Exception:
                pass
            time.sleep(0.5)
        time.sleep(1.0)
        self.save_main("07-ai-analyze.png")

    def shot_08_top3_popup(self) -> None:
        self.hotkey("alt", "shift", "3")
        hwnd = find_window_for_process(self.app_pid, title_contains=("Top 3",), timeout=8)
        time.sleep(self.pause)
        self.save_hwnd(hwnd, "08-top3-popup.png")
        send_keys("{ESC}")
        time.sleep(self.pause)

    def shot_09_tray_menu(self) -> None:
        assert self.main_hwnd is not None
        win32gui.PostMessage(self.main_hwnd, win32con.WM_CLOSE, 0, 0)
        time.sleep(1.2)
        print()
        print("=" * 60)
        print(f"  请在 {self.tray_wait} 秒内右键点击任务栏托盘区的 ToDoList 图标")
        print("  检测到菜单后会立即截取（无需等倒计时结束）")
        print("=" * 60)
        print()

        menu_hwnd = 0
        end = time.time() + self.tray_wait
        while time.time() < end:
            remaining = max(0, int(end - time.time()))
            print(f"\r  等待托盘菜单… {remaining:2d}s ", end="", flush=True)
            hwnd = win32gui.FindWindow(POPUP_MENU_CLASS, None)
            if hwnd and win32gui.IsWindowVisible(hwnd):
                time.sleep(0.25)
                menu_hwnd = hwnd
                break
            time.sleep(0.08)
        print()

        if not menu_hwnd:
            raise TimeoutError("未捕捉到托盘右键菜单，请在倒计时内右键托盘图标后重试")

        self.save_hwnd(menu_hwnd, "09-tray-menu.png")
        send_keys("{ESC}")
        time.sleep(0.5)
        self.hotkey("alt", "shift", "j")
        time.sleep(0.6)
        send_keys("{ESC}")
        assert self.app_pid is not None
        self.main_hwnd = find_main_window(self.app_pid, timeout=15)
        time.sleep(self.pause)

    def shot_10_settings(self) -> None:
        self.click_main_button("设置")
        hwnd = find_window_for_process(self.app_pid, exact_title="设置", timeout=8)
        time.sleep(self.pause)
        self.save_hwnd(hwnd, "10-settings.png")
        send_keys("{ESC}")
        time.sleep(self.pause)

    def shot_11_about_update(self) -> None:
        self.click_main_button("关于")
        about_hwnd = find_window_for_process(self.app_pid, exact_title="关于", timeout=8)
        app = Application(backend="uia").connect(handle=about_hwnd)
        dlg = app.window(handle=about_hwnd)
        dlg.child_window(title="检查更新", control_type="Button").click_input()
        time.sleep(0.8)
        update_hwnd = find_window_for_process(self.app_pid, exact_title="检查更新", timeout=15)
        time.sleep(2.5)
        self.save_hwnd(update_hwnd, "11-about-update.png")
        send_keys("{ESC}")
        time.sleep(0.4)
        send_keys("{ESC}")
        time.sleep(self.pause)

    def shot_12_daily_evaluation(self) -> None:
        self.click_main_button("每日评估")
        hwnd = find_window_for_process(self.app_pid, title_contains=("每日评估历史",), timeout=10)
        time.sleep(self.pause)
        self.save_hwnd(hwnd, "12-daily-evaluation.png")
        send_keys("{ESC}")
        time.sleep(self.pause)

    def shot_13_llm_settings(self) -> None:
        self.click_main_button("设置 LLM")
        hwnd = find_window_for_process(self.app_pid, exact_title="LLM 设置", timeout=8)
        time.sleep(self.pause)
        self.save_hwnd(hwnd, "13-llm-settings.png")
        send_keys("{ESC}")
        time.sleep(self.pause)

    def shot_14_llm_ollama(self) -> None:
        self.click_main_button("设置 LLM")
        hwnd = find_window_for_process(self.app_pid, exact_title="LLM 设置", timeout=8)
        app = Application(backend="uia").connect(handle=hwnd)
        dlg = app.window(handle=hwnd)
        combos = dlg.descendants(control_type="ComboBox")
        if combos:
            combos[0].select("Ollama (本地 Qwen)")
            time.sleep(0.4)
        edits = dlg.descendants(control_type="Edit")
        for edit in edits:
            name = edit.window_text()
            if "11434" in name or edit.legacy_properties().get("Value", "") == "":
                pass
        if self.ollama_test:
            try:
                dlg.child_window(title="测试连接", control_type="Button").click_input()
                time.sleep(4.0)
            except Exception as exc:
                log(f"Ollama 测试连接跳过: {exc}")
        time.sleep(self.pause)
        self.save_hwnd(hwnd, "14-llm-ollama.png")
        send_keys("{ESC}")
        time.sleep(self.pause)

    def run(self, selected: Optional[set[str]] = None) -> None:
        handlers: dict[str, Callable[[], None]] = {
            "01": self.shot_01_quick_start,
            "02": self.shot_02_main_window,
            "03": self.shot_03_add_task,
            "04": self.shot_04_today_tasks,
            "05": self.shot_05_quick_capture,
            "06": self.shot_06_focus_25,
            "07": self.shot_07_ai_analyze,
            "08": self.shot_08_top3_popup,
            "09": self.shot_09_tray_menu,
            "10": self.shot_10_settings,
            "11": self.shot_11_about_update,
            "12": self.shot_12_daily_evaluation,
            "13": self.shot_13_llm_settings,
            "14": self.shot_14_llm_ollama,
        }

        plan = [s for s in SHOTS if selected is None or s.code in selected]
        needs_app = any(s.code != "01" for s in plan)

        try:
            if not self.attach:
                self.prepare_staging()

            if any(s.code == "01" for s in plan):
                log("=== 01 01-quick-start.png — 便携版解压目录 ===")
                try:
                    self.shot_01_quick_start()
                except Exception as exc:
                    log(f"失败 01-quick-start.png: {exc}")

            if needs_app:
                if self.attach:
                    self.attach_to_running()
                else:
                    self.launch()

            for spec in plan:
                if spec.code == "01":
                    continue
                log(f"=== {spec.code} {spec.filename} — {spec.description} ===")
                handler = handlers[spec.code]
                try:
                    handler()
                except Exception as exc:
                    log(f"失败 {spec.filename}: {exc}")
                    self.dismiss_message_boxes()
                    if self.app_pid:
                        try:
                            self.resize_and_focus_main()
                        except Exception:
                            pass

            log(f"完成，输出目录: {self.output_dir.resolve()}")
        finally:
            self.cleanup()


def seed_demo_data(data_dir: Path) -> None:
    """写入演示任务、Top3、每日评估与专注记录。"""
    data_dir.mkdir(parents=True, exist_ok=True)
    db_path = data_dir / "tasks.db"
    today = date.today()
    today_s = today.isoformat()
    now = now_iso()

    tasks = [
        ("今日转发客户报价单", 1, 92.5, 1, "需今日转发客户，重要且紧急，建议优先完成"),
        ("整理 Q2 项目文档", 2, 74.0, 2, "重要不紧急，建议本周安排"),
        ("回复行政通知", 3, 48.0, 3, "紧急但不重要，可快速处理"),
        ("浏览行业资讯", 4, 18.0, 0, "可延后处理"),
    ]

    conn = sqlite3.connect(db_path)
    cur = conn.cursor()
    cur.executescript(
        """
        CREATE TABLE IF NOT EXISTS schema_meta (
            key TEXT PRIMARY KEY,
            value TEXT NOT NULL
        );
        CREATE TABLE IF NOT EXISTS tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            due_at TEXT,
            quadrant INTEGER NOT NULL DEFAULT 2,
            completed INTEGER NOT NULL DEFAULT 0,
            rule_score REAL NOT NULL DEFAULT 0,
            ai_rank INTEGER NOT NULL DEFAULT 0,
            ai_reason TEXT,
            external_source TEXT,
            external_id TEXT,
            created_at TEXT NOT NULL,
            updated_at TEXT NOT NULL,
            archived INTEGER NOT NULL DEFAULT 0,
            archived_at TEXT
        );
        CREATE TABLE IF NOT EXISTS behavior_events (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            event_type TEXT NOT NULL,
            task_id INTEGER,
            quadrant INTEGER,
            hour_of_day INTEGER,
            created_at TEXT NOT NULL
        );
        CREATE TABLE IF NOT EXISTS daily_evaluations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            eval_date TEXT NOT NULL UNIQUE,
            tasks_completed INTEGER NOT NULL DEFAULT 0,
            tasks_due INTEGER NOT NULL DEFAULT 0,
            tasks_pending INTEGER NOT NULL DEFAULT 0,
            summary TEXT,
            ai_feedback TEXT,
            used_llm INTEGER NOT NULL DEFAULT 0,
            created_at TEXT NOT NULL,
            task_plan_review TEXT,
            llm_provider INTEGER NOT NULL DEFAULT 0,
            llm_model TEXT
        );
        CREATE TABLE IF NOT EXISTS focus_sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            task_id INTEGER NOT NULL,
            started_at TEXT NOT NULL,
            ended_at TEXT,
            duration_sec INTEGER NOT NULL,
            completed INTEGER NOT NULL DEFAULT 0,
            abandoned INTEGER NOT NULL DEFAULT 0,
            pomodoro_index INTEGER NOT NULL DEFAULT 1
        );
        """
    )
    cur.execute("DELETE FROM schema_meta")
    cur.execute("INSERT INTO schema_meta(key, value) VALUES ('schema_version', '2')")
    cur.execute("DELETE FROM tasks")
    cur.execute("DELETE FROM daily_evaluations")
    cur.execute("DELETE FROM focus_sessions")

    due_at = f"{today_s}T18:00:00"
    task_ids: list[int] = []
    for title, quadrant, score, ai_rank, reason in tasks:
        cur.execute(
            """
            INSERT INTO tasks
            (title, due_at, quadrant, completed, rule_score, ai_rank, ai_reason,
             created_at, updated_at)
            VALUES (?, ?, ?, 0, ?, ?, ?, ?, ?)
            """,
            (title, due_at, quadrant, score, ai_rank, reason, now, now),
        )
        task_ids.append(int(cur.lastrowid))

    top3 = [
        (task_ids[0], tasks[0][0], 1, tasks[0][2], tasks[0][4]),
        (task_ids[1], tasks[1][0], 2, tasks[1][2], tasks[1][4]),
        (task_ids[2], tasks[2][0], 3, tasks[2][2], tasks[2][4]),
    ]
    top3_path = data_dir / f"top3-{today_s}.json"
    top3_doc = {
        "provider": 0,
        "model": "demo-screenshot",
        "used_llm": False,
        "top3": [
            {
                "id": tid,
                "title": title,
                "rank": rank,
                "score": score,
                "reason": reason,
            }
            for tid, title, rank, score, reason in top3
        ],
        "trace": [],
    }
    top3_path.write_text(json.dumps(top3_doc, ensure_ascii=False, indent=2), encoding="utf-8")

    for offset in range(3):
        eval_day = today - timedelta(days=offset)
        eval_s = eval_day.isoformat()
        cur.execute(
            """
            INSERT INTO daily_evaluations
            (eval_date, tasks_completed, tasks_due, tasks_pending, summary, ai_feedback,
             used_llm, created_at, llm_provider, llm_model)
            VALUES (?, ?, ?, ?, ?, ?, 0, ?, 0, 'demo')
            """,
            (
                eval_s,
                2 + offset,
                4,
                max(0, 2 - offset),
                f"{eval_s} 完成核心任务，明日优先推进报价与客户沟通。",
                "专注时段有效；建议继续将 Q1 任务放在上午处理。",
                now,
            ),
        )

    start = datetime.now().replace(hour=10, minute=0, second=0, microsecond=0)
    for i in range(2):
        cur.execute(
            """
            INSERT INTO focus_sessions
            (task_id, started_at, ended_at, duration_sec, completed, abandoned, pomodoro_index)
            VALUES (?, ?, ?, 1500, 1, 0, ?)
            """,
            (
                task_ids[0],
                (start + timedelta(minutes=i * 30)).isoformat(),
                (start + timedelta(minutes=i * 30 + 25)).isoformat(),
                i + 1,
            ),
        )

    conn.commit()
    conn.close()

    settings_path = data_dir / "settings.ini"
    settings_path.write_text(
        "\n".join(
            [
                "[security]",
                "lockOnStartup=false",
                "minimizeToTray=true",
                "",
                "[usage]",
                "statisticsEnabled=false",
                "",
                "[ui]",
                "language=zh_CN",
                "",
            ]
        ),
        encoding="utf-8",
    )


def parse_args(argv: Optional[Iterable[str]] = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="自动截取 README 配图")
    parser.add_argument(
        "--attach",
        action="store_true",
        help="附着到已运行的 ToDoList（不复制 staging、不启动/结束进程）",
    )
    parser.add_argument("--pid", type=int, help="配合 --attach 指定进程 PID")
    parser.add_argument("--app-dir", help="包含 ToDoList.exe 的目录（--attach 时可省略，自动从进程解析）")
    parser.add_argument("--output-dir", default=str(DEFAULT_OUTPUT), help="截图输出目录")
    parser.add_argument("--only", help="只截取指定编号，逗号分隔，如 02,05,07")
    parser.add_argument("--list", action="store_true", help="列出全部截图任务")
    parser.add_argument("--no-maximize", action="store_true", help="主窗口不最大化，改用 --width/--height 固定尺寸")
    parser.add_argument("--width", type=int, default=1280, help="--no-maximize 时主窗口宽度")
    parser.add_argument("--height", type=int, default=800, help="--no-maximize 时主窗口高度")
    parser.add_argument("--ai-timeout", type=int, default=120, help="AI 分析最长等待秒数")
    parser.add_argument("--tray-wait", type=int, default=15, help="等待用户右键托盘的秒数")
    parser.add_argument("--ollama-test", action="store_true", help="14 图尝试点击 Ollama 测试连接")
    return parser.parse_args(argv)


def main(argv: Optional[Iterable[str]] = None) -> int:
    args = parse_args(argv)

    if args.list:
        for spec in SHOTS:
            print(f"{spec.code}\t{spec.filename}\t{spec.description}")
        return 0

    if os.name != "nt":
        print("此脚本仅支持 Windows。", file=sys.stderr)
        return 1

    if args.attach:
        attach_pid = find_running_todolist_pid(args.pid)
        app_dir = Path(args.app_dir).resolve() if args.app_dir else process_exe_dir(attach_pid)
        if not (app_dir / "ToDoList.exe").exists():
            raise SystemExit(f"未找到 ToDoList.exe: {app_dir}")
        log(f"附着模式 PID={attach_pid}")
    else:
        attach_pid = None
        app_dir = resolve_app_dir(args.app_dir)

    output_dir = Path(args.output_dir).resolve()
    selected = None
    if args.only:
        selected = {part.strip() for part in args.only.split(",") if part.strip()}

    log(f"程序目录: {app_dir}")
    log(f"输出目录: {output_dir}")

    session = ScreenshotSession(
        app_dir=app_dir,
        output_dir=output_dir,
        attach=args.attach,
        attach_pid=attach_pid,
        maximize_main=not args.no_maximize,
        width=args.width,
        height=args.height,
        ai_timeout=args.ai_timeout,
        ollama_test=args.ollama_test,
        tray_wait=args.tray_wait,
    )
    session.run(selected=selected)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
