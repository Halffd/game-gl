#include <glad/glad.h>
#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <string>

#include "Util.hpp"

void glCheckError(const char *file, int line)
{
    GLenum errorCode = glGetError();
    while (errorCode != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
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
        errorCode = glGetError();
    }
}
void glCheckError()
{
    GLenum errorCode = glGetError();

    while (errorCode != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
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
        errorCode = glGetError();
    }
}
namespace util {
    std::string to_string(const glm::vec2& vec) {
        std::ostringstream oss;
        oss << "vec2(" << vec.x << ", " << vec.y << ")";
        return oss.str();
    }

    std::string to_string(const glm::vec3& vec) {
        std::ostringstream oss;
        oss << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return oss.str();
    }

    std::string to_string(const glm::vec4& vec) {
        std::ostringstream oss;
        oss << "vec4(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
        return oss.str();
    }

    std::string to_string(const glm::quat& quat) {
        std::ostringstream oss;
        oss << "quat(" << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z << ")";
        return oss.str();
    }

    std::string to_string(const glm::mat2& mat) {
        std::ostringstream oss;
        oss << "mat2(\n[";
        for (int i = 0; i < 2; ++i) {
            oss << "[";
            for (int j = 0; j < 2; ++j) {
                oss << mat[i][j];
                if (j < 1) oss << ", ";
            }
            oss << "]";
            if (i < 1) oss << ", \n";
        }
        oss << "]\n)";
        return oss.str();
    }

    std::string to_string(const glm::mat3& mat) {
        std::ostringstream oss;
        oss << "mat3(\n[";
        for (int i = 0; i < 3; ++i) {
            oss << "[";
            for (int j = 0; j < 3; ++j) {
                oss << mat[i][j];
                if (j < 2) oss << ", ";
            }
            oss << "]";
            if (i < 2) oss << ", \n";
        }
        oss << "]\n)";
        return oss.str();
    }

    std::string to_string(const glm::mat4& mat) {
        std::ostringstream oss;
        oss << "mat4(\n[";
        for (int i = 0; i < 4; ++i) {
            oss << "[";
            for (int j = 0; j < 4; ++j) {
                oss << mat[i][j];
                if (j < 3) oss << ", ";
            }
            oss << "]";
            if (i < 3) oss << ", \n";
        }
        oss << "]\n)";
        return oss.str();
    }
}
// Function to check if a string contains a substring
int includes(const char *string, const char *substring) {
    return strstr(string, substring) != NULL;
}
