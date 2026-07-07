#!/usr/bin/env python3
"""Copy portable helper files with correct Unicode filenames on Windows."""

from __future__ import annotations

import shutil
import sys
from pathlib import Path

README_NAME = "使用说明.txt"
START_BAT_NAME = "启动 ToDoList.bat"
LEGACY_BAD_README = "浣跨敤璇存槑.txt"


def install_helper_files(out_dir: Path, script_dir: Path) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)

    shutil.copy2(script_dir / "portable-readme.txt", out_dir / README_NAME)
    shutil.copy2(script_dir / "portable-start.bat", out_dir / START_BAT_NAME)

    legacy_readme = out_dir / LEGACY_BAD_README
    if legacy_readme.exists():
        legacy_readme.unlink()

    for bat in out_dir.glob("*ToDoList.bat"):
        if bat.name != START_BAT_NAME:
            bat.unlink()

    manifest_path = out_dir / "file-manifest.txt"
    if not manifest_path.exists():
        return

    lines: list[str] = []
    seen: set[str] = set()
    for raw in manifest_path.read_text(encoding="utf-8-sig").splitlines():
        line = raw.strip().lstrip("\ufeff")
        if not line or line.startswith("#"):
            continue
        if line in {LEGACY_BAD_README, README_NAME, START_BAT_NAME}:
            continue
        if line.endswith("ToDoList.bat"):
            continue
        if line not in seen:
            lines.append(line)
            seen.add(line)

    lines.append(README_NAME)
    lines.append(START_BAT_NAME)
    lines.sort(key=str.lower)
    manifest_path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def main() -> int:
    if len(sys.argv) != 2:
        print("Usage: copy-portable-helper-files.py <portable-output-dir>", file=sys.stderr)
        return 1

    out_dir = Path(sys.argv[1]).resolve()
    script_dir = Path(__file__).resolve().parent
    install_helper_files(out_dir, script_dir)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
