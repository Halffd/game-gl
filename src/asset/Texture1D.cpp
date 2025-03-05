#include "Texture1D.h"

Texture1D::Texture1D()
    : Width(0), Internal_Format(GL_RGB), Image_Format(GL_RGB),
      Wrap_S(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR) {
    glGenTextures(1, &ID);
}

void Texture1D::Generate(unsigned int width, unsigned char* data) {
    Width = width;
    glBindTexture(GL_TEXTURE_1D, ID);
    glTexImage1D(GL_TEXTURE_1D, 0, Internal_Format, width, 0, Image_Format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, Wrap_S);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, Filter_Min);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, Filter_Max);
    glBindTexture(GL_TEXTURE_1D, 0);
}

void Texture1D::Bind() const {
    glBindTexture(GL_TEXTURE_1D, ID);
}