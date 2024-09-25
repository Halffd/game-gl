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
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
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

// Utility functions for GLM types
namespace util {
    std::string to_string(const glm::vec2& vec);
    std::string to_string(const glm::vec3& vec);
    std::string to_string(const glm::vec4& vec);
    std::string to_string(const glm::quat& quat);
    std::string to_string(const glm::mat2& mat);
    std::string to_string(const glm::mat3& mat);
    std::string to_string(const glm::mat4& mat);
}

#include "Util.hpp" // Include the Util header

class Log {
public:
    enum class Mode {
        APPEND,
        OVERWRITE
    };

    Log() = default;

    Log(const std::string& root, const std::string& filename, Mode mode)
        : baseDir(root), logFileName(filename), logMode(mode) {
        openLogFile();
    }

    ~Log() {
        if (logFile.is_open()) {
            //logFile.close();
        }
    }

    void info(const std::string& message) {
        log("INFO", message);
    }

    void warning(const std::string& message) {
        log("WARNING", message);
    }

    void error(const std::string& message) {
        log("ERROR", message);
    }

    // Overloaded log functions for GLM types
    void log(const std::string& level, const glm::vec2& vec) {
        log(level, util::to_string(vec));
    }

    void log(const std::string& level, const glm::vec3& vec) {
        log(level, util::to_string(vec));
    }

    void log(const std::string& level, const glm::vec4& vec) {
        log(level, util::to_string(vec));
    }

    void log(const std::string& level, const glm::quat& quat) {
        log(level, util::to_string(quat));
    }

    void log(const std::string& level, const glm::mat2& mat) {
        log(level, util::to_string(mat));
    }

    void log(const std::string& level, const glm::mat3& mat) {
        log(level, util::to_string(mat));
    }

    void log(const std::string& level, const glm::mat4& mat) {
        log(level, util::to_string(mat));
    }

    template <typename T>
    void log(const std::string& level, const std::vector<T>& vec) {
        std::ostringstream oss;
        oss << "{ ";
        for (const auto& item : vec) {
            oss << item << " ";
        }
        oss << "}";
        log(level, oss.str());
    }

    void setDir(std::string directory) {
        baseDir = directory;
        openLogFile();
    }

private:
    std::string baseDir;
    std::string logFileName;
    Mode logMode;
    std::ofstream logFile;
    std::mutex logMutex;

    void openLogFile() {
        if(baseDir == "") {
            return;
        }
        std::string fullPath = baseDir + "/" + logFileName;

        // Attempt to open the log file
        logFile.open(fullPath, logMode == Mode::APPEND ? std::ios::app : std::ios::out | std::ios::trunc);

        // Check if the log file opened successfully
        if (!logFile.is_open()) {
            std::cerr << "Unable to open log file: " << fullPath << std::endl;
            std::cerr << "Error: " << std::strerror(errno) << std::endl; // Print system error message
        } else {
            std::cout << "Log file opened successfully: " << fullPath << std::endl;
        }
    }

    void log(const std::string& level, const std::string& message) {
        std::lock_guard<std::mutex> guard(logMutex);

        if (logFile.is_open()) {
            logFile << currentDateTime() << " [" << level << "] " << message << std::endl;
        }
    }

    std::string currentDateTime() {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};
extern Log logger;
#endif  // util_h