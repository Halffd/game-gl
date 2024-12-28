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
	add_link_options(-static -static-libgcc -static-libstdc++)

    # Check required directories
    foreach(REQUIRED_DIR IN ITEMS ${LIB_DIR} ${INCLUDE_DIR})
        if(NOT EXISTS ${REQUIRED_DIR})
            message(FATAL_ERROR "Required directory not found: ${REQUIRED_DIR}")
        endif()
    endforeach()

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
    add_library(glfw SHARED IMPORTED)
    set_target_properties(glfw PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/lib/glfw3.lib"
            IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/glfw3.dll"
    )
    if(MSVC)

    # Set up Assimp as imported library
    add_library(assimp STATIC IMPORTED)
        set_target_properties(assimp PROPERTIES
            IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/assimp/msvc/assimp-vc143-mt.lib"
        )
    else()
    # Set up Assimp paths
    set(ASSIMP_LIB "${CMAKE_SOURCE_DIR}/lib/libassimp.dll.a")

    # Verify files exist
    if(NOT EXISTS ${ASSIMP_LIB})
        message(FATAL_ERROR "Assimp library not found at: ${ASSIMP_LIB}")
    endif()

    # Set up Assimp as imported library
    add_library(assimp STATIC IMPORTED)
    set_target_properties(assimp PROPERTIES
        IMPORTED_LOCATION "${ASSIMP_LIB}"
    )
    endif()

    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        glad
        assimp
        glfw
    )

    # Set up irrKlang paths
    set(IRRKLANG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/irrKlang/include") # Adjust the path if needed
    set(IRRKLANG_LIB_DIR "${CMAKE_SOURCE_DIR}/lib/irrKlang/lib/Winx64-visualStudio") # MSVC-specific lib directory
    set(IRRKLANG_LIB "${IRRKLANG_LIB_DIR}/irrKlang.lib") # Static or dynamic library for MSVC
    set(IRRKLANG_DLL "${IRRKLANG_LIB_DIR}/irrKlang.dll") # DLL for dynamic linking (if needed)

    # Check if the required files exist
    if(NOT EXISTS ${IRRKLANG_LIB})
        message(FATAL_ERROR "irrKlang library not found at: ${IRRKLANG_LIB}")
    endif()

    # Include irrKlang directories
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${IRRKLANG_INCLUDE_DIR})

    # Link irrKlang
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        "${IRRKLANG_LIB}"
    )
    # Ensure the DLL is available at runtime (for dynamic linking)
    add_custom_command(TARGET game POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${IRRKLANG_DLL}" $<TARGET_FILE_DIR:game>)
    add_custom_command(TARGET game POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/lib/glfw3.dll" $<TARGET_FILE_DIR:game>)

    add_custom_command(TARGET game POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/lib/assimp/msvc/assimp-vc143-mt.dll" $<TARGET_FILE_DIR:game>)
else()
    # Linux: Use system packages
    
    find_package(PkgConfig REQUIRED)
    cmake_policy(SET CMP0072 NEW)
    find_package(OpenGL REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glfw3 REQUIRED)

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

    # Linux specific settings for irrKlang
    set(IRRKLANG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/irrKlang/include")
    set(IRRKLANG_LIB_DIR "${CMAKE_SOURCE_DIR}/lib/irrKlang/bin/linux-gcc-64")
    set(IRRKLANG_LIB "${IRRKLANG_LIB_DIR}/libIrrKlang.so") # Dynamic library for Linux

    # Check if the required files exist
    if(NOT EXISTS ${IRRKLANG_LIB})
        message(FATAL_ERROR "irrKlang library not found at: ${IRRKLANG_LIB}")
    endif()

    # Include irrKlang directories
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${IRRKLANG_INCLUDE_DIR})

    # Link irrKlang
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        glad
        glfw
        ${OPENGL_LIBRARIES}
        ${ASSIMP_LIBRARIES}
        ${IRRKLANG_LIB}
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
