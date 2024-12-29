#ifndef util_h
#define util_h
#include <string>
#include <glad/glad.h>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
#include <memory>
#include "asset/Texture2D.h"
#include "Log.h"

void glCheckError();

void glCheckError(const char* file, int line);

// Function to check if a string contains a substring
int includes(const char *string, const char *substring);
// Function to convert a map to a vector

// Template function
template <typename Key, typename Value>
std::vector<Value> ConvertMapToList(const std::map<Key, Value>& inputMap) {
    std::vector<Value> valueList;
    valueList.reserve(inputMap.size()); // Reserve space for efficiency

    for (const auto& pair : inputMap) {
        if constexpr (std::is_same_v<Value, std::shared_ptr<Texture2D>>) {
            if (pair.second) {
                valueList.push_back(pair.second); // Dereference and copy the Texture2D
            }
        } else {
            valueList.push_back(pair.second);
        }
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