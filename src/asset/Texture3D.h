#ifndef TEXTURE3D_HPP
#define TEXTURE3D_HPP

#include <glad/glad.h>

class Texture3D
{
public:
    unsigned int ID;
    unsigned int Width, Height, Depth;
    GLenum Internal_Format;
    GLenum Image_Format;
    GLenum Wrap_S;
    GLenum Wrap_T;
    GLenum Wrap_R;
    GLenum Filter_Min;
    GLenum Filter_Max;

    Texture3D();
    void Generate(unsigned int width, unsigned int height, unsigned int depth, unsigned char* data);
    void Bind() const;
};

#endif