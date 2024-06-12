#ifndef ROOT_H
#define ROOT_H

#include <string>
#include <cstring> // for std::strcpy

#define BUFFER_SIZE 1024

struct Path {
    std::string root;

    Path(const std::string& rootDir) : root(rootDir) {}

    const char* file(const std::string& filename) const {
        return fullPath(root + "/" + filename);
    }

    const char* shader(const std::string& filename) const {
        return fullPath(root + "/shaders/" + filename);
    }

    const char* texture(const std::string& filename) const {
        return fullPath(root + "/textures/" + filename);
    }

    const char* fullPath(const std::string& fullPath) const {
        // Allocate memory dynamically for buffer
        char* buffer = new char[BUFFER_SIZE];
        
        // Copy the content of fullPath to buffer
        std::strcpy(buffer, fullPath.c_str());
        
        // Return the pointer to buffer
        return buffer;
    }
};

static Path fs("");

#endif // ROOT_H
