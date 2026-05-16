#pragma once

#include <memory>
#include <string>
#include <functional>
#include <vector>

/**
 * HavelScriptIntegration - Embeds Havel scripting language into the game engine
 * 
 * This class provides a clean interface for:
 * - Loading and executing Havel scripts
 * - Registering game engine functions for scripts to call
 * - Calling script functions from C++
 * - Managing the Havel lifecycle
 */
class GameRuntime; // Forward declare
class GameRuntime3D;

class HavelScriptIntegration {
public:
    HavelScriptIntegration();
    ~HavelScriptIntegration();

    HavelScriptIntegration(const HavelScriptIntegration&) = delete;
    HavelScriptIntegration& operator=(const HavelScriptIntegration&) = delete;

    HavelScriptIntegration(HavelScriptIntegration&&) noexcept;
    HavelScriptIntegration& operator=(HavelScriptIntegration&&) noexcept;

    bool initialize();
    bool isInitialized() const;
    bool executeString(const std::string& code, const std::string& sourceName = "inline");
    bool executeFile(const std::string& filepath);
    bool callFunction(const std::string& functionName, 
                      const std::vector<std::string>& args = {});
    std::string getLastError() const;
    void registerFunction(const std::string& name, 
        std::function<std::string(const std::vector<std::string>&)> fn);
    void setGameEngineContext(void* gameEngine);
    void shutdown();
    void registerDefaultBindings();
    void registerRuntimeBindings(GameRuntime* rt);
    void registerRuntime3DBindings(GameRuntime3D* rt3d);
    bool reload(const std::string& filepath);

    class Impl;
    std::unique_ptr<Impl> impl;

    void pushArgsToHavel(const std::vector<std::string>& args);
    bool callHavelFunction(const std::string& functionName, int nargs, int nresults);
};
