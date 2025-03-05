#include "Texture3D.h"

Texture3D::Texture3D()
    : Width(0), Height(0), Depth(0), Internal_Format(GL_RGB), Image_Format(GL_RGB),
      Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Wrap_R(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR) {
    glGenTextures(1, &ID);
}

void Texture3D::Generate(unsigned int width, unsigned int height, unsigned int depth, unsigned char* data) {
    Width = width;
    Height = height;
    Depth = depth;
    glBindTexture(GL_TEXTURE_3D, ID);
    glTexImage3D(GL_TEXTURE_3D, 0, Internal_Format, width, height, depth, 0, Image_Format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, Wrap_S);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, Wrap_T);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, Wrap_R);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, Filter_Min);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, Filter_Max);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture3D::Bind() const {
    glBindTexture(GL_TEXTURE_3D, ID);
}