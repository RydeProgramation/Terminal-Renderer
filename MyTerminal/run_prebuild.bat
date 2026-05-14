@echo off
set "SOL=%~1"
set "INT=%~2"

if "%SOL:~-1%"=="\" set "SOL=%SOL:~0,-1%"
if "%INT:~-1%"=="\" set "INT=%INT:~0,-1%"

python "%~dp0Pre_Build.py" "%SOL%" "%INT%"