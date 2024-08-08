#include <glad/glad.h>
#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#else
#include <execinfo.h>
#include <cxxabi.h>
#endif

#include "Util.hpp"


void glCheckError(const char *file, int line)
{
    GLenum errorCode = glGetError();
    while (errorCode != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:
                error = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "GL_INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "GL_STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "GL_STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                error = "Unknown error";
                break;
        }

        // Get the call stack
        std::vector<std::string> callStack;
#ifdef _WIN32
        // Windows implementation
        HANDLE process = GetCurrentProcess();
        HANDLE thread = GetCurrentThread();
        CONTEXT context;
        RtlCaptureContext(&context);
        STACKFRAME64 stackFrame = {0};
        stackFrame.AddrPC.Offset = context.Rip;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = context.Rsp;
        stackFrame.AddrStack.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = context.Rbp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;

        while (StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &stackFrame, &context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
        {
            DWORD64 displacement = 0;
            CHAR symbol[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
            PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbol;
            pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            pSymbol->MaxNameLen = MAX_SYM_NAME;
            if (SymFromAddr(process, stackFrame.AddrPC.Offset, &displacement, pSymbol))
            {
                callStack.push_back(pSymbol->Name);
            }
            else
            {
                callStack.push_back("Unknown function");
            }
        }
#else
        // Linux/Unix implementation
        void *callStackArray[128];
        int callStackSize = backtrace(callStackArray, 128);
        char **callStackStrings = backtrace_symbols(callStackArray, callStackSize);
        for (int i = 0; i < callStackSize; ++i)
        {
            std::string callStackEntry(callStackStrings[i]);
            int status;
            char *demangledName = abi::__cxa_demangle(callStackEntry.c_str(), NULL, NULL, &status);
            if (status == 0)
            {
                callStack.push_back(demangledName);
                free(demangledName);
            }
            else
            {
                callStack.push_back(callStackEntry);
            }
        }
        free(callStackStrings);
#endif

        std::stringstream ss;
        ss << "OpenGL error: " << error << " at " << file << ":" << line << std::endl;
        ss << "Call stack:" << std::endl;
        for (const auto &entry : callStack)
        {
            ss << "    " << entry << std::endl;
        }
        std::cout << ss.str() << std::endl;

        errorCode = glGetError();
    }
}
void glCheckError()
{
    GLenum errorCode = glGetError();

    while (errorCode != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:
            error = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "GL_INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "GL_STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "GL_STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "GL_OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        default:
            error = "Unknown error";
            break;
        }

        std::cout << "OpenGL error: " << error << std::endl;
        errorCode = glGetError();
    }
}
