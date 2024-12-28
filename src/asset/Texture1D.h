#ifndef TEXTURE1D_HPP
#define TEXTURE1D_HPP

#include <glad/glad.h>

class Texture1D
{
public:
    unsigned int ID;
    unsigned int Width;
    GLenum Internal_Format;
    GLenum Image_Format;
    GLenum Wrap_S;
    GLenum Filter_Min;
    GLenum Filter_Max;

    Texture1D();
    void Generate(unsigned int width, unsigned char* data);
    void Bind() const;
};

#endif