@echo off
setlocal
cd /d "%~dp0\.."

where python >nul 2>&1
if errorlevel 1 (
  echo Python not found. Install Python 3.10+ and add to PATH.
  exit /b 1
)

python -m pip install -r scripts\requirements-screenshots.txt
if errorlevel 1 exit /b 1

set "APP_DIR=build\Desktop_Qt_5_15_2_MinGW_64_bit-Release"
if exist "%APP_DIR%\release\ToDoList.exe" set "APP_DIR=%APP_DIR%\release"

python scripts\capture-readme-screenshots.py --attach --app-dir "%APP_DIR%" %*
endlocal
