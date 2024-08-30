#ifndef util
#define util

// Include any other necessary headers for your project

// Include the necessary OpenGL headers

#include <string>
#include <glad/glad.h>     // for OpenGL 2.1
#include <map>
#include <vector>
// #include <GL/glext.h>  // for OpenGL extensions (if needed)

void glCheckError();
void glCheckError(const char* file, int line);

// Function to convert a map to a vector
template <typename Key, typename Value>
std::vector<Value> ConvertMapToList(const std::map<Key, Value>& inputMap) {
    std::vector<Value> valueList;
    for (const auto& pair : inputMap) {
        valueList.push_back(pair.second);
    }
    return valueList;
}

#endif  // util