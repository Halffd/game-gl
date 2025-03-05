<<<<<<< HEAD
# OpenGL Project
Learning OpenGL (C++)
=======
# NeuroJam OpenGL Game Project

An OpenGL (C++) game developed for the NeuroJam 2024.

## Table of Contents

- [Overview](#overview)
- [Dependencies](#dependencies)
- [Building](#building)
- [Running the Game](#running-the-game)
- [Cleaning the Project](#cleaning-the-project)

## Overview

This project is designed to compete in the NeuroJam 2024. It utilizes OpenGL for rendering graphics and GLFW for window management and input handling.

## Dependencies

To build this project, ensure you have the following library installed:

- `glfw`
  - On **Ubuntu**: `libglfw-dev` and `glfw`
  - On **Fedora**: `glfw-devel` and `glfw`
  - On **Arch**: `glfw`
  - On **Windows MSYS (MinGW)**: Install the GLFW package using:
    ```bash
    pacman -S mingw-w64-x86_64-glfw
    ```

## Building

To build the project, follow these steps:

1. **Install Dependencies**:
   Make sure you have the GLFW library installed as described above.

2. **Create a Build Directory**:
   Open a terminal and navigate to the project root directory. Then run:

   ```bash
   mkdir build
   cd build
   ```
Run CMake:
Generate the makefiles using CMake:
```bash
cmake ..
```
Build the Project:
Compile the project with:
```bash
cmake --build .
```
Running the Game
After building the project, you can run the game with the following command:

```bash
../bin/NeuroMonsters
```
Cleaning the Project
To clean up the project and remove build artifacts, you can delete the build directory:

```bash
rm -rf build
```
or
```bash
cd build
cmake --build . --target clean
```
>>>>>>> jam/master
