# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name  
project(MyProject)

# Check the architecture type
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit configuration
    message(STATUS "Configuring for 64-bit")
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
    set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
else()
    # 32-bit configuration
    message(STATUS "Configuring for 32-bit")
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
    set(CMAKE_FIND_ROOT_PATH /usr/i686-w64-mingw32)
endif()

# Search for programs in the build host directories
set(CMAKE_FIND_PROGRAMS ${CMAKE_FIND_PROGRAMS})

# Adjust the default behavior of the find commands
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Add your executable or library
add_executable(MyExecutable main.cpp)  # Replace main.cpp with your source files
