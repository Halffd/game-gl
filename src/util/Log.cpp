#include "Log.h"
#include <iostream>
#include <glm/glm.hpp> // Include GLM library for vector and matrix types
#include <glm/gtx/quaternion.hpp>

// Constructor
Log::Log(const std::string& root, const std::string& filename, Mode mode)
    : baseDir(root), logFileName(filename), logMode(mode) {
    openLogFile();
}

// Destructor
Log::~Log() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Log::info(const std::string& message) {
    log("INFO", message);
}

void Log::warning(const std::string& message) {
    log("WARNING", message);
}

void Log::error(const std::string& message) {
    log("ERROR", message);
}

// Overloaded log functions for GLM types
void Log::log(const std::string& level, const glm::vec2& vec) {
    log(level, util::to_string(vec));
}

void Log::log(const std::string& level, const glm::vec3& vec) {
    log(level, util::to_string(vec));
}

void Log::log(const std::string& level, const glm::vec4& vec) {
    log(level, util::to_string(vec));
}

void Log::log(const std::string& level, const glm::quat& quat) {
    log(level, util::to_string(quat));
}

void Log::log(const std::string& level, const glm::mat2& mat) {
    log(level, util::to_string(mat));
}

void Log::log(const std::string& level, const glm::mat3& mat) {
    log(level, util::to_string(mat));
}

void Log::log(const std::string& level, const glm::mat4& mat) {
    log(level, util::to_string(mat));
}

template <typename T>
void Log::log(const std::string& level, const std::vector<T>& vec) {
    std::ostringstream oss;
    oss << "{ ";
    for (const auto& item : vec) {
        oss << item << " ";
    }
    oss << "}";
    log(level, oss.str());
}

Log& Log::operator<<(const std::string& message) {
    info(message);
    return *this;
}

void Log::setDir(const std::string& directory) {
    baseDir = directory;
    openLogFile();
}

void Log::openLogFile() {
    if (baseDir.empty() || logFileName.empty()) {
        return;
    }

    std::string fullPath = baseDir + "/" + logFileName;
    logFile.open(fullPath, logMode == Mode::APPEND ? std::ios::app : std::ios::out | std::ios::trunc);

    if (!logFile.is_open()) {
        std::cerr << "Unable to open log file: " << fullPath << std::endl;
        std::cerr << "Error: " << std::strerror(errno) << std::endl;
    }
}

void Log::log(const std::string& level, const std::string& message) {
    std::lock_guard<std::mutex> guard(logMutex);

    if (logFile.is_open()) {
        logFile << currentDateTime() << " [" << level << "] " << message << std::endl;
    }
}

std::string Log::currentDateTime() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

namespace util {
    std::string to_string(const glm::vec2& vec) {
        std::ostringstream oss;
        oss << "vec2(" << vec.x << ", " << vec.y << ")";
        return oss.str();
    }

    std::string to_string(const glm::vec3& vec) {
        std::ostringstream oss;
        oss << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return oss.str();
    }

    std::string to_string(const glm::vec4& vec) {
        std::ostringstream oss;
        oss << "vec4(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
        return oss.str();
    }

    std::string to_string(const glm::quat& quat) {
        std::ostringstream oss;
        oss << "quat(" << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z << ")";
        return oss.str();
    }

    std::string to_string(const glm::mat2& mat) {
        std::ostringstream oss;
        oss << "mat2(\n[";
        for (int i = 0; i < 2; ++i) {
            oss << "[";
            for (int j = 0; j < 2; ++j) {
                oss << mat[i][j];
                if (j < 1) oss << ", ";
            }
            oss << "]";
            if (i < 1) oss << ", \n";
        }
        oss << "]\n)";
        return oss.str();
    }

    std::string to_string(const glm::mat3& mat) {
        std::ostringstream oss;
        oss << "mat3(\n[";
        for (int i = 0; i < 3; ++i) {
            oss << "[";
            for (int j = 0; j < 3; ++j) {
                oss << mat[i][j];
                if (j < 2) oss << ", ";
            }
            oss << "]";
            if (i < 2) oss << ", \n";
        }
        oss << "]\n)";
        return oss.str();
    }

    std::string to_string(const glm::mat4& mat) {
        std::ostringstream oss;
        oss << "mat4(\n[";
        for (int i = 0; i < 4; ++i) {
            oss << "[";
            for (int j = 0; j < 4; ++j) {
                oss << mat[i][j];
                if (j < 3) oss << ", ";
            }
            oss << "]";
            if (i < 3) oss << ", \n";
        }
        oss << "]\n)";
        return oss.str();
    }
}

// Instantiate the global logger
Log logger("", "game.log", Log::Mode::OVERWRITE);