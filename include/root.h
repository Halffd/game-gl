#ifndef ROOT_H
#define ROOT_H

#include <string>

extern std::string root; // Declare the root variable as extern
#define TEXTURE_DIR "textures"
#define SHADER_DIR "shaders"

#define APPEND_DIR(dir, filename) (std::string(root) + "/" + std::string(dir) + "/" + std::string(filename)).c_str()

#define SHADER(filename) APPEND_DIR(SHADER_DIR, filename)
#define TEXTURE(filename) APPEND_DIR(TEXTURE_DIR, filename)

#endif // ROOT_H
