#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "Util.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

std::map<std::string, Shader>    ResourceManager::Shaders;
std::map<std::string, Texture1D> ResourceManager::Textures1D;
std::map<std::string, Texture2D> ResourceManager::Textures2D;
std::map<std::string, Texture3D> ResourceManager::Textures3D;

Shader ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, std::string name) {
    return LoadShader(vShaderFile, fShaderFile, nullptr, name);
}
Shader ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture1D ResourceManager::LoadTexture1D(const char *file, bool alpha, std::string name,
                                         GLint sWrap, GLint minFilter, GLint magFilter)
{
    Textures1D[name] = loadTexture1DFromFile(file, alpha, sWrap, minFilter, magFilter);
    return Textures1D[name];
}

Texture1D ResourceManager::GetTexture1D(std::string name)
{
    return Textures1D[name];
}

Texture2D ResourceManager::LoadTexture2D(const char *file,  std::string name, bool alpha,GLint sWrap,GLint tWrap,GLint minFilter,GLint magFilter)
{
    if(name.empty()) {
        name = file;
    }
    Textures2D[name] = loadTexture2DFromFile(file, alpha, sWrap, tWrap, minFilter, magFilter);
    return Textures2D[name];
}

Texture2D ResourceManager::GetTexture2D(std::string name)
{
    return Textures2D[name];
}

Texture3D ResourceManager::LoadTexture3D(const char *file, bool alpha, std::string name,
                                         GLint sWrap, GLint tWrap, GLint rWrap,
                                         GLint minFilter, GLint magFilter)
{
    Textures3D[name] = loadTexture3DFromFile(file, alpha, sWrap, tWrap, rWrap, minFilter, magFilter);
    return Textures3D[name];
}

Texture3D ResourceManager::GetTexture3D(std::string name)
{
    return Textures3D[name];
}
// Implementation of GetTexture2DByIndex
Texture2D ResourceManager::GetTexture2DByIndex(size_t index) {
    std::vector<Texture2D> textureList = ConvertMapToList(Textures2D);

    if (index >= textureList.size()) {
        throw std::out_of_range("Index out of range");
    }

    return textureList[index];
}
void ResourceManager::Clear()
{
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    for (auto iter : Textures1D)
        glDeleteTextures(1, &iter.second.ID);
    for (auto iter : Textures2D)
        glDeleteTextures(1, &iter.second.ID);
    for (auto iter : Textures3D)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile)
{
    std::string vertexCode, fragmentCode, geometryCode;
    try
    {
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        vertexShaderFile.close();
        fragmentShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        if (gShaderFile != nullptr)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception& e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    const char *gShaderCode = geometryCode.c_str();
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

Texture1D ResourceManager::loadTexture1DFromFile(const char *file, bool alpha,
                                                 GLint sWrap, GLint minFilter, GLint magFilter)
{
    Texture1D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    int width, nrChannels;
    unsigned char* data = stbi_load(file, &width, nullptr, &nrChannels, 0);
    texture.Wrap_S = sWrap;
    texture.Filter_Min = minFilter;
    texture.Filter_Max = magFilter;
    texture.Generate(width, data);
    stbi_image_free(data);
    return texture;
}

Texture2D ResourceManager::loadTexture2DFromFile(const char *file, bool alpha, GLint sWrap, GLint tWrap, GLint minFilter, GLint magFilter)
{
    Texture2D texture;

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << file << std::endl;
        return texture;
    }

    texture.Wrap_S = sWrap;
    texture.Wrap_T = tWrap;
    texture.Filter_Min = minFilter;
    texture.Filter_Max = magFilter;
    if (alpha || nrChannels > 3) {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    } else {
        texture.Internal_Format = GL_RGB;
        texture.Image_Format = GL_RGB;
    }
    texture.Generate(width, height, data);

    stbi_image_free(data);
    return texture;
}

Texture3D ResourceManager::loadTexture3DFromFile(const char *file, bool alpha,
                                                 GLint sWrap, GLint tWrap, GLint rWrap,
                                                 GLint minFilter, GLint magFilter)
{
    Texture3D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    int width, height, depth = 1, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    texture.Wrap_S = sWrap;
    texture.Wrap_T = tWrap;
    texture.Wrap_R = rWrap;
    texture.Filter_Min = minFilter;
    texture.Filter_Max = magFilter;
    texture.Generate(width, height, depth, data);
    stbi_image_free(data);
    return texture;
}