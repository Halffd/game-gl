#include "Shader.h"
#include "util/Util.h"
#include <iostream>

Shader &Shader::Use()
{
    glUseProgram(this->ID);
    return *this;
}

void Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
    unsigned int sVertex, sFragment, gShader;
    // vertex Shader
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX");
    // fragment Shader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT");
    // if geometry shader source code is given, also compile geometry shader
    if (geometrySource != nullptr)
    {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &geometrySource, NULL);
        glCompileShader(gShader);
        checkCompileErrors(gShader, "GEOMETRY");
    }
    // shader program
    this->ID = glCreateProgram();
    glAttachShader(this->ID, sVertex);
    glAttachShader(this->ID, sFragment);
    if (geometrySource != nullptr)
        glAttachShader(this->ID, gShader);
    glLinkProgram(this->ID);
    checkCompileErrors(this->ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (geometrySource != nullptr)
        glDeleteShader(gShader);
    
    std::cout << "Shader program ID: " << this->ID << std::endl;
}

void Shader::SetFloat(const char *name, float value, bool useShader)
{
    if (useShader)
        this->Use();
    GLint location = glGetUniformLocation(this->ID, name);
    if (location != -1) {
        glUniform1f(location, value);
    }
    glCheckError(__FILE__, __LINE__);
}
void Shader::SetInteger(const char *name, int value, bool useShader)
{
    if (useShader)
        this->Use ();
    GLint location = glGetUniformLocation(this->ID, name);
    if (location != -1) {
        glUniform1i(location, value);
    }
    glCheckError(__FILE__, __LINE__);
}
void Shader::SetVector2f(const char *name, float x, float y, bool useShader)
{
    if (useShader)
        this->Use();
    GLint location = glGetUniformLocation(this->ID, name);
    if (location != -1) {
        glUniform2f(location, x, y);
    }
    glCheckError(__FILE__, __LINE__);
}
void Shader::SetVector2f(const char *name, const glm::vec2 &value, bool useShader)
{
    if (useShader)
        this->Use();
    GLint location = glGetUniformLocation(this->ID, name);
    if (location != -1) {
        glUniform2f(location, value.x, value.y);
    }
    glCheckError(__FILE__, __LINE__);
}
void Shader::SetVector3f(const char *name, float x, float y, float z, bool useShader)
{
    if (useShader)
        this->Use();
    GLint location = glGetUniformLocation(this->ID, name);
    if (location != -1) {
        glUniform3f(location, x, y, z);
    }
    glCheckError(__FILE__, __LINE__);
}
void Shader::SetVector3f(const char *name, const glm::vec3 &value, bool useShader)
{
    if (useShader)
        this->Use();
    GLint location = glGetUniformLocation(this->ID, name);
    if (location != -1) {
        glUniform3f(location, value.x, value.y, value.z);
    }
    glCheckError(__FILE__, __LINE__);
}
void Shader::SetVector4f(const char *name, float x, float y, float z, float w, bool useShader)
{
    if (useShader)
        this->Use();
    GLint location = glGetUniformLocation(this->ID, name);
    if (location != -1) {
        glUniform4f(location, x, y, z, w);
    }
    glCheckError(__FILE__, __LINE__);
}
void Shader::SetVector4f(const char *name, const glm::vec4 &value, bool useShader)
{
    if (useShader)
        this->Use();
    GLint location = glGetUniformLocation(this->ID, name);
    if (location != -1) {
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }
    glCheckError(__FILE__, __LINE__);
}
void Shader::SetMatrix4(const char *name, const glm::mat4 &matrix, bool useShader)
{
    if (useShader)
        this->Use();
    GLint location = glGetUniformLocation(this->ID, name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, false, glm::value_ptr(matrix));
    }
    glCheckError(__FILE__, __LINE__);
}


void Shader::checkCompileErrors(unsigned int object, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
            throw std::runtime_error("Shader compilation failed: " + type);
        }
        else
        {
            std::cout << "Shader compilation successful: " << type << std::endl;
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
            throw std::runtime_error("Shader linking failed");
        }
        else
        {
            std::cout << "Shader linking successful" << std::endl;
        }
    }
}