@echo off
setlocal
set "ROOT=%~dp0.."
REM Requires Git Bash or WSL with bash in PATH
bash "%ROOT%\scripts\start-ui.sh"
endlocal