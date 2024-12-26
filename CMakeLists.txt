cmake_minimum_required(VERSION 3.28.1)

# Define project name and output executable name
set(PROJECT_NAME "game")
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

# Option to enable verbose messages
option(ENABLE_VERBOSE "Enable verbose makefile messages" ON)
if(ENABLE_VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

# Option to control warning levels
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

# Source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.c")

# GLAD
add_library(glad STATIC src/glad.c)
target_include_directories(glad PRIVATE 
    ${CMAKE_SOURCE_DIR}/include_libs
    ${CMAKE_SOURCE_DIR}/include_libs/GL
)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# ImGui (local)
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
target_sources(${EXECUTABLE_NAME} PRIVATE ${IMGUI_FILES})

# Platform-specific settings
if(WIN32)
    # Windows: Use local files
    set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
    set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

    # Check required directories
    foreach(REQUIRED_DIR IN ITEMS ${LIB_DIR} ${INCLUDE_DIR})
        if(NOT EXISTS ${REQUIRED_DIR})
            message(FATAL_ERROR "Required directory not found: ${REQUIRED_DIR}")
        endif()
    endforeach()
    
    # GLFW (local)
    add_library(glfw SHARED IMPORTED)
    set_target_properties(glfw PROPERTIES
        IMPORTED_IMPLIB "${LIB_DIR}/libglfw3.a"
        IMPORTED_LOCATION "${LIB_DIR}/glfw3.dll"
    )

    # Windows include directories
    target_include_directories(${EXECUTABLE_NAME} PRIVATE
        ${INCLUDE_DIR}
        ${INCLUDE_DIR}/mesh
        ${CMAKE_SOURCE_DIR}/include_libs
        ${CMAKE_SOURCE_DIR}/include_libs/GL
        ${CMAKE_SOURCE_DIR}/include_libs/windows  # Local Assimp headers
        ${CMAKE_SOURCE_DIR}/include_libs/windows/assimp  # Local Assimp headers
        ${CMAKE_SOURCE_DIR}/include_libs/imgui
        ${CMAKE_SOURCE_DIR}/include_libs/imgui/backends
        ${CMAKE_SOURCE_DIR}/include_libs/imgui/misc/cpp
    )

    # Copy DLLs to output directory
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LIB_DIR}/glfw3.dll"
        $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
    )
    # Set up Assimp paths
    set(ASSIMP "${CMAKE_SOURCE_DIR}/lib/libassimp.dll.a")

    # Verify files exist
    if(NOT EXISTS ${ASSIMP})
        message(FATAL_ERROR "Assimp library not found at: ${ASSIMP_LIB}")
    endif()

    # Set up Assimp as imported library
    add_library(assimp STATIC IMPORTED)
    set_target_properties(assimp PROPERTIES
        IMPORTED_LOCATION "${ASSIMP}"
    )

    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        glad
        glfw
        assimp
    )
else()
    # Linux: Use system packages
    find_package(PkgConfig REQUIRED)
    cmake_policy(SET CMP0072 NEW)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(assimp REQUIRED)

    # Linux include directories
    target_include_directories(${EXECUTABLE_NAME} PRIVATE
        ${OPENGL_INCLUDE_DIR}
        ${GLFW_INCLUDE_DIRS}
        ${ASSIMP_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/mesh
        ${CMAKE_SOURCE_DIR}/include_libs
        ${CMAKE_SOURCE_DIR}/include_libs/GL
        ${CMAKE_SOURCE_DIR}/include_libs/imgui
        ${CMAKE_SOURCE_DIR}/include_libs/imgui/backends
        ${CMAKE_SOURCE_DIR}/include_libs/imgui/misc/cpp
    )

    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        glad
        glfw
        ${OPENGL_LIBRARIES}
        ${ASSIMP_LIBRARIES}
    )
endif()

# Common compiler warnings
if(ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(${EXECUTABLE_NAME} PRIVATE /W4)
    else()
        target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

# Configure root directory
configure_file(
    ${CMAKE_SOURCE_DIR}/configuration/root_directory.h.in 
    ${CMAKE_BINARY_DIR}/configuration/root_directory.h
)
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_BINARY_DIR}/configuration)