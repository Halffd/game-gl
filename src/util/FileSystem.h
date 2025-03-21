#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class FileSystem {
public:
    static std::string readShaderFile(const std::string& filePath) {
        std::string content;
        std::ifstream fileStream;
        
        // Ensure ifstream objects can throw exceptions
        fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        
        try {
            // Open file
            fileStream.open(filePath);
            std::stringstream stringStream;
            
            // Read file's buffer contents into stream
            stringStream << fileStream.rdbuf();
            
            // Close file handler
            fileStream.close();
            
            // Convert stream into string
            content = stringStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << std::endl;
            std::cerr << e.what() << std::endl;
            content = ""; // Return empty string on failure
        }
        
        return content;
    }
}; 