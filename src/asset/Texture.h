#pragma once

#include <glad/glad.h>
#include <string>
#include <iostream>

// TextureSimple class (formerly Texture2D)
class TextureSimple {
public:
    // Texture ID
    unsigned int ID;
    // Texture dimensions
    unsigned int Width, Height;
    // Texture format
    unsigned int Internal_Format;
    unsigned int Image_Format;
    // Texture configuration
    unsigned int Wrap_S;
    unsigned int Wrap_T;
    unsigned int Filter_Min;
    unsigned int Filter_Max;
    // Status (1 = success, -1 = error)
    int status;
    
    // Constructor
    TextureSimple() 
        : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), 
          Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR), status(0) {
        glGenTextures(1, &this->ID);
    }
    
    // Generate texture from image data
    void Generate(unsigned int width, unsigned int height, unsigned char* data) {
        this->Width = width;
        this->Height = height;
        
        // Create texture
        glBindTexture(GL_TEXTURE_2D, this->ID);
        glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
        
        // Set texture wrap and filter modes
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
        
        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    // Bind texture as current active GL_TEXTURE_2D texture object
    void Bind() const {
        glBindTexture(GL_TEXTURE_2D, this->ID);
    }
}; 