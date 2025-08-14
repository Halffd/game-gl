#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status
set -o pipefail  # Prevent errors in a pipeline from being masked

LOG_FILE="build.log"  # Log file to record build processes
BUILD_DIR="build"      # Base build directory
BIN_DIR="bin"          # Directory for binary outputs

# Function to log messages with timestamps
log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$LOG_FILE"
}

# Create necessary directories for builds and zips
mkdir -p "$BIN_DIR/linux" "$BIN_DIR/windows/32" "$BIN_DIR/windows/64" "$BUILD_DIR/zip"

# Check for command-line arguments
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 {all|linux|windows [32|64]}"
    exit 1
fi

# Build process based on the argument provided
case "$1" in
    all)
        log "Building for both Linux and Windows (32-bit and 64-bit)..."
        # Build for Linux
        "$0" linux
        # Build for Windows (64-bit)
        "$0" windows 64
        ;;
    linux)
        log "Starting Linux build..."
        rm -rf "$BIN_DIR/linux/"*  # Clean previous builds
        mkdir -p "$BUILD_DIR/linux"  # Ensure Linux build directory exists
        cd "$BUILD_DIR/linux"
        
        # Configure and build
        cmake ../..  # Adjust the path as necessary
        cmake --build .  # Compile the project
        cd ../..
        
        # Copy Linux binaries to their directory using rsync
        if [ -d "$BUILD_DIR/linux" ]; then
            rsync -av --exclude='*.o' --exclude='*.cmake' --exclude='CMakeFiles' --exclude='Makefile' "$BUILD_DIR/linux/" "$BIN_DIR/linux/"
        fi
        log "Linux build completed."

        # Zip the Linux binaries if they exist
        if [ -d "$BIN_DIR/linux" ] && [ "$(ls -A "$BIN_DIR/linux")" ]; then
            cd "$BIN_DIR" && zip -r "../$BUILD_DIR/zip/linux.zip" linux/
            cd ..
            log "Linux binaries zipped into linux.zip."
        else
            log "No Linux binaries found to zip."
        fi
        ;;
    windows)
        if [ "$#" -ne 2 ]; then
            echo "Usage: $0 windows {32|64}"
            exit 1
        fi
        
        if [ "$2" == "32" ]; then
            ARCH="32"
            TOOLCHAIN="i686-w64-mingw32"
        elif [ "$2" == "64" ]; then
            ARCH="64"
            TOOLCHAIN="x86_64-w64-mingw32"
        else
            echo "Invalid architecture: $2. Use '32' or '64'."
            exit 1
        fi
        
        log "Starting Windows $ARCH-bit build..."
        rm -rf "$BIN_DIR/windows/$ARCH/"*  # Clean previous binaries
        mkdir -p "$BUILD_DIR/windows_$ARCH"  # Ensure build directory exists
        cd "$BUILD_DIR/windows_$ARCH"
        
        # Configure the project based on architecture
        cmake -DCMAKE_SYSTEM_NAME=Windows \
              -DCMAKE_C_COMPILER=${TOOLCHAIN}-gcc \
              -DCMAKE_CXX_COMPILER=${TOOLCHAIN}-g++ \
              -DCMAKE_FIND_ROOT_PATH=/usr/${TOOLCHAIN} \
              -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
              -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
              -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY ../..

        cmake --build .  # Compile the project
        cd ../..

        # Copy Windows binaries to their directory using rsync
        if [ -d "$BUILD_DIR/windows_$ARCH" ]; then
            rsync -av --exclude='*.o' --exclude='*.cmake' --exclude='CMakeFiles' --exclude='Makefile' "$BUILD_DIR/windows_$ARCH/" "$BIN_DIR/windows/$ARCH/"
        fi
        log "Windows $ARCH-bit build completed."

        # Zip the Windows binaries if they exist
        if [ -d "$BIN_DIR/windows/$ARCH" ] && [ "$(ls -A "$BIN_DIR/windows/$ARCH")" ]; then
            cd "$BIN_DIR/windows" && zip -r "../../$BUILD_DIR/zip/windows_${ARCH}.zip" "$ARCH/"
            cd ../..
            log "Windows $ARCH-bit binaries zipped into windows_${ARCH}.zip."
        else
            log "No Windows $ARCH-bit binaries found to zip."
        fi
        ;;
    *)
        echo "Invalid argument: $1"
        echo "Usage: $0 {all|linux|windows [32|64]}"
        exit 1
        ;;
esac

# Create a symbolic link to the Linux game executable if it exists
if [ -f "$BIN_DIR/linux/game" ]; then
    ln -sf "$BIN_DIR/linux/game" "$BIN_DIR/game"
    log "Created symbolic link for the Linux game executable."
else
    log "No Linux game executable found to link."
fi

# Move zip files to bin directory
if [ -d "$BUILD_DIR/zip" ] && [ "$(ls -A "$BUILD_DIR/zip")" ]; then
    mkdir -p "$BIN_DIR/zip"
    mv "$BUILD_DIR/zip"/* "$BIN_DIR/zip/"
    log "Zip files moved to bin directory."
fi

log "Build process completed successfully."