#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Cubemap {
public:
    // Holds the ID of the texture object
    unsigned int ID;
    
    // Texture configuration
    GLenum Internal_Format;
    GLenum Image_Format;
    GLenum Wrap_S;
    GLenum Wrap_T;
    GLenum Wrap_R;
    GLenum Filter_Min;
    GLenum Filter_Max;

    // Constructor (sets default texture modes)
    Cubemap();
    
    // Supported cubemap layouts
    enum class Layout {
        INDIVIDUAL_FACES,  // 6 separate image files
        HORIZONTAL_CROSS,  // 3x2 grid layout (horizontal cross)
        VERTICAL_CROSS,    // 3x2 grid layout (vertical cross)
        EQUIRECTANGULAR,   // Equirectangular projection
        HORIZONTAL_STRIP   // 6x1 horizontal strip
    };

    // Loads cubemap from 6 individual texture faces
    // Order: +X (right), -X (left), +Y (top), -Y (bottom), +Z (front), -Z (back)
    bool Load(const std::vector<std::string>& faces);
    
    // Loads cubemap from a single image with specified layout
    bool LoadFromSingleImage(const std::string& path, Layout layout);
    
    // Binds the cubemap texture as the current active GL_TEXTURE_CUBE_MAP texture
    void Bind() const;
};

#endif // CUBEMAP_H
