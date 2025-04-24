@echo off
setlocal

:: === Configuration ===
set "DLL_NAME=d3d8.dll"
set "GAME_EXE=Trucker.exe"
set "GAME_ROOT=C:\Program Files (x86)\Mirage Interactive\Trucker"

:: Set BUILD_TYPE from argument if provided, otherwise default to x86-MinSizeRel
if "%~1"=="" (
    set "SOURCE_DLL_ROOT=out\build\x86-MinSizeRel\bin\"
) else (
    set "SOURCE_DLL_ROOT=%~1"
)

echo Working directory: %CD%

:: === Paths ===
set "SOURCE_DLL=%SOURCE_DLL_ROOT%\%DLL_NAME%"
set "TARGET_DLL=%GAME_ROOT%\%DLL_NAME%"
set "GAME_EXECUTABLE=%GAME_ROOT%\%GAME_EXE%"

:: === Delete the old DLL ===
echo Deleting existing DLL: %TARGET_DLL%
if exist "%TARGET_DLL%" (
    del "%TARGET_DLL%"
)

:: === Copy new DLL ===
echo Copying new DLL from %SOURCE_DLL% to %TARGET_DLL%
copy "%SOURCE_DLL%" "%TARGET_DLL%" >nul
if errorlevel 1 (
    echo Failed to copy DLL. Aborting.
    exit /b 1
)

:: === Run the game ===
echo Running the game: "%GAME_ROOT%/%GAME_EXE%"
cd /D "%GAME_ROOT%"
%GAME_EXE%

endlocal
