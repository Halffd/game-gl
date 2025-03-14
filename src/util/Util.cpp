#include "Util.h"
#include <glad/glad.h>
#include <cstring> // For strstr
#include <execinfo.h> // For backtrace
#include <cstdlib> // For free

// Enhanced error checking with stack trace
void glCheckError(const char *file, int line) {
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "GL_INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "GL_STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "GL_STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                error = "Unknown error";
                break;
        }

        std::cout << "OpenGL error: " << error << " at " << file << ":" << line << std::endl;
        
        // Get stack trace
        void* callstack[128];
        int frames = backtrace(callstack, 128);
        char** strs = backtrace_symbols(callstack, frames);
        
        std::cout << "Stack trace:" << std::endl;
        for (int i = 0; i < frames; ++i) {
            std::cout << "  " << i << ": " << strs[i] << std::endl;
        }
        
        free(strs);
        
        // Check for additional errors
        while ((errorCode = glGetError()) != GL_NO_ERROR) {
            switch (errorCode) {
                case GL_INVALID_ENUM:
                    error = "GL_INVALID_ENUM";
                    break;
                case GL_INVALID_VALUE:
                    error = "GL_INVALID_VALUE";
                    break;
                case GL_INVALID_OPERATION:
                    error = "GL_INVALID_OPERATION";
                    break;
                case GL_STACK_OVERFLOW:
                    error = "GL_STACK_OVERFLOW";
                    break;
                case GL_STACK_UNDERFLOW:
                    error = "GL_STACK_UNDERFLOW";
                    break;
                case GL_OUT_OF_MEMORY:
                    error = "GL_OUT_OF_MEMORY";
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    error = "GL_INVALID_FRAMEBUFFER_OPERATION";
                    break;
                default:
                    error = "Unknown error";
                    break;
            }
            std::cout << "Additional OpenGL error: " << error << std::endl;
        }
    }
}

void glCheckError() {
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "GL_INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "GL_STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "GL_STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                error = "Unknown error";
                break;
        }

        std::cout << "OpenGL error: " << error << std::endl;
        
        // Get stack trace
        void* callstack[128];
        int frames = backtrace(callstack, 128);
        char** strs = backtrace_symbols(callstack, frames);
        
        std::cout << "Stack trace:" << std::endl;
        for (int i = 0; i < frames; ++i) {
            std::cout << "  " << i << ": " << strs[i] << std::endl;
        }
        
        free(strs);
        
        // Check for additional errors
        while ((errorCode = glGetError()) != GL_NO_ERROR) {
            switch (errorCode) {
                case GL_INVALID_ENUM:
                    error = "GL_INVALID_ENUM";
                    break;
                case GL_INVALID_VALUE:
                    error = "GL_INVALID_VALUE";
                    break;
                case GL_INVALID_OPERATION:
                    error = "GL_INVALID_OPERATION";
                    break;
                case GL_STACK_OVERFLOW:
                    error = "GL_STACK_OVERFLOW";
                    break;
                case GL_STACK_UNDERFLOW:
                    error = "GL_STACK_UNDERFLOW";
                    break;
                case GL_OUT_OF_MEMORY:
                    error = "GL_OUT_OF_MEMORY";
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    error = "GL_INVALID_FRAMEBUFFER_OPERATION";
                    break;
                default:
                    error = "Unknown error";
                    break;
            }
            std::cout << "Additional OpenGL error: " << error << std::endl;
        }
    }
}

// Function to check if a string contains a substring
int includes(const char *string, const char *substring) {
    return strstr(string, substring) != NULL;
}

// Function to convert a char to lowercase
void lower(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}