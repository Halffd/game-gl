#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <cerrno>
#include <cstring>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

// Utility class for `glm` types to string conversion
namespace util {
    std::string to_string(const glm::vec2&);
    std::string to_string(const glm::vec3&);
    std::string to_string(const glm::vec4&);
    std::string to_string(const glm::quat&);
    std::string to_string(const glm::mat2&);
    std::string to_string(const glm::mat3&);
    std::string to_string(const glm::mat4&);
}

class Log {
public:
    enum class Mode {
        APPEND,
        OVERWRITE
    };

    Log() = default;
    Log(const std::string& root, const std::string& filename, Mode mode);
    ~Log();

    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);

    // Overloaded log functions for GLM types
    void log(const std::string& level, const glm::vec2& vec);
    void log(const std::string& level, const glm::vec3& vec);
    void log(const std::string& level, const glm::vec4& vec);
    void log(const std::string& level, const glm::quat& quat);
    void log(const std::string& level, const glm::mat2& mat);
    void log(const std::string& level, const glm::mat3& mat);
    void log(const std::string& level, const glm::mat4& mat);

    template <typename T>
    void log(const std::string& level, const std::vector<T>& vec);

    Log& operator<<(const std::string& message);

    void setDir(const std::string& directory);

private:
    std::string baseDir;
    std::string logFileName;
    Mode logMode = Mode::OVERWRITE;
    std::ofstream logFile;
    std::mutex logMutex;

    void openLogFile();
    void log(const std::string& level, const std::string& message);
    std::string currentDateTime();
};

// Declare a global logger instance
extern Log logger;

#endif // LOG_H
