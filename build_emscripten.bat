@echo off
setlocal enabledelayedexpansion

SET OUTPUT_NAME=emscripten
SET BUILD_DIR=build_emscripten

IF EXIST emscripten\index.html (
    echo Copying index.html to %BUILD_DIR%...
    mkdir %BUILD_DIR%
    copy emscripten\index.html %BUILD_DIR%\
) ELSE (
    echo ERROR: index.html was not found in the emscripten folder
    pause
    exit 1
)

set FILES=
for %%f in (src\*.cpp) do (
    set FILES=!FILES! /app/src/%%~nxf
)

SET EMCC_COMMAND="em++ -O3  %FILES% -o %BUILD_DIR%/%OUTPUT_NAME%.js -s USE_SDL=2 --preload-file textures"

echo "Running Emscripten compilation using Docker..."
docker run --rm -v "%cd%:/app" -w /app emscripten/emsdk bash -c %EMCC_COMMAND%

if %ERRORLEVEL% EQU 0 (
    echo Build successful into folder: %BUILD_DIR%
) else (
    echo Build failed.
)

pause