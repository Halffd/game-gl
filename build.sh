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
if [ "$1" -ne 1 ]; then
    echo "Usage: $0 {all|linux|windows}"
    exit 1
fi

# Build process based on the argument provided
case "$1" in
    all)
        log "Building for both Linux and Windows (32-bit and 64-bit)..."
        # Build for Linux
        ./build.sh linux
        # Build for Windows (32-bit)
        #./build.sh windows 32
        # Build for Windows (64-bit)
        ./build.sh windows 64
        ;;
    linux)
        log "Starting Linux build..."
        rm -rf "$BIN_DIR/linux/*"  # Clean previous builds
        mkdir -p "$BUILD_DIR/linux"  # Ensure Linux build directory exists
        cd "$BUILD_DIR/linux"
        
        # Configure and build
        cmake ../..  # Adjust the path as necessary
        cmake --build .  # Compile the project
        cd ../..
        # Copy Linux binaries to their directory using rsync
        rsync -a --exclude=linux  --exclude=windows "$BUILD_DIR/" "$BIN_DIR/linux/"  # Use rsync to copy files
        log "Linux build completed."

        # Zip the Linux binaries if they exist
        if [ -d "$BIN_DIR/linux" ] && [ "$(ls -A "$BIN_DIR/linux")" ]; then
            zip -r "$BUILD_DIR/zip/linux.zip" "$BIN_DIR/linux"
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
  #      rm -r "$BIN_DIR*"  # Clean previous binaries
        mkdir -p "$BUILD_DIR"  # Ensure build directory exists
        cd "$BUILD_DIR"
        pwd
        # Configure the project based on architecture
        cmake -DCMAKE_SYSTEM_NAME=Windows \
              -DCMAKE_C_COMPILER=${TOOLCHAIN}-gcc \
              -DCMAKE_CXX_COMPILER=${TOOLCHAIN}-g++ \
              -DCMAKE_FIND_ROOT_PATH=/usr/${TOOLCHAIN} ..  # Adjust as necessary

        cmake --build .  # Compile the project
        #cd ..

        # Copy Windows binaries to their directory using rsync
        rsync -a --exclude=zip --exclude=linux --exclude=linux --exclude=windows "$BUILD_DIR/" "$BIN_DIR/windows/$ARCH/"  # Use rsync to copy files
        log "Windows $ARCH-bit build completed."

        # Zip the Windows binaries if they exist
        if [ -d "$BIN_DIR/windows/$ARCH" ] && [ "$(ls -A "$BIN_DIR/windows/$ARCH")" ]; then
            zip -r "$BUILD_DIR/zip/windows_${ARCH}.zip" "$BIN_DIR/windows/$ARCH"
            log "Windows $ARCH-bit binaries zipped into windows_${ARCH}.zip."
        else
            log "No Windows $ARCH-bit binaries found to zip."
        fi
        ;;
    *)
        echo "Invalid argument: $1"
        echo "Usage: $0 {all|linux|windows}"
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

# Ensure the zip directory exists and move zip files into it
mkdir -p "$BIN_DIR/zip"
cp -r "$BUILD_DIR/zip" "$BIN_DIR"  # Move zip files to the bin directory
log "Build process completed successfully."
