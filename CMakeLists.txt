cmake_minimum_required(VERSION 3.28.1)

# Define project name and output executable name
set(PROJECT_NAME "GameEngine")
set(EXECUTABLE_NAME "${PROJECT_NAME}")

project(${PROJECT_NAME}
    VERSION 1.0
    DESCRIPTION "OpenGL Game Engine with Havel Language"
    LANGUAGES C CXX
)

# ==============================================================================
# HAVEL LANGUAGE INTEGRATION
# ==============================================================================
option(ENABLE_HAVEL_LANG "Enable Havel language scripting support" ON)
set(HAVEL_ROOT "${CMAKE_SOURCE_DIR}/havel")

# Global settings
# Havel language requires C++23 for features like 'requires' constraints
set(CMAKE_CXX_STANDARD 23)
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

# ==============================================================================
# HAVEL LANGUAGE INTEGRATION (Link against pre-built libraries)
option(ENABLE_HAVEL_LANG "Enable Havel language scripting support" ON)
set(HAVEL_ROOT "${CMAKE_SOURCE_DIR}/havel")

# Find Qt6 for Havel's core dependencies
find_package(Qt6 COMPONENTS Core Widgets Gui QUIET)
if(Qt6_FOUND)
    message(STATUS "Qt6 found for Havel: ${Qt6_VERSION}")
endif()

set(HAVEL_SOURCES "")
# ==============================================================================
set(HAVEL_SOURCES "")
if(ENABLE_HAVEL_LANG)
    # Find pre-built Havel libraries
    set(HAVEL_BUILD_DIR "${HAVEL_ROOT}/build-debug")
    
    # Havel libraries in build-debug/
    set(HAVEL_CORE_LIB "${HAVEL_BUILD_DIR}/libhavel_core.a")
    set(HAVEL_LANG_LIB "${HAVEL_BUILD_DIR}/libhavel_lang.a")
    set(HAVEL_MODULES_LIB "${HAVEL_BUILD_DIR}/libhavel_modules.a")
    set(HAVEL_GUI_LIB "${HAVEL_BUILD_DIR}/libhavel_gui.a")
    set(HAVEL_EXT_RUNTIME_LIB "${HAVEL_BUILD_DIR}/libhavel_extension_runtime.a")
    
    # Verify libraries exist
    if(NOT EXISTS "${HAVEL_CORE_LIB}")
        message(FATAL_ERROR "Havel core library not found: ${HAVEL_CORE_LIB}. Run: cd havel && ./build.sh 6 build")
    endif()
    
    # Set libraries to link (order matters: lang depends on core)
    set(HAVEL_LIBRARIES
        "${HAVEL_LANG_LIB}"
        "${HAVEL_CORE_LIB}"
        "${HAVEL_MODULES_LIB}"
        "${HAVEL_GUI_LIB}"
        "${HAVEL_EXT_RUNTIME_LIB}"
    )
    
    # Additional required system libraries for Havel (Havel core has Qt dependencies)
    set(HAVEL_SYSTEM_LIBS dl pthread stdc++fs Xfixes Xrandr Xinerama Xcomposite Xtst Xi ncurses dbus-1 pcre2-8 pcre2-16 tesseract leptonica crypto ssl m ffi Qt6::Core Qt6::Widgets Qt6::Gui)
    
    message(STATUS "Havel Language: ENABLED")
    message(STATUS "  Core lib: ${HAVEL_CORE_LIB}")
    message(STATUS "  Lang lib: ${HAVEL_LANG_LIB}")
else()
    message(STATUS "Havel Language: DISABLED")
endif()

# Add glad.c explicitly
set(GLAD_SOURCE src/glad.c)

# Add tinyexpr.c explicitly
set(TINYEXPR_SOURCE include_libs/tinyexpr/tinyexpr.c)

# Add stb_image_write_impl.cpp explicitly
set(STB_IMAGE_WRITE_IMPL_SOURCE src/stb_image_write_impl.cpp)

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
add_executable(${PROJECT_NAME} 
    ${SOURCE_FILES} 
    ${IMGUI_FILES} 
    ${GLAD_SOURCE} 
    ${TINYEXPR_SOURCE} 
    ${STB_IMAGE_WRITE_IMPL_SOURCE}
)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)

# OpenCV (required for Havel OCR module)
if(ENABLE_HAVEL_LANG)
    find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include_libs
    ${CMAKE_SOURCE_DIR}/include_libs/GL
    ${CMAKE_SOURCE_DIR}/include_libs/imgui
    ${CMAKE_SOURCE_DIR}/include_libs/imgui/backends
    ${CMAKE_SOURCE_DIR}/include_libs/imgui/misc/cpp
    ${CMAKE_SOURCE_DIR}/src
    $<$<BOOL:${ENABLE_HAVEL_LANG}>:${HAVEL_ROOT}/src>
    $<$<BOOL:${ENABLE_HAVEL_LANG}>:${HAVEL_ROOT}/include>
    $<$<BOOL:${ENABLE_HAVEL_LANG}>:${HAVEL_ROOT}/src/havel-lang>
    $<$<BOOL:${OpenCV_FOUND}>:${OpenCV_INCLUDE_DIRS}>
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OPENGL_LIBRARIES}
    glfw
    assimp
    dl
    pthread
    X11
    $<$<BOOL:${ENABLE_HAVEL_LANG}>:${HAVEL_LIBRARIES}>
    $<$<BOOL:${ENABLE_HAVEL_LANG}>:${HAVEL_SYSTEM_LIBS}>
    $<$<BOOL:${OpenCV_FOUND}>:${OpenCV_LIBS}>
)

# Common compiler warnings
if(ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

# Havel Language compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<BOOL:${ENABLE_HAVEL_LANG}>:ENABLE_HAVEL_LANG>
    $<$<NOT:$<BOOL:${ENABLE_HAVEL_LANG}>>:DISABLE_HAVEL_LANG>
)

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

add_custom_command(
    TARGET GameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/textures
    ${CMAKE_BINARY_DIR}/textures
)

# Print message during build
message(STATUS "Resource directories will be copied to build directory")
