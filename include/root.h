#ifndef ROOT_H
#define ROOT_H

#include <string>
#include <cstring> // for std::strcpy
#include <iostream>

#define BUFFER_SIZE 1024


inline void safe_strcpy(char* dest, size_t dest_size, const char* src) {
#if defined(_MSC_VER) // Microsoft Visual C++
    strcpy_s(dest, dest_size, src);
#elif defined(__STDC_LIB_EXT1__) // C11 with bounds checking
    strcpy_s(dest, dest_size, src);
#else // POSIX systems (Linux, macOS, etc.)
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0'; // Ensure null termination
#endif
}
struct Path {
    std::string root;

    Path(const std::string& rootDir) : root(rootDir) {}

    const char* file(const std::string& filename) const {
        return fullPath(root.empty() ? filename : root + "/" + filename);
    }

    const char* shader(const std::string& filename) const {
        std::string path = (root.empty() ? "shaders/" : root + "/shaders/") + filename;
        return fullPath(path);
    }

    const char* texture(const std::string& filename) const {
        return fullPath((root.empty() ? "textures/" : root + "/textures/") + filename);
    }
    const char* fullPath(const std::string& fullPath) const {
        // Allocate memory dynamically for buffer
        char* buffer = new char[BUFFER_SIZE];

        // Copy the content of fullPath to buffer
        safe_strcpy(buffer, BUFFER_SIZE, fullPath.c_str());

        // Return the pointer to buffer
        return buffer;
    }
};

static Path fs("");

#endif // ROOT_H
