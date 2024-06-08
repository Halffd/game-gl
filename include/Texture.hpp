#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glad/glad.h>

class Texture {
public:
    Texture() : ID(0), height(0), width(0), exists(false), activeTextureUnit(GL_TEXTURE0) {}

    bool Load(const char* filePath) {
        Free();
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
        if (data) {
            glGenTextures(1, &ID);
            glBindTexture(GL_TEXTURE_2D, ID);
            //Activate(GL_TEXTURE0);

            // Set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // Set the texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            std::cout << "Channels: " << nrChannels  << "\n" << width << "x" << height << "\n";
            if (nrChannels == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            } else if (nrChannels == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            } else {
                std::cout << "Unsupported number of channels: " << nrChannels << std::endl;
                stbi_image_free(data);
                return false;
            }

            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);

            this->width = width;
            this->height = height;
            this->exists = true;
            return true;
        } else {
            std::cout << "Failed to load texture: " << filePath << std::endl;
            return false;
        }
    }

    void Activate(GLenum textureUnit) {
        if (!exists) return;
        activeTextureUnit = textureUnit;
        glActiveTexture(activeTextureUnit);
        glBindTexture(GL_TEXTURE_2D, ID);
    }
    void Free() {
        if (exists)
            glDeleteTextures(1, &ID);
        exists = false;
    }
    ~Texture(){
        Free();
    }

private:
    unsigned ID;
    unsigned height;
    unsigned width;
    bool exists;
    GLenum activeTextureUnit;
};
#endif