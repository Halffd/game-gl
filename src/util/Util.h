#ifndef util_h
#define util_h

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <memory>
#include "asset/Texture2D.h"

// Function prototypes
void glCheckError();
void glCheckError(const char* file, int line);

// Function to check if a string contains a substring
int includes(const char *string, const char *substring);

// Function to convert a map to a vector
template <typename Key, typename Value>
std::vector<Value> ConvertMapToList(const std::map<Key, Value>& inputMap) {
    std::vector<Value> valueList;
    valueList.reserve(inputMap.size()); // Reserve space for efficiency

    for (const auto& pair : inputMap) {
        if constexpr (std::is_same_v<Value, std::shared_ptr<Texture2D>>) {
            if (pair.second) {
                valueList.push_back(pair.second); // Copy the shared_ptr
            }
        } else {
            valueList.push_back(pair.second);
        }
    }

    return valueList;
}

// Function to convert a char to lowercase
void lower(char *str);

#endif // util_h