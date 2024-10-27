#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glad/glad.h>
#include "Util.hpp"

class Texture
{
public:
    unsigned height;
    unsigned width;
    Texture() : ID(0), height(0), width(0), exists(false), activeTextureUnit(GL_TEXTURE0) {}
    Texture(const char *filePath,
              GLenum textureTarget = GL_TEXTURE_2D,
              GLint sWrap = GL_REPEAT,
              GLint tWrap = GL_REPEAT,
              GLint rWrap = GL_REPEAT,
              GLint minFilter = GL_LINEAR,
              GLint magFilter = GL_LINEAR)
    {
        Load(filePath, textureTarget, sWrap, tWrap, rWrap, minFilter, magFilter);
    }
    bool Load(const char *filePath,
              GLenum textureTarget = GL_TEXTURE_2D,
              GLint sWrap = GL_REPEAT,
              GLint tWrap = GL_REPEAT,
              GLint rWrap = GL_REPEAT,
              GLint minFilter = GL_LINEAR,
              GLint magFilter = GL_LINEAR)
    {
        Free();
        int width, height, depth, nrChannels;
        stbi_set_flip_vertically_on_load(true);

        unsigned char *data = nullptr;
        if (textureTarget == GL_TEXTURE_1D)
        {
            data = stbi_load(filePath, &width, &height, &nrChannels, 0);
            height = 1;
            depth = 1;
        }
        else if (textureTarget == GL_TEXTURE_2D)
        {
            data = stbi_load(filePath, &width, &height, &nrChannels, 0);
            depth = 1;
        }
        else if (textureTarget == GL_TEXTURE_3D)
        {
            int dataSize;
            data = stbi_load_from_memory((unsigned char *)filePath, strlen(filePath), &width, &height, &depth, 4);
        }
        else
        {
            lo << "Unsupported texture target: " << textureTarget << std::endl;
            return false;
        }

        if (data)
        {
            glGenTextures(1, &ID);
            glCheckError(__FILE__, __LINE__);

            glBindTexture(textureTarget, ID);
            glCheckError(__FILE__, __LINE__);

            // Set the texture wrapping parameters
            switch (textureTarget)
            {
            case GL_TEXTURE_1D:
                glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, sWrap);
                glCheckError(__FILE__, __LINE__);
                break;
            case GL_TEXTURE_2D:
                glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, sWrap);
                glCheckError(__FILE__, __LINE__);
                glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, tWrap);
                glCheckError(__FILE__, __LINE__);
                break;
            case GL_TEXTURE_3D:
                glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, sWrap);
                glCheckError(__FILE__, __LINE__);
                glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, tWrap);
                glCheckError(__FILE__, __LINE__);
                glTexParameteri(textureTarget, GL_TEXTURE_WRAP_R, rWrap);
                glCheckError(__FILE__, __LINE__);
                break;
            default:
                break;
            }

            // Set the texture filtering parameters
            switch (minFilter)
            {
            case GL_NEAREST:
            case GL_LINEAR:
            case GL_NEAREST_MIPMAP_NEAREST:
            case GL_LINEAR_MIPMAP_NEAREST:
            case GL_NEAREST_MIPMAP_LINEAR:
            case GL_LINEAR_MIPMAP_LINEAR:
                glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, minFilter);
                glCheckError(__FILE__, __LINE__);
                break;
            default:
                lo << "Unsupported min filter: " << minFilter << std::endl;
                break;
            }

            switch (magFilter)
            {
            case GL_NEAREST:
            case GL_LINEAR:
                glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, magFilter);
                glCheckError(__FILE__, __LINE__);
                break;
            default:
                lo << "Unsupported mag filter: " << magFilter << std::endl;
                break;
            }

            // Load the texture data
            GLenum format = GL_RGBA;
            switch (nrChannels)
            {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                lo << "Unsupported number of channels: " << nrChannels << std::endl;
                stbi_image_free(data);
                return false;
            }
            lo << width << "x"<< height << "x" << depth << "  " << nrChannels << ' ' << format << std::endl;
            if (textureTarget == GL_TEXTURE_1D)
            {
                glTexImage1D(textureTarget, 0, format, width, 0, format, GL_UNSIGNED_BYTE, data);
                glCheckError(__FILE__, __LINE__);
            }
            else if (textureTarget == GL_TEXTURE_2D)
            {
                glTexImage2D(textureTarget, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glCheckError(__FILE__, __LINE__);
            }
            else if (textureTarget == GL_TEXTURE_3D)
            {
                glTexImage3D(textureTarget, 0, format, width, height, depth, 0, format, GL_UNSIGNED_BYTE, data);
                glCheckError(__FILE__, __LINE__);
            }
            else
            {
                lo << "Unsupported texture target: " << textureTarget << std::endl;
                stbi_image_free(data);
                return false;
            }

            glGenerateMipmap(textureTarget);
            glCheckError(__FILE__, __LINE__);
            stbi_image_free(data);

            this->width = width;
            this->height = height;
            this->depth = depth;
            this->nrChannels = nrChannels;
            this->exists = true;
            return true;
        }
        else
        {
            lo << "Failed to load texture: " << filePath << std::endl;
            return false;
        }
    }
    void Activate(GLenum textureUnit)
    {
        if (!exists)
            return;
        activeTextureUnit = textureUnit;
        glActiveTexture(activeTextureUnit);
        glCheckError(__FILE__, __LINE__);
        glBindTexture(GL_TEXTURE_2D, ID);
        glCheckError(__FILE__, __LINE__);
    }
    void Free() {
        if (exists && ID != 0) {
            //glDeleteTextures(1, &ID);
            //glCheckError(__FILE__, __LINE__);
        }
        exists = false;
        ID = 0;
    }

    ~Texture() {
        Free();
    }

private:
    unsigned ID;
    unsigned depth;
    unsigned nrChannels;
    bool exists;
    GLenum activeTextureUnit;
};
void unbindTextures() {
    int maxTextureUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    for (int i = 0; i < maxTextureUnits; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glCheckError(__FILE__, __LINE__);
        glBindTexture(GL_TEXTURE_2D, 0);
        glCheckError(__FILE__, __LINE__);
    }
}
#endif