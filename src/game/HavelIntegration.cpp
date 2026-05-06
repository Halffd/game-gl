#include "HavelIntegration.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <unordered_map>
#include <functional>
#include <memory>

#include <havel.h>
#include "Game.h"

class HavelScriptIntegration::Impl {
public:
    struct FunctionCtx {
        Impl* impl;
        std::string name;
    };

    HavelState* H = nullptr;
    std::string lastError;
    bool initialized = false;
    void* gameContext = nullptr;  // Game* stored separately, not in userdata
    std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>> registeredFunctions;
    std::vector<std::unique_ptr<FunctionCtx>> functionContexts;

    Impl() = default;
    ~Impl() {
        if (H) {
            havel_close(H);
        }
    }
};

namespace {
    int function_wrapper(HavelState* H, void* ctx) {
        auto* fnCtx = static_cast<HavelScriptIntegration::Impl::FunctionCtx*>(ctx);
        if (!fnCtx || !fnCtx->impl) return HAVEL_ERR;

        fprintf(stderr, "[Binding] %s called, gameContext=%p\n", fnCtx->name.c_str(), fnCtx->impl->gameContext);

        auto it = fnCtx->impl->registeredFunctions.find(fnCtx->name);
        if (it == fnCtx->impl->registeredFunctions.end()) {
            fprintf(stderr, "[Binding] %s NOT found in registeredFunctions\n", fnCtx->name.c_str());
            return HAVEL_ERR;
        }

        std::vector<std::string> args;
        int nargs = havel_gettop(H);
        for (int i = 0; i < nargs; i++) {
            const char* s = havel_tostring(H, i - nargs);
            if (s) args.push_back(s);
        }

        std::string result = it->second(args);
        fprintf(stderr, "[Binding] %s returns: '%s'\n", fnCtx->name.c_str(), result.c_str());

        havel_pushstring(H, result.c_str());
        return HAVEL_OK;
    }
}

HavelScriptIntegration::HavelScriptIntegration()
    : impl(std::make_unique<Impl>()) {
}

HavelScriptIntegration::~HavelScriptIntegration() {
    shutdown();
}

HavelScriptIntegration::HavelScriptIntegration(HavelScriptIntegration&&) noexcept = default;
HavelScriptIntegration& HavelScriptIntegration::operator=(HavelScriptIntegration&&) noexcept = default;

bool HavelScriptIntegration::initialize() {
    if (impl->initialized) {
        return true;
    }

    try {
        impl->H = havel_newstate();
        if (!impl->H) {
            impl->lastError = "Failed to create Havel state";
            return false;
        }
        
        havel_setuserdata(impl->H, (void*)impl.get());
        
        impl->initialized = true;
        std::cout << "[Havel] Scripting engine initialized (C API mode)" << std::endl;
        return true;
    } catch (const std::exception& e) {
        impl->lastError = std::string("Failed to initialize Havel: ") + e.what();
        std::cerr << "[Havel] Error: " << impl->lastError << std::endl;
        return false;
    }
}

bool HavelScriptIntegration::isInitialized() const {
    return impl->initialized;
}

bool HavelScriptIntegration::executeString(const std::string& code, const std::string& sourceName) {
    if (!impl->initialized || !impl->H) {
        impl->lastError = "Havel not initialized";
        return false;
    }

    int result = havel_loadstring(impl->H, code.c_str(), sourceName.c_str());
    if (result != 0) {
        impl->lastError = "Script execution failed";
        std::cerr << "[Havel] Execution error in " << sourceName << ": " << impl->lastError << std::endl;
        return false;
    }

    return true;
}

bool HavelScriptIntegration::executeFile(const std::string& filepath) {
    if (!impl->initialized || !impl->H) {
        impl->lastError = "Havel not initialized";
        return false;
    }

    int result = havel_loadfile(impl->H, filepath.c_str());
    if (result != 0) {
        impl->lastError = "File load/execution failed: " + std::string(havel_errmsg(impl->H));
        std::cerr << "[Havel] Error in file: " << filepath << " - " << impl->lastError << std::endl;
        return false;
    }

    return true;
}

bool HavelScriptIntegration::callFunction(const std::string& functionName, 
                      const std::vector<std::string>& args) {
    if (!impl->initialized || !impl->H) {
        impl->lastError = "Havel not initialized";
        return false;
    }

    try {
        havel_getglobal(impl->H, functionName.c_str());
        
        for (const auto& arg : args) {
            havel_pushstring(impl->H, arg.c_str());
        }
        
        int result = havel_pcall(impl->H, (int)args.size(), 1, 0);
        
        if (result == HAVEL_ERR) {
            impl->lastError = havel_errmsg(impl->H);
            return false;
        }
        
        havel_pop(impl->H, 1);
        
        return true;
    } catch (const std::exception& e) {
        impl->lastError = e.what();
        std::cerr << "[Havel] Error calling function " << functionName << ": " << impl->lastError << std::endl;
        return false;
    }
}

std::string HavelScriptIntegration::getLastError() const {
    return impl->lastError;
}

void HavelScriptIntegration::registerFunction(const std::string& name, 
    std::function<std::string(const std::vector<std::string>&)> fn) {
    if (!impl->initialized || !impl->H) {
        std::cerr << "[Havel] Cannot register function '" << name << "': Havel not initialized" << std::endl;
        return;
    }

    try {
        impl->registeredFunctions[name] = fn;

        auto fnCtx = std::make_unique<Impl::FunctionCtx>();
        fnCtx->impl = impl.get();
        fnCtx->name = name;
        Impl::FunctionCtx* rawCtx = fnCtx.get();
        impl->functionContexts.push_back(std::move(fnCtx));
        havel_pushcfunction_with_ctx(impl->H, function_wrapper, rawCtx, name.c_str());
        havel_setglobal(impl->H, name.c_str());
        
    } catch (const std::exception& e) {
        std::cerr << "[Havel] Error registering function '" << name << "': " << e.what() << std::endl;
    }
}

void HavelScriptIntegration::setGameEngineContext(void* gameEngine) {
    impl->gameContext = gameEngine;
}

void HavelScriptIntegration::shutdown() {
    if (!impl->initialized) return;
    
    if (impl->H) {
        havel_close(impl->H);
        impl->H = nullptr;
    }
    
    impl->initialized = false;
    std::cout << "[Havel] Scripting engine shutdown" << std::endl;
}

bool HavelScriptIntegration::reload(const std::string& filepath) {
    if (!impl->initialized) return false;
    shutdown();
    if (!initialize()) return false;
    return executeFile(filepath);
}

void HavelScriptIntegration::registerDefaultBindings() {
    registerFunction("print", [](const std::vector<std::string>& args) {
        std::string msg;
        for (size_t i = 0; i < args.size(); i++) {
            if (i > 0) msg += " ";
            msg += args[i];
        }
        std::cout << "[Script] " << msg << std::endl;
        return std::string("ok");
    });

    registerFunction("get_fps", [this](const std::vector<std::string>& args) {
        fprintf(stderr, "[Binding] get_fps called, gameContext=%p\n", impl->gameContext);
        if (impl->gameContext) {
            auto* game = static_cast<Game*>(impl->gameContext);
            return std::string("fps query called");
        }
        return std::string("no game context");
    });

    registerFunction("get_level", [this](const std::vector<std::string>& args) {
        if (impl->gameContext) {
            auto* game = static_cast<Game*>(impl->gameContext);
            return std::to_string(game->Level);
        }
        return std::string("0");
    });

    registerFunction("set_level", [this](const std::vector<std::string>& args) {
        if (!impl->gameContext || args.empty()) return std::string("error: missing level number");
        auto* game = static_cast<Game*>(impl->gameContext);
        try {
            int lvl = std::stoi(args[0]);
            if (lvl >= 0 && lvl < (int)game->Levels.size()) {
                game->Level = lvl;
                return std::string("ok");
            }
        } catch (...) {}
        return std::string("error: invalid level");
    });

    registerFunction("reset_game", [this](const std::vector<std::string>& args) {
        if (impl->gameContext) {
            auto* game = static_cast<Game*>(impl->gameContext);
            game->ResetPlayer();
            game->ResetLevel();
            return std::string("ok");
        }
        return std::string("error: no game context");
    });

    registerFunction("get_window_size", [this](const std::vector<std::string>& args) {
        if (impl->gameContext) {
            auto* game = static_cast<Game*>(impl->gameContext);
            return std::to_string(game->Width) + " " + std::to_string(game->Height);
        }
        return std::string("0 0");
    });

    std::cout << "[Havel] Registered default bindings (print, get_fps, get_level, set_level, reset_game, get_window_size)" << std::endl;
}
