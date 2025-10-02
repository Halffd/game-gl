#include "Util.h"
#include <glad/glad.h>
#include <cstring> // For strstr
#include <execinfo.h> // For backtrace
#include <cstdlib> // For free

// Enhanced error checking with stack trace and context
void glCheckError(const char *file, int line) {
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        std::string error;
        const char* errorStr = "";
        
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "GL_INVALID_ENUM";
                errorStr = "An unacceptable value is specified for an enumerated argument.";
                break;
            case GL_INVALID_VALUE:
                error = "GL_INVALID_VALUE";
                errorStr = "A numeric argument is out of range.";
                break;
            case GL_INVALID_OPERATION:
                error = "GL_INVALID_OPERATION";
                errorStr = "The specified operation is not allowed in the current state.";
                break;
            case GL_STACK_OVERFLOW:
                error = "GL_STACK_OVERFLOW";
                errorStr = "This command would cause a stack overflow.";
                break;
            case GL_STACK_UNDERFLOW:
                error = "GL_STACK_UNDERFLOW";
                errorStr = "This command would cause a stack underflow.";
                break;
            case GL_OUT_OF_MEMORY:
                error = "GL_OUT_OF_MEMORY";
                errorStr = "There is not enough memory left to execute the command.";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "GL_INVALID_FRAMEBUFFER_OPERATION";
                errorStr = "The framebuffer object is not complete.";
                break;
            default:
                error = "Unknown error";
                errorStr = "No description available.";
                break;
        }

        std::cerr << "\n=== OpenGL Error ===" << std::endl;
        std::cerr << "Error: " << error << std::endl;
        std::cerr << "Description: " << errorStr << std::endl;
        std::cerr << "File: " << file << " (line " << line << ")" << std::endl;
        
        // Get OpenGL context info
        const GLubyte* vendor = glGetString(GL_VENDOR);
        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* version = glGetString(GL_VERSION);
        const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
        
        std::cerr << "\n=== OpenGL Context ===" << std::endl;
        std::cerr << "Vendor: " << vendor << std::endl;
        std::cerr << "Renderer: " << renderer << std::endl;
        std::cerr << "Version: " << version << std::endl;
        std::cerr << "GLSL Version: " << glslVersion << std::endl;
        
        // Get stack trace
        void* callstack[128];
        int frames = backtrace(callstack, 128);
        char** strs = backtrace_symbols(callstack, frames);
        
        std::cerr << "\n=== Stack Trace ===" << std::endl;
        for (int i = 1; i < frames && i < 10; ++i) {
            std::cerr << "  #" << i-1 << " " << strs[i] << std::endl;
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