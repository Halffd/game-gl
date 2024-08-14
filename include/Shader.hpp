#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include "Util.hpp"

#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>

class Shader
{
public:
    unsigned int ID;

    Shader() : ID(0) {}

    Shader(const char* vertexPath, const char* fragmentPath) {
        loadShader(vertexPath, fragmentPath);
    }

    ~Shader() {
        return;
        if (ID != 0) {
            glDeleteProgram(ID);
            glCheckError(__FILE__, __LINE__);
            ID = 0;
        }
    }

    // Copy constructor
    Shader(const Shader& other) {
        if (other.ID != 0) {
            ID = other.ID;
        } else {
            ID = 0;
        }
    }


    // Copy assignment operator
    Shader& operator=(const Shader& other) {
        if (this == &other) {
            return *this;
        }

        if (ID != 0) {
            glDeleteProgram(ID);
            glCheckError(__FILE__, __LINE__);
        }

        if (other.ID != 0) {
            ID = other.ID;
        } else {
            ID = 0;
        }

        return *this;
    }

    // Move constructor
    Shader(Shader&& other) noexcept : ID(other.ID) {
        other.ID = 0;
    }

    // Move assignment operator
    Shader& operator=(Shader&& other) noexcept {
        if (this != &other) {
            if (ID != 0) {
                glDeleteProgram(ID);
                glCheckError(__FILE__, __LINE__);
            }

            ID = other.ID;
            other.ID = 0;
        }
        return *this;
    }

    // Equality operator
    bool operator==(const Shader& other) const {
        return ID == other.ID;
    }

    // Inequality operator
    bool operator!=(const Shader& other) const {
        return !(*this == other);
    }

    // Less-than operator (for sorting purposes)
    bool operator<(const Shader& other) const {
        return ID < other.ID;
    }

    void use() const {
        glUseProgram(ID);
        glCheckError(__FILE__, __LINE__);
    }

    auto get(const std::string &name) const {
        return glGetUniformLocation(ID, name.c_str());
    }

    void setBool(const std::string &name, bool value) const {
        glUniform1i(getUniformLocation(name), (int)value);
        glCheckError(__FILE__, __LINE__);
    }

    void setInt(const std::string &name, int value) const {
        glUniform1i(getUniformLocation(name), value);
        glCheckError(__FILE__, __LINE__);
    }

    void setFloat(const std::string &name, float value) const {
        glUniform1f(getUniformLocation(name), value);
        glCheckError(__FILE__, __LINE__);
    }

    void setFloat(const std::string& name, const std::vector<float>& values) const {
        glUniform1fv(getUniformLocation(name), static_cast<GLsizei>(values.size()), values.data());
        glCheckError(__FILE__, __LINE__);
    }

    void setVec2(const std::string &name, const glm::vec2 &value) const {
        glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
        glCheckError(__FILE__, __LINE__);
    }

    void setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
        glCheckError(__FILE__, __LINE__);
    }

    void setVec3(const std::string& name, float x, float y, float z) const {
        setVec3(name, glm::vec3(x, y, z));
    }

    void setVec4(const std::string &name, const glm::vec4 &value) const {
        glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
        glCheckError(__FILE__, __LINE__);
    }

    void setVec4(const std::string& name, float x, float y, float z, float w) const {
        setVec4(name, glm::vec4(x, y, z, w));
    }

    void setMat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
        glCheckError(__FILE__, __LINE__);
    }

    void setMat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
        glCheckError(__FILE__, __LINE__);
    }

    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
        glCheckError(__FILE__, __LINE__);
    }

private:
    std::string vertexCode;
    std::string fragmentCode;
    std::shared_ptr<unsigned int> shaderProgram;

    mutable std::unordered_map<std::string, GLint> uniformLocationCache;

    void loadShader(const char* vertexPath, const char* fragmentPath) {
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            std::filesystem::path currentDir = std::filesystem::current_path();
            if (currentDir == "/") {
                currentDir = "";
            }

            std::string vertexFilePath = (currentDir / vertexPath).generic_string();
            std::string fragmentFilePath = (currentDir / fragmentPath).generic_string();
            std::stringstream vShaderStream, fShaderStream;

            #if DEBUG == 1
            std::cout << vertexFilePath << " / " << fragmentFilePath << " / " << currentDir << "\n";
            #endif

            vShaderFile.open(vertexFilePath);
            fShaderFile.open(fragmentFilePath);
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
         } catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        ID = createShaderProgram(vShaderCode, fShaderCode);
    }

    unsigned int createShaderProgram(const char* vertexCode, const char* fragmentCode) const {
        unsigned int vertex, fragment;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        unsigned int program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        checkCompileErrors(program, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        return program;
    }

    void checkCompileErrors(unsigned int shader, std::string type) const {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    GLint getUniformLocation(const std::string &name) const {
        if (uniformLocationCache.find(name) != uniformLocationCache.end()) {
            return uniformLocationCache.at(name);
        }

        GLint location = glGetUniformLocation(ID, name.c_str());
        uniformLocationCache[name] = location;
        return location;
    }
};

#endif
