@echo off

echo Building Docker image...
docker build -t my-alpine-mingw-build .
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to build Docker image.
    pause
    exit /b %ERRORLEVEL%
)

echo Creating local build directory...
if exist build rmdir /s /q build
mkdir build 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to create build directory.
    pause
    exit /b %ERRORLEVEL%
)

echo Running temporary container with mounted src and build directories...
docker run --rm -v "%CD%\src:/app/src" -v "%CD%\build:/app/build" my-alpine-mingw-build
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to run Docker container.
    pause
    exit /b %ERRORLEVEL%
)

echo Copying assets folder...
xcopy "assets" "build\assets" /E /Y
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to copy assets folder.
    pause
    exit /b %ERRORLEVEL%
)

echo Done. Build output is in %CD%\build
pause