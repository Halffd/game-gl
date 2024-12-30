@echo off
setlocal

:: Define directories and log file
set "BUILD_DIR=build"
set "WINDOWS_BUILD_DIR=%BUILD_DIR%\windows"
set "BIN_DIR=bin"
set "LOG_FILE=build.log"

:: Function to log messages
call :log "Starting Windows build..."

:: Clean up previous builds
if exist "%BIN_DIR%" (
    rmdir /s /q "%BIN_DIR%"
    call :log "Removed existing bin directory."
)

:: Create required directories
mkdir "%WINDOWS_BUILD_DIR%"
call :log "Created build directory: %WINDOWS_BUILD_DIR%"

:: Navigate to the build directory
pushd "%WINDOWS_BUILD_DIR%"
call :log "Navigated to build directory."

:: Run CMake and build
mingw64-cmake ../..
if errorlevel 1 (
    call :log "CMake configuration failed."
    exit /b 1
)

call :log "CMake configuration successful."

:: Build the project
make
if errorlevel 1 (
    call :log "Build failed."
    exit /b 1
)

call :log "Build completed successfully."

:: Navigate back
popd

:: Zip the output files
cd "%BIN_DIR%"
if exist "*" (
    zip -r "%BUILD_DIR%\zip\windows.zip" *
    call :log "Zipped binaries into windows.zip."
) else (
    call :log "No binaries found to zip."
)

exit /b 0

:log
echo %1 >> %LOG_FILE%
echo %1
exit /b 0