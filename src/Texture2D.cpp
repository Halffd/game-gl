#include "Texture2D.h"
#include "Util.hpp"

Texture2D::Texture2D()
    : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB),
      Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR) {
    glGenTextures(1, &ID);
    glCheckError(__FILE__, __LINE__);
}

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data) {
    Width = width;
    Height = height;
    Bind();

    glTexImage2D(GL_TEXTURE_2D, 0, Internal_Format, width, height, 0, Image_Format, GL_UNSIGNED_BYTE, data);
    glCheckError(__FILE__, __LINE__);
    glGenerateMipmap(GL_TEXTURE_2D);
    glCheckError(__FILE__, __LINE__);
}

void Texture2D::Bind() const {
    glBindTexture(GL_TEXTURE_2D, ID);
    glCheckError(__FILE__, __LINE__);
}