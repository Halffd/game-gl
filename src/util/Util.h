#ifndef util_h
#define util_h

// Include any other necessary headers for your project

// Include the necessary OpenGL headers

#include <string>
#include <glad/glad.h>     // for OpenGL 2.1
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
#include "Log.h"
// #include <GL/glext.h>  // for OpenGL extensions (if needed)


void glCheckError();

void glCheckError(const char* file, int line);

// Function to check if a string contains a substring
int includes(const char *string, const char *substring);
// Function to convert a map to a vector

template <typename Key, typename Value>
std::vector<Value> ConvertMapToList(const std::map<Key, Value>& inputMap) {
    std::vector<Value> valueList;
    for (const auto& pair : inputMap) {
        valueList.push_back(pair.second);
    }
    return valueList;
}
// Function to convert a char to lowercase
void lower(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}

#endif  // util_h