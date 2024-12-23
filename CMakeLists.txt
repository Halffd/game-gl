cmake_minimum_required(VERSION 3.28.1)
project(game 
    VERSION 1.0
    DESCRIPTION "OpenGL Game Engine"
    LANGUAGES C CXX
)

# Global settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.c")

# GLAD
add_library(glad STATIC src/glad.c)
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
target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_FILES})

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

    # Assimp (local)
    add_library(assimp SHARED IMPORTED)
    set_target_properties(assimp PROPERTIES
        IMPORTED_IMPLIB "${LIB_DIR}/assimp/libassimp.a"
        IMPORTED_LOCATION "${LIB_DIR}/assimp/assimp.dll"
    )

    # Copy DLLs to output directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LIB_DIR}/glfw3.dll"
        "${LIB_DIR}/assimp/assimp.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

    # Windows include directories
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${INCLUDE_DIR}
        ${INCLUDE_DIR}/mesh
        ${CMAKE_SOURCE_DIR}/imgui
        ${CMAKE_SOURCE_DIR}/imgui/backends
        ${CMAKE_SOURCE_DIR}/imgui/misc/cpp
    )

    # Windows linking
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glad
        glfw
        assimp
    )

else()
    # Linux: Use system packages
    find_package(PkgConfig REQUIRED)
    cmake_policy(SET CMP0072 NEW)
    find_package(OpenGL REQUIRED)
    find_package(imgui REQUIRED)
    
    # Required system packages
    pkg_check_modules(GLFW REQUIRED glfw3)
    pkg_check_modules(ASSIMP REQUIRED assimp)
    
    # Linux include directories
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${GLFW_INCLUDE_DIRS}
        ${ASSIMP_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include_libs      
        ${CMAKE_SOURCE_DIR}/include_libs/imgui
        ${CMAKE_SOURCE_DIR}/imgui/backends
        ${CMAKE_SOURCE_DIR}/imgui/misc/cpp
#        "/usr/include/imgui"
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/mesh
    )

    # Linux linking
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glad
        ${GLFW_LIBRARIES}
        ${ASSIMP_LIBRARIES}
        imgui
        OpenGL::GL
    )
endif()

include_directories(include)
include_directories(include/mesh)
include_directories(include_libs)

# Common compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Configure root directory
configure_file(
    ${CMAKE_SOURCE_DIR}/configuration/root_directory.h.in 
    ${CMAKE_BINARY_DIR}/configuration/root_directory.h
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/configuration)