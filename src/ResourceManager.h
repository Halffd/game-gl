#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>
#include <vector>

#include <glad/glad.h>

#include "Texture1D.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "Shader.h"

class ResourceManager
{
public:
    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture1D> Textures1D;
    static std::map<std::string, Texture2D> Textures2D;
    static std::map<std::string, Texture3D> Textures3D;

    static Shader    LoadShader(const char *vShaderFile, const char *fShaderFile, std::string name);
    static Shader    LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name);
    static Shader    GetShader(std::string name);

    static Texture1D LoadTexture1D(const char *file, bool alpha, std::string name,
                                   GLint sWrap = GL_REPEAT, GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR);
    static Texture1D GetTexture1D(std::string name);

    static Texture2D LoadTexture2D(const char *file,
                std::string name = "",
                bool alpha = false,
              GLint sWrap = GL_REPEAT,
              GLint tWrap = GL_REPEAT,
              GLint minFilter = GL_LINEAR,
              GLint magFilter = GL_LINEAR);
    static Texture2D GetTexture2D(std::string name);

    static Texture3D LoadTexture3D(const char *file, bool alpha, std::string name,
                                   GLint sWrap = GL_REPEAT, GLint tWrap = GL_REPEAT, GLint rWrap = GL_REPEAT,
                                   GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR);
    static Texture3D GetTexture3D(std::string name);
    static Texture2D GetTexture2DByIndex(size_t index);

    static void      Clear();

private:
    ResourceManager() { }

    static Shader    loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile = nullptr);
    static Texture1D loadTexture1DFromFile(const char *file, bool alpha,
                                           GLint sWrap, GLint minFilter, GLint magFilter);
    static Texture2D loadTexture2DFromFile(const char *file, bool alpha = false,
              GLint sWrap = GL_REPEAT,
              GLint tWrap = GL_REPEAT,
              GLint minFilter = GL_LINEAR,
              GLint magFilter = GL_LINEAR);
    static Texture3D loadTexture3DFromFile(const char *file, bool alpha,
                                           GLint sWrap, GLint tWrap, GLint rWrap,
                                           GLint minFilter, GLint magFilter);
};

#endif