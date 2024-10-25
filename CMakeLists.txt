cmake_minimum_required(VERSION 3.28.1)
# Rest of your CMakeLists.txt file
project(game LANGUAGES C CXX)

# find the required packages

# Find Assimp
find_package(assimp REQUIRED)

# Find ZLIB
#find_package(ZLIB REQUIRED)

# find_package(GLM REQUIRED)
# message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
# find_package(GLFW3 REQUIRED)
# message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
# find_package(ASSIMP REQUIRED)
# message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")
# Set the C and C++ compilers explicitly
#set(CMAKE_C_COMPILER "gcc")
#set(CMAKE_CXX_COMPILER "g++")
# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directory for binaries 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add the GLAD library (specifying LANGUAGE C explicitly)
add_library(glad STATIC src/glad.c)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# Collect all source files
file(GLOB_RECURSE source_files "src/*.cpp")
# Gather all ImGui source files
file(GLOB_RECURSE imgui_sources "imgui/imgui.cpp"
        "imgui/imgui_demo.cpp"
        "imgui/imgui_draw.cpp"
        "imgui/imgui_tables.cpp"
        "imgui/imgui_widgets.cpp"
)
file(GLOB_RECURSE imgui_backends 
    "imgui/backends/imgui_impl_glfw.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
    "imgu8i/misc/cpp/imgui_stdlib.cpp"
)

# Add executable
add_executable(game ${source_files} ${imgui_sources} ${imgui_backends})
message(STATUS "Include.")

# Include directories
include_directories(include)
include_directories(include/mesh)
include_directories(include_libs)
include_directories(imgui)
include_directories(imgui/backends)
include_directories(imgui/misc/cpp)
#include_directories(include_libs/zlib)

# Configure file for the root directory
set(logl_root "${CMAKE_SOURCE_DIR}")
configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)
# Include directories
include_directories(${ASSIMP_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})


# Define an array of libraries to link
#set(LIBS glad "${CMAKE_SOURCE_DIR}/lib/glfw3.lib")
message(STATUS "LIBS.")
# Link libraries
target_link_libraries(game PRIVATE ${ASSIMP_LIBRARIES})
#target_link_libraries(game PRIVATE ZLIB::ZLIB)

# Add the GLFW DLL as an imported library
add_library(glfw3dll SHARED IMPORTED)
set_target_properties(glfw3dll PROPERTIES
        IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/lib/libglfw3.a"
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/glfw3.dll"
)

# Ensure the DLL is available at runtime (for dynamic linking)
#add_custom_command(TARGET game POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different
#        "${ASSIMP_DIR}/lib/assimpd.dll" $<TARGET_FILE_DIR:game>)

# Specify the path to the Assimp library
set(ASSIMP_DIR "${CMAKE_SOURCE_DIR}/lib/assimp")
message(STATUS "Assimp ${ASSIMP_DIR}.")

# Check if the file exists
if(EXISTS ${ASSIMP_DIR})
    message(STATUS "Dir ${ASSIMP_DIR} exists.")
else()
    message(WARNING "Dir ${ASSIMP_DIR} does not exist.")
endif()
set(ASSIMP "${ASSIMP_DIR}/libassimp.a")
# Check if the file exists
if(EXISTS ${ASSIMP})
    message(STATUS "Dir ${ASSIMP} exists.")
else()
    message(WARNING "Dir ${ASSIMP} does not exist.")
endif()

# Define an array of libraries to link
set(LIBS glad glfw3dll)

# Link libraries
target_link_libraries(game PRIVATE ${LIBS})

# Include directories for the game target
target_include_directories(game PRIVATE ${INCLUDE})

