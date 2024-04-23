#ifndef util
#define util

// Include any other necessary headers for your project

// Include the necessary OpenGL headers
#include <glad/glad.h>     // for OpenGL 2.1
// #include <GL/glext.h>  // for OpenGL extensions (if needed)

void glCheckError();
void glCheckError(const char* file, int line);
#endif  // vertex