#ifndef util
#define util

// Include any other necessary headers for your project

// Include the necessary OpenGL headers
#include <glad/glad.h>     // for OpenGL 2.1
// #include <GL/glext.h>  // for OpenGL extensions (if needed)
#define TEXTURE_DIR "textures"
#define SHADER_DIR "shaders"


#define APPEND_DIR(dir, filename) (std::string(dir) + "/" + std::string(filename)).c_str()

#define SHADER(filename) APPEND_DIR(SHADER_DIR, filename)
#define TEXTURE(filename) APPEND_DIR(TEXTURE_DIR, filename)

void glCheckError();
void glCheckError(const char* file, int line);
#endif  // vertex