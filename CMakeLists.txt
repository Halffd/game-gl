cmake_minimum_required(VERSION 3.28.1)

# Define project name and output executable name
set(PROJECT_NAME "GameEngine")
set(EXECUTABLE_NAME "${PROJECT_NAME}")

project(${PROJECT_NAME}
    VERSION 1.0
    DESCRIPTION "OpenGL Game Engine"
    LANGUAGES C CXX
)

# Global settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_FIND_STATIC_PREFERENCES TRUE)
set(BUILD_SHARED_LIBS OFF)  # Ensure shared libraries are not built
# Default debug, -DRelease: release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Option to enable verbose messages
option(ENABLE_VERBOSE "Enable verbose makefile messages" ON)
if(ENABLE_VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

# Option to control warning levels
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

file(GLOB_RECURSE SOURCE_FILES
    src/*.cpp
    src/game/*.cpp
    src/render/*.cpp
    src/ui/*.cpp
    src/util/*.cpp
    src/effects/*.cpp
    src/asset/*.cpp
)

# Add glad.c explicitly
set(GLAD_SOURCE src/glad.c)

# ImGui files
set(IMGUI_FILES
    include_libs/imgui/imgui.cpp
    include_libs/imgui/imgui_demo.cpp
    include_libs/imgui/imgui_draw.cpp
    include_libs/imgui/imgui_tables.cpp
    include_libs/imgui/imgui_widgets.cpp
    include_libs/imgui/backends/imgui_impl_glfw.cpp
    include_libs/imgui/backends/imgui_impl_opengl3.cpp
    include_libs/imgui/misc/cpp/imgui_stdlib.cpp
)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${IMGUI_FILES} ${GLAD_SOURCE})

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include_libs
    ${CMAKE_SOURCE_DIR}/include_libs/GL
    ${CMAKE_SOURCE_DIR}/include_libs/imgui
    ${CMAKE_SOURCE_DIR}/include_libs/imgui/backends
    ${CMAKE_SOURCE_DIR}/include_libs/imgui/misc/cpp
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OPENGL_LIBRARIES}
    glfw
    assimp
    dl
    pthread
    X11
)

# Common compiler warnings
if(ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

# Define the directories to copy
set(DIRECTORIES_TO_COPY
    textures
    levels
    shaders
    audio
)

# Create the bin directory if it doesn't exist
set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${BIN_DIR})

# Copy the directories to the bin directory
foreach(DIR ${DIRECTORIES_TO_COPY})
    file(COPY ${CMAKE_SOURCE_DIR}/${DIR} DESTINATION ${BIN_DIR})
endforeach()

# Add commands to copy resource directories
add_custom_command(
    TARGET GameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    ${CMAKE_BINARY_DIR}/shaders
)

add_custom_command(
    TARGET GameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/models
    ${CMAKE_BINARY_DIR}/models
)

# Print message during build
message(STATUS "Resource directories will be copied to build directory")
