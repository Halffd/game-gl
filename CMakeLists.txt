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

# Option to enable verbose messages
option(ENABLE_VERBOSE "Enable verbose makefile messages" ON)
if(ENABLE_VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

# Option to control warning levels
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

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

target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include_libs
    ${CMAKE_SOURCE_DIR}/include_libs/GL
    ${CMAKE_SOURCE_DIR}/include_libs/imgui
    ${CMAKE_SOURCE_DIR}/include_libs/imgui/backends
    ${CMAKE_SOURCE_DIR}/include_libs/imgui/misc/cpp
    ${CMAKE_SOURCE_DIR}/src
)
# Platform-specific settings
if(WIN32)
    # Windows: Use local files
    set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
    set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
	add_link_options(-static -static-libgcc -static-libstdc++)
    # Example for MSYS2
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
    #find_package(glfw3 REQUIRED STATIC)

    if(glfw3_FOUND)
    set(GLFW_LIBRARIES glfw3_static)  # Use the static version
    endif()

    # Check required directories
    foreach(REQUIRED_DIR IN ITEMS ${LIB_DIR} ${INCLUDE_DIR})
        if(NOT EXISTS ${REQUIRED_DIR})
            message(FATAL_ERROR "Required directory not found: ${REQUIRED_DIR}")
        endif()
    endforeach()

    # Windows include directories
    target_include_directories(${EXECUTABLE_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include_libs/windows
        ${GLFW_INCLUDE_DIRS}
    )
    add_library(glfw STATIC IMPORTED)
    set_target_properties(glfw PROPERTIES
            #IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/lib/glfw3.lib"
            IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/glfw3.lib"
    )
    # Add MSYS2 static libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE 
    -Wl,--start-group
    ${MSYS2_LIBRARIES}  # Add other static libraries here
    -Wl,--end-group
    )
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        glad
        glfw
    )

    # Set up irrKlang paths
    # set(IRRKLANG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/irrKlang/include") # Adjust the path if needed
    # set(IRRKLANG_LIB_DIR "${CMAKE_SOURCE_DIR}/lib/irrKlang/lib/Winx64-visualStudio") # MSVC-specific lib directory
    # set(IRRKLANG_LIB "${IRRKLANG_LIB_DIR}/irrKlang.lib") # Static or dynamic library for MSVC
    # set(IRRKLANG_DLL "${IRRKLANG_LIB_DIR}/irrKlang.dll") # DLL for dynamic linking (if needed)

    # Check if the required files exist
    #if(NOT EXISTS ${IRRKLANG_LIB})
    #    message(FATAL_ERROR "irrKlang library not found at: ${IRRKLANG_LIB}")
    #endif()

    # Include irrKlang directories
    #target_include_directories(${EXECUTABLE_NAME} PRIVATE ${IRRKLANG_INCLUDE_DIR})

    # Link irrKlang
    #target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    #    "${IRRKLANG_LIB}"
    #)
    # Ensure the DLL is available at runtime (for dynamic linking)
    #add_custom_command(TARGET game POST_BUILD
    #        COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #        "${IRRKLANG_DLL}" $<TARGET_FILE_DIR:game>)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/lib/glfw3.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>)

    target_link_libraries(${PROJECT_NAME} bcrypt)
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework Security")
else()
    # Linux: Use system packages
    
    find_package(PkgConfig REQUIRED)
    cmake_policy(SET CMP0072 NEW)
    #find_package(Freetype REQUIRED)
    find_package(glfw3 REQUIRED STATIC)

    add_library(lglfw STATIC IMPORTED)
    set_target_properties(lglfw PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/libglfw3.a"
    )
    # Linux include directories
    target_include_directories(${EXECUTABLE_NAME} PRIVATE
        ${GLFW_INCLUDE_DIRS}
        #${FREETYPE_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include_libs/GL
        ${CMAKE_SOURCE_DIR}/include_libs/imgui
        ${CMAKE_SOURCE_DIR}/include_libs/imgui/backends
        ${CMAKE_SOURCE_DIR}/include_libs/imgui/misc/cpp
    )
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        glad
        lglfw
        ${OPENGL_LIBRARIES}
        #${FREETYPE_LIBRARIES}
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
