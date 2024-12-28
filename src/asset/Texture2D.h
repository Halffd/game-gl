#ifndef TEXTURE2D_HPP
#define TEXTURE2D_HPP

#include <glad/glad.h>

class Texture2D
{
public:
    unsigned int ID;
    unsigned int Width, Height;
    int status;
    GLenum Internal_Format;
    GLenum Image_Format;
    GLenum Wrap_S;
    GLenum Wrap_T;
    GLenum Filter_Min;
    GLenum Filter_Max;

    Texture2D();
    void Generate(unsigned int width, unsigned int height, unsigned char* data);
    void Bind() const;
};

#endif