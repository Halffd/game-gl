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
#include "GameRuntime.h"
#include <GLFW/glfw3.h>

class HavelScriptIntegration::Impl {
public:
    struct FunctionCtx {
        std::string name;
    };

    HavelState* H = nullptr;
    std::string lastError;
    bool initialized = false;
    void* gameContext = nullptr;
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
        if (!fnCtx || !H) return HAVEL_ERR;

        auto it = ((HavelScriptIntegration::Impl*)havel_getuserdata(H))->registeredFunctions.find(fnCtx->name);
        if (it == ((HavelScriptIntegration::Impl*)havel_getuserdata(H))->registeredFunctions.end()) return HAVEL_ERR;

        std::vector<std::string> args;
        int nargs = havel_gettop(H);
        for (int i = 0; i < nargs; i++) {
            const char* s = havel_tostring(H, i - nargs);
            if (s) args.push_back(s);
        }

        std::string result = it->second(args);
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
        bool hasGlobal = havel_hasglobal(impl->H, functionName.c_str());
        if (!hasGlobal) {
            std::cerr << "[DEBUG] Function '" << functionName << "' not defined in script, skipping" << std::endl;
            return false;
        }

        havel_getglobal(impl->H, functionName.c_str());
        
        for (const auto& arg : args) {
            havel_pushstring(impl->H, arg.c_str());
        }
        
        int result = havel_pcall(impl->H, (int)args.size(), 1, 0);
        
        if (result == HAVEL_ERR) {
            impl->lastError = havel_errmsg(impl->H);
            std::cerr << "[DEBUG] Function '" << functionName << "' error: " << impl->lastError << std::endl;
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

    impl->registeredFunctions[name] = fn;

    auto fnCtx = std::make_unique<Impl::FunctionCtx>();
    fnCtx->name = name;
    Impl::FunctionCtx* rawCtx = fnCtx.get();
    impl->functionContexts.push_back(std::move(fnCtx));

    havel_pushcfunction_with_ctx(impl->H, function_wrapper, rawCtx, name.c_str());
    havel_setglobal(impl->H, name.c_str());
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
        std::cout << msg << std::endl;
        return std::string("ok");
    });

    registerFunction("get_fps", [this](const std::vector<std::string>&) {
        if (impl->gameContext) {
            return std::string("fps query called");
        }
        return std::string("no game context");
    });

    registerFunction("get_level", [this](const std::vector<std::string>&) {
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

    registerFunction("reset_game", [this](const std::vector<std::string>&) {
        if (impl->gameContext) {
            auto* game = static_cast<Game*>(impl->gameContext);
            game->ResetPlayer();
            game->ResetLevel();
            return std::string("ok");
        }
        return std::string("error: no game context");
    });

    registerFunction("get_window_size", [this](const std::vector<std::string>&) {
        if (impl->gameContext) {
            auto* game = static_cast<Game*>(impl->gameContext);
            return std::to_string(game->Width) + " " + std::to_string(game->Height);
        }
        return std::string("0 0");
    });

    std::cout << "[Havel] Registered default bindings" << std::endl;
}

void HavelScriptIntegration::registerRuntimeBindings(GameRuntime* rt) {
    registerFunction("obj_rect", [rt](const std::vector<std::string>& a) {
        float x = a.size() > 0 ? std::stof(a[0]) : 0;
        float y = a.size() > 1 ? std::stof(a[1]) : 0;
        float w = a.size() > 2 ? std::stof(a[2]) : 32;
        float h = a.size() > 3 ? std::stof(a[3]) : 32;
        return std::to_string(rt->createObject("rect", x, y, w, h));
    });

    registerFunction("obj_set_x", [rt](const std::vector<std::string>& a) {
        if (a.size() < 2) return std::string("0");
        int idx = std::stoi(a[0]); float v = std::stof(a[1]);
        rt->getObject(idx).x = v; return std::string("1");
    });
    registerFunction("obj_get_x", [rt](const std::vector<std::string>& a) {
        if (a.empty()) return std::string("0");
        return std::to_string(rt->getObject(std::stoi(a[0])).x);
    });
    registerFunction("obj_set_y", [rt](const std::vector<std::string>& a) {
        if (a.size() < 2) return std::string("0");
        rt->getObject(std::stoi(a[0])).y = std::stof(a[1]); return std::string("1");
    });
    registerFunction("obj_get_y", [rt](const std::vector<std::string>& a) {
        if (a.empty()) return std::string("0");
        return std::to_string(rt->getObject(std::stoi(a[0])).y);
    });
    registerFunction("obj_set_color", [rt](const std::vector<std::string>& a) {
        if (a.size() < 4) return std::string("0");
        int idx = std::stoi(a[0]);
        rt->getObject(idx).r = std::stof(a[1]);
        rt->getObject(idx).g = std::stof(a[2]);
        rt->getObject(idx).b = std::stof(a[3]);
        return std::string("1");
    });

    registerFunction("key_down", [rt](const std::vector<std::string>& a) {
        if (a.empty()) return std::string("0");
        int key = 0;
        auto& k = a[0];
        if (k == "up") key = GLFW_KEY_UP; else if (k == "down") key = GLFW_KEY_DOWN;
        else if (k == "left") key = GLFW_KEY_LEFT; else if (k == "right") key = GLFW_KEY_RIGHT;
        else if (k == "w" || k == "W") key = GLFW_KEY_W; else if (k == "s" || k == "S") key = GLFW_KEY_S;
        else if (k == "a" || k == "A") key = GLFW_KEY_A; else if (k == "d" || k == "D") key = GLFW_KEY_D;
        else if (k == "space") key = GLFW_KEY_SPACE; else if (k == "enter") key = GLFW_KEY_ENTER;
        bool pressed = rt->keyDown(key);
        static int frameCnt = 0;
        if (k == "w" && frameCnt++ % 30 == 0)
            std::cerr << "[DEBUG] key_down(w) = " << pressed << std::endl;
        return pressed ? std::string("1") : std::string("0");
    });

    registerFunction("delta", [rt](const std::vector<std::string>&) {
        return std::to_string(rt->delta());
    });

    registerFunction("get_width", [rt](const std::vector<std::string>&) {
        return std::to_string(rt->getWidth());
    });

    registerFunction("get_height", [rt](const std::vector<std::string>&) {
        return std::to_string(rt->getHeight());
    });

    registerFunction("time", [rt](const std::vector<std::string>&) {
        return std::to_string(rt->getTime());
    });

    registerFunction("obj_destroy", [rt](const std::vector<std::string>& a) {
        if (!a.empty()) rt->destroyObject(std::stoi(a[0]));
        return std::string("1");
    });

    registerFunction("obj_count", [rt](const std::vector<std::string>&) {
        return std::to_string(rt->gameObjectCount());
    });

    registerFunction("obj_set_w", [rt](const std::vector<std::string>& a) {
        if (a.size() >= 2) rt->getObject(std::stoi(a[0])).w = std::stof(a[1]);
        return std::string("1");
    });
    registerFunction("obj_set_h", [rt](const std::vector<std::string>& a) {
        if (a.size() >= 2) rt->getObject(std::stoi(a[0])).h = std::stof(a[1]);
        return std::string("1");
    });

    registerFunction("obj_hide", [rt](const std::vector<std::string>& a) {
        if (!a.empty()) rt->getObject(std::stoi(a[0])).hidden = true;
        return std::string("1");
    });

    // Run the game loop: calls the script's init/input/update/render functions by name
    registerFunction("runtime_run", [this, rt](const std::vector<std::string>& a) {
        std::string initFn = a.size() > 0 ? a[0] : "init";
        std::string inputFn = a.size() > 1 ? a[1] : "input";
        std::string updateFn = a.size() > 2 ? a[2] : "update";
        std::string renderFn = a.size() > 3 ? a[3] : "render";

        havel_call_named_function(impl->H, initFn.c_str());

        while (!rt->shouldClose()) {
            rt->beginFrame();
            havel_call_named_function(impl->H, inputFn.c_str());
            float dt = rt->delta();
            havel_call_named_function(impl->H, updateFn.c_str());
            for (int i = 0; i < rt->gameObjectCount(); i++)
                rt->draw(i);
            havel_call_named_function(impl->H, renderFn.c_str());
            rt->endFrame();
        }
        return std::string("done");
    });
}
