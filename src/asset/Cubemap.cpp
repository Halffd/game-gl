#include "Cubemap.h"
#include <iostream>
#include <stdexcept>
#include <stb_image.h>
#include "stb/stb_image_resize2.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

Cubemap::Cubemap() 
    : ID(0),
      Internal_Format(GL_SRGB8),
      Image_Format(GL_RGB),
      Wrap_S(GL_CLAMP_TO_EDGE),
      Wrap_T(GL_CLAMP_TO_EDGE),
      Wrap_R(GL_CLAMP_TO_EDGE),
      Filter_Min(GL_LINEAR_MIPMAP_LINEAR),
      Filter_Max(GL_LINEAR) {
    glGenTextures(1, &ID);
}

// Helper function to load an image with error checking
static unsigned char* LoadImage(const std::string& path, int* width, int* height, int* channels, int desired_channels = 0) {
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(path.c_str(), width, height, channels, desired_channels);
    if (!data) {
        throw std::runtime_error("Failed to load image: " + path + " - " + stbi_failure_reason());
    }
    return data;
}

bool Cubemap::Load(const std::vector<std::string>& faces) {
    if (faces.size() != 6) {
        throw std::runtime_error("Cubemap requires exactly 6 faces, got " + std::to_string(faces.size()));
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    try {
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(false);

        // Load each face
        for (unsigned int i = 0; i < 6; i++) {
            unsigned char* data = LoadImage(faces[i], &width, &height, &nrChannels, 0);
            
            // Determine format
            GLenum format = GL_RGB;
            if (nrChannels == 1) format = GL_RED;
            else if (nrChannels == 3) format = GL_RGB;
            else if (nrChannels == 4) format = GL_RGBA;

            // Store format from first face
            if (i == 0) {
                Image_Format = format;
                Internal_Format = (format == GL_RGBA) ? GL_SRGB8_ALPHA8 : GL_SRGB8;
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, Internal_Format, 
                        width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }

        // Generate mipmaps and set texture parameters
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, Filter_Min);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, Filter_Max);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, Wrap_S);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, Wrap_T);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, Wrap_R);

        // Anisotropic filtering if supported
#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY
        float maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
        if (maxAniso > 0.0f) {
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
        }
#endif

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Cubemap loading error: " << e.what() << std::endl;
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return false;
    }
}

bool Cubemap::LoadFromSingleImage(const std::string& path, Layout layout) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    
    try {
        // Load the source image
        unsigned char* srcData = LoadImage(path, &width, &height, &channels, 0);
        
        // Determine format
        GLenum format = GL_RGB;
        if (channels == 1) format = GL_RED;
        else if (channels == 3) format = GL_RGB;
        else if (channels == 4) format = GL_RGBA;
        
        Image_Format = format;
        Internal_Format = (format == GL_RGBA) ? GL_SRGB8_ALPHA8 : GL_SRGB8;
        
        // Calculate face size based on layout
        int faceWidth, faceHeight, faceChannels = channels;
        
        switch (layout) {
            case Layout::HORIZONTAL_CROSS: // 3x2 grid
            case Layout::VERTICAL_CROSS:
                faceWidth = width / 3;
                faceHeight = height / 2;
                break;
            case Layout::HORIZONTAL_STRIP: // 6x1 strip
                faceWidth = width / 6;
                faceHeight = height;
                break;
            default:
                throw std::runtime_error("Unsupported cubemap layout");
        }
        
        // Allocate memory for each face
        size_t faceSize = faceWidth * faceHeight * channels;
        std::vector<unsigned char> faceData(faceSize);
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
        
        // Process each face
        for (int i = 0; i < 6; i++) {
            int srcX = 0, srcY = 0;
            
            // Calculate source position based on layout
            switch (layout) {
                case Layout::HORIZONTAL_CROSS: // 3x2 grid
                    // +X (right), -X (left), +Y (top), -Y (bottom), +Z (front), -Z (back)
                    switch (i) {
                        case 0: srcX = 2 * faceWidth; srcY = 1 * faceHeight; break; // +X (right)
                        case 1: srcX = 0 * faceWidth; srcY = 1 * faceHeight; break; // -X (left)
                        case 2: srcX = 1 * faceWidth; srcY = 0 * faceHeight; break; // +Y (top)
                        case 3: srcX = 1 * faceWidth; srcY = 2 * faceHeight; break; // -Y (bottom)
                        case 4: srcX = 1 * faceWidth; srcY = 1 * faceHeight; break; // +Z (front)
                        case 5: srcX = 3 * faceWidth; srcY = 1 * faceHeight; break; // -Z (back)
                    }
                    break;
                    
                case Layout::VERTICAL_CROSS: // 3x2 grid (vertical cross)
                    switch (i) {
                        case 0: srcX = 2 * faceWidth; srcY = 1 * faceHeight; break; // +X (right)
                        case 1: srcX = 0 * faceWidth; srcY = 1 * faceHeight; break; // -X (left)
                        case 2: srcX = 1 * faceWidth; srcY = 0 * faceHeight; break; // +Y (top)
                        case 3: srcX = 1 * faceWidth; srcY = 2 * faceHeight; break; // -Y (bottom)
                        case 4: srcX = 1 * faceWidth; srcY = 1 * faceHeight; break; // +Z (front)
                        case 5: srcX = 3 * faceWidth; srcY = 1 * faceHeight; break; // -Z (back)
                    }
                    break;
                    
                case Layout::HORIZONTAL_STRIP: // 6x1 strip
                    srcX = i * faceWidth;
                    srcY = 0;
                    break;
                    
                default:
                    throw std::runtime_error("Unsupported cubemap layout");
            }
            
            // Extract face data
            for (int y = 0; y < faceHeight; y++) {
                size_t srcOffset = ((srcY + y) * width + srcX) * channels;
                size_t dstOffset = y * faceWidth * channels;
                std::memcpy(&faceData[dstOffset], &srcData[srcOffset], faceWidth * channels);
            }
            
            // Upload face to OpenGL
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, Internal_Format,
                        faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faceData.data());
        }
        
        // Generate mipmaps and set texture parameters
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, Filter_Min);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, Filter_Max);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, Wrap_S);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, Wrap_T);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, Wrap_R);

        // Anisotropic filtering if supported
#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY
        float maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
        if (maxAniso > 0.0f) {
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
        }
#endif

        stbi_image_free(srcData);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to load cubemap from single image: " << e.what() << std::endl;
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return false;
    }
}

void Cubemap::Bind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}
