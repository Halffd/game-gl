cmake_minimum_required(VERSION 3.28.1)
# Rest of your CMakeLists.txt file

project(game LANGUAGES C CXX)

# find the required packages
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

# Add executable
add_executable(game ${source_files}
        src/game)

# Include directories
include_directories(include)
include_directories(include/mesh)
include_directories(include_libs)
# Configure file for the root directory
set(logl_root "${CMAKE_SOURCE_DIR}")
configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

# Define an array of libraries to link
#set(LIBS glad "${CMAKE_SOURCE_DIR}/lib/glfw3.lib")

# Add the GLFW DLL as an imported library
add_library(glfw3dll SHARED IMPORTED)
set_target_properties(glfw3dll PROPERTIES
        IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/lib/libglfw3.a"
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/glfw3.dll"
)

# Define an array of libraries to link
set(LIBS glad glfw3dll)

# Link libraries
target_link_libraries(game PRIVATE ${LIBS})

# Include directories for the game target
target_include_directories(game PRIVATE ${INCLUDE})

