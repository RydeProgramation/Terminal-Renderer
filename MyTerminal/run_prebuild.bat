@echo off
set "SOL=%~1"
set "INT=%~2"

:: Vérifie si les arguments sont vides
if "%SOL%"=="" (
    echo.
    echo [ERREUR] Le chemin SOL est manquant.
    echo Utilisation :
    echo    script.bat "CheminSolution" "CheminIntermediaire"
    echo.
    pause
    exit /b 1
)

if "%INT%"=="" (
    echo.
    echo [ERREUR] Le chemin INT est manquant.
    echo Utilisation :
    echo    script.bat "CheminSolution" "CheminIntermediaire"
    echo.
    pause
    exit /b 1
)

:: Supprime le slash final si présent
if "%SOL:~-1%"=="\" set "SOL=%SOL:~0,-1%"
if "%INT:~-1%"=="\" set "INT=%INT:~0,-1%"

python "%~dp0Pre_Build.py" "%SOL%" "%INT%"