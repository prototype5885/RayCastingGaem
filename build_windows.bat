@echo off
setlocal enabledelayedexpansion

set TARGET=x86_64-w64-mingw32
set SDL2_VERSION=2.32.8
set OUTPUT=game.exe
set BUILD_DIR=build_windows

echo Compiling...

if not exist "%BUILD_DIR%\textures" xcopy textures "%BUILD_DIR%\textures" /E /I /H /K /Y

echo Building Docker image...
docker build -f dockerfile_windows -t sdl-builder --build-arg SDL2_VERSION=%SDL2_VERSION% --build-arg TARGET=%TARGET% .

if %ERRORLEVEL% neq 0 (
    echo Docker build failed
    pause
    exit 1
)

set FILES=
for %%f in (src\*.cpp) do (
    set FILES=!FILES! /app/src/%%~nxf
)

echo Running compilation...
docker run --rm -v "%cd%:/app" -w /app sdl-builder bash -c "%TARGET%-g++ -Wall -Wextra -O3 %FILES% -o %BUILD_DIR%/%OUTPUT% -lmingw32 -lSDL2main -lSDL2 -static-libstdc++"

if %ERRORLEVEL% EQU 0 (
    echo Build successful into folder: %BUILD_DIR%
) else (
    echo Build failed.
)

pause