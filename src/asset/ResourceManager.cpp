#include "ResourceManager.h"

#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>

#include "util/Util.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

std::map<std::string, std::shared_ptr<Shader>> ResourceManager::Shaders;
std::map<std::string, Texture1D> ResourceManager::Textures1D;
std::map<std::string, std::shared_ptr<Texture2D>> ResourceManager::Textures2D;
std::map<std::string, Texture3D> ResourceManager::Textures3D;

// Static member initialization
std::string ResourceManager::root = "";

// Implement full path handling for shaders and textures
const char *ResourceManager::GetFullPath(const std::string &filename)
{
    // Use a thread-local static buffer to avoid memory leaks
    // This is only safe because the path is used immediately for file operations
    static thread_local char buffer[BUFFER_SIZE];
    size_t len = std::min(filename.length(), (size_t)(BUFFER_SIZE - 1));
    std::memcpy(buffer, filename.c_str(), len);
    buffer[len] = '\0';
    std::cout << "GetFullPath: " << buffer << std::endl;
    return buffer;
}

const char *ResourceManager::GetModelPath(const std::string &filename)
{
    std::string path;
    if (root.empty()) {
        path = "models/" + filename;
    } else {
        path = root + "/models/" + filename;
    }
    
    // Check if the file exists
    std::ifstream fileCheck(path);
    if (!fileCheck.good()) {
        std::cout << "WARNING: Model file does not exist at path: " << path << std::endl;
        
        // Try bin/models as an alternative
        std::string binPath = root + "/bin/models/" + filename;
        std::ifstream binFileCheck(binPath);
        if (binFileCheck.good()) {
            std::cout << "Found model in bin/models instead: " << binPath << std::endl;
            path = binPath;
        }
    } else {
        std::cout << "Model file exists at: " << path << std::endl;
    }
    
    return GetFullPath(path);
}

const char *ResourceManager::GetShaderPath(const std::string &filename)
{
    std::string path;
    if (root.empty()) {
        path = "shaders/" + filename;
    } else {
        path = root + "/shaders/" + filename;
    }

    // Check if the file exists
    std::ifstream fileCheck(path);
    if (!fileCheck.good()) {
        std::cout << "WARNING: Shader file does not exist at path: " << path << std::endl;

        // Try bin/shaders as an alternative
        std::string binPath = root + "/bin/shaders/" + filename;
        std::ifstream binFileCheck(binPath);
        if (binFileCheck.good()) {
            std::cout << "Found shader in bin/shaders instead: " << binPath << std::endl;
            path = binPath;
        }
    } else {
        std::cout << "Shader file exists at: " << path << std::endl;
    }

    return GetFullPath(path);
}

const char *ResourceManager::GetTexturePath(const std::string &filename)
{
    std::string path;
    if (root.empty()) {
        path = "textures/" + filename;
    } else {
        path = root + "/textures/" + filename;
    }
    
    // Check if the file exists
    std::ifstream fileCheck(path);
    if (!fileCheck.good()) {
        std::cout << "WARNING: Texture file does not exist at path: " << path << std::endl;
        
        // Try bin/textures as an alternative
        std::string binPath = root + "/bin/textures/" + filename;
        std::ifstream binFileCheck(binPath);
        if (binFileCheck.good()) {
            std::cout << "Found texture in bin/textures instead: " << binPath << std::endl;
            path = binPath;
        }
    } else {
        std::cout << "Texture file exists at: " << path << std::endl;
    }
    
    return GetFullPath(path);
}

const char *ResourceManager::GetPath(const std::string &filename)
{
    std::string path = root + "/" + filename;
    std::cout << "GetPath: " << path << std::endl;
    return GetFullPath(path);
}

Shader& ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, std::string name)
{
    return LoadShader(vShaderFile, fShaderFile, nullptr, name);
}
Shader& ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name)
{
    // Make copies of the original paths to avoid overwriting issues
    std::string origVShader = vShaderFile ? vShaderFile : "";
    std::string origFShader = fShaderFile ? fShaderFile : "";
    std::string origGShader = gShaderFile ? gShaderFile : "";

    const char* resolvedVShader = vShaderFile;
    const char* resolvedFShader = fShaderFile;
    const char* resolvedGShader = gShaderFile;

    if (!includes(vShaderFile, ":"))
    {
        resolvedVShader = GetShaderPath(origVShader.c_str());
        // Make a copy by converting to string and back to char* to ensure we have separate memory
        origVShader = std::string(resolvedVShader);
        resolvedVShader = origVShader.c_str();
    }
    if (!includes(fShaderFile, ":"))
    {
        resolvedFShader = GetShaderPath(origFShader.c_str());
        // Make a copy by converting to string and back to char* to ensure we have separate memory
        origFShader = std::string(resolvedFShader);
        resolvedFShader = origFShader.c_str();
    }
    if (gShaderFile && !includes(gShaderFile, ":"))
    {
        resolvedGShader = GetShaderPath(origGShader.c_str());
        origGShader = std::string(resolvedGShader);
        resolvedGShader = origGShader.c_str();
    }

    //std::cout << "Loading shader: " << resolvedVShader << " " << resolvedFShader << " " << resolvedGShader << std::endl;
    auto shader = std::make_shared<Shader>();
    loadShaderFromFile(*shader, resolvedVShader, resolvedFShader, resolvedGShader);
    Shaders[name] = shader;
    return *shader;
}

Shader& ResourceManager::GetShader(std::string name)
{
    auto it = Shaders.find(name);
    if (it == Shaders.end())
    {
        throw std::runtime_error("Shader not found: " + name);
    }
    return *it->second;
}
Shader *ResourceManager::ShaderP(std::string &name)
{
    return Shaders[name].get(); // Assuming Shaders is a map or similar structure
}

Texture1D ResourceManager::LoadTexture1D(const char *file, bool alpha, std::string name,
                                         GLint sWrap, GLint minFilter, GLint magFilter)
{
    Textures1D[name] = loadTexture1DFromFile(file, alpha, sWrap, minFilter, magFilter);
    return Textures1D[name];
}

Texture1D ResourceManager::GetTexture1D(std::string name)
{
    return Textures1D[name];
}

Texture2D ResourceManager::LoadTexture2D(const char *file, std::string name, bool alpha, GLint sWrap, GLint tWrap, GLint minFilter, GLint magFilter)
{
    if (name.empty())
    {
        name = file;
    }
    if (!includes(file, ":"))
    {
        file = GetTexturePath(file);
    }
    auto ptr = std::make_shared<Texture2D>(loadTexture2DFromFile(file, alpha, sWrap, tWrap, minFilter, magFilter));
    Textures2D[file] = ptr;
    if(file != name){
        Textures2D[name] = ptr;
    }
    return *ptr;
}

Texture2D ResourceManager::GetTexture2D(std::string name)
{
    if (Textures2D.find(name) == Textures2D.end())
    {
        LoadTexture2D(name.c_str(), "");
    }
    return *Textures2D[name];
}
Texture2D *ResourceManager::GetTexture(std::string name)
{
    auto it = Textures2D.find(name);
    if (it != Textures2D.end())
    {
        // Return the raw pointer from the shared_ptr without creating a new instance
        return it->second.get(); // Use .get() to get the raw pointer
    }
    return nullptr; // Texture not found
}

Texture3D ResourceManager::LoadTexture3D(const char *file, bool alpha, std::string name,
                                         GLint sWrap, GLint tWrap, GLint rWrap,
                                         GLint minFilter, GLint magFilter)
{
    Textures3D[name] = loadTexture3DFromFile(file, alpha, sWrap, tWrap, rWrap, minFilter, magFilter);
    return Textures3D[name];
}

Texture3D ResourceManager::GetTexture3D(std::string name)
{
    return Textures3D[name];
}
std::shared_ptr<Texture2D> ResourceManager::GetTexture2DByIndex(size_t index)
{
    static std::vector<std::shared_ptr<Texture2D>> textureList = ConvertMapToList(Textures2D);

    if (index >= textureList.size())
    {
        throw std::out_of_range("Index out of range");
    }

    return textureList[index]; // Return the shared_ptr directly
}

void ResourceManager::LoadAllTexturesFromDirectory()
{
    namespace fs = std::filesystem;

    // Iterate over files in the directory
    for (const auto &entry : fs::directory_iterator(ResourceManager::root + "/textures"))
    {
        std::string path = entry.path().string();

        // Filter image files based on extension (e.g., png, jpg, jpeg)
        if (entry.path().extension() == ".png" || entry.path().extension() == ".jpg" || entry.path().extension() == ".jpeg")
        {
            std::string filename = entry.path().stem().string(); // File name without extension

            // Load texture and add to the map
            ResourceManager::LoadTexture2D(path.c_str(), filename);
            std::cout << "Loaded texture: " << filename << " from path: " << path;
        }
    }
}
void ResourceManager::Clear() {
    // Clear shaders
    for (auto& iter : Shaders) {
        glDeleteProgram(iter.second->ID);
    }

    // Clear 1D textures
    for (auto& iter : Textures1D) {
        glDeleteTextures(1, &iter.second.ID);
    }

    // Clear 2D textures
    for (auto& iter : Textures2D) {
        if (iter.second) { // Check if the shared_ptr is valid
            glDeleteTextures(1, &iter.second->ID); // Use -> for dereferencing
        }
    }

    // Clear 3D textures
    for (auto& iter : Textures3D) {
        glDeleteTextures(1, &iter.second.ID);
    }
}
void ResourceManager::loadShaderFromFile(Shader &shader, const char *vShaderFile, const char *fShaderFile, const char *gShaderFile)
{
    std::string vertexCode, fragmentCode, geometryCode;
    try
    {
        std::cout << "Loading vertex shader from: " << vShaderFile << std::endl;
        std::cout << "Loading fragment shader from: " << fShaderFile << std::endl;
        if (gShaderFile) {
            std::cout << "Loading geometry shader from: " << gShaderFile << std::endl;
        }
        
        std::ifstream vertexShaderFile(vShaderFile);
        if (!vertexShaderFile.is_open()) {
            std::cout << "ERROR::SHADER: Failed to open vertex shader file: " << vShaderFile << std::endl;
            throw std::runtime_error("Failed to open vertex shader file");
        }
        
        std::ifstream fragmentShaderFile(fShaderFile);
        if (!fragmentShaderFile.is_open()) {
            std::cout << "ERROR::SHADER: Failed to open fragment shader file: " << fShaderFile << std::endl;
            throw std::runtime_error("Failed to open fragment shader file");
        }
        
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        vertexShaderFile.close();
        fragmentShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        
        std::cout << "Vertex shader code length: " << vertexCode.length() << std::endl;
        std::cout << "Fragment shader code length: " << fragmentCode.length() << std::endl;
        
        if (gShaderFile != nullptr)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            if (!geometryShaderFile.is_open()) {
                std::cout << "ERROR::SHADER: Failed to open geometry shader file: " << gShaderFile << std::endl;
                throw std::runtime_error("Failed to open geometry shader file");
            }
            
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
            
            std::cout << "Geometry shader code length: " << geometryCode.length() << std::endl;
        }
    }
    catch (std::exception &e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files: " << e.what() << std::endl;
        throw;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    const char *gShaderCode = geometryCode.c_str();
    try {
        shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
        std::cout << "Shader compilation successful" << std::endl;
    } catch (std::exception &e) {
        std::cout << "ERROR::SHADER: Failed to compile shader: " << e.what() << std::endl;
        throw;
    }
}

Texture1D ResourceManager::loadTexture1DFromFile(const char *file, bool alpha,
                                                 GLint sWrap, GLint minFilter, GLint magFilter)
{
    Texture1D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    int width, nrChannels;
    unsigned char *data = stbi_load(file, &width, nullptr, &nrChannels, 0);
    texture.Wrap_S = sWrap;
    texture.Filter_Min = minFilter;
    texture.Filter_Max = magFilter;
    texture.Generate(width, data);
    stbi_image_free(data);
    return texture;
}

Texture2D ResourceManager::loadTexture2DFromFile(const char *file, bool alpha, GLint sWrap, GLint tWrap, GLint minFilter, GLint magFilter)
{
    Texture2D texture;

    int width, height, nrChannels;
    // stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (!data)
    {
        printf("Failed to load texture: %s\n", file);
        std::cerr << "Failed to load texture: " << file << std::endl;
        texture.status = -1;
        return texture;
    }
    else
    {
        texture.status = 1;
    }

    texture.Wrap_S = sWrap;
    texture.Wrap_T = tWrap;
    texture.Filter_Min = minFilter;
    texture.Filter_Max = magFilter;
    if (alpha || nrChannels > 3)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    else
    {
        texture.Internal_Format = GL_RGB;
        texture.Image_Format = GL_RGB;
    }
    texture.Generate(width, height, data);

    stbi_image_free(data);
    return texture;
}

Texture3D ResourceManager::loadTexture3DFromFile(const char *file, bool alpha,
                                                 GLint sWrap, GLint tWrap, GLint rWrap,
                                                 GLint minFilter, GLint magFilter)
{
    Texture3D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    int width, height, depth = 1, nrChannels;
    unsigned char *data = stbi_load(file, &width, &height, &nrChannels, 0);
    texture.Wrap_S = sWrap;
    texture.Wrap_T = tWrap;
    texture.Wrap_R = rWrap;
    texture.Filter_Min = minFilter;
    texture.Filter_Max = magFilter;
    texture.Generate(width, height, depth, data);
    stbi_image_free(data);
    return texture;
}

std::string ResourceManager::getExecutablePath()
{
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    return std::string(path);
#elif defined(__linux__) || defined(__APPLE__)
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count == -1)
    {
        return "Error retrieving path";
    }
    return std::string(path, count);
#else
    return "Unsupported platform";
#endif
}

std::string ResourceManager::getExecutableName()
{
    std::string fullPath = getExecutablePath();
    size_t lastSlash = fullPath.find_last_of("/\\");
    return fullPath.substr(lastSlash + 1);
}
std::string ResourceManager::getExecutableDir()
{
    std::string fullPath = getExecutablePath();
    size_t lastSlash = fullPath.find_last_of("/\\");

    // Ensure lastSlash is valid before using it in substr
    if (lastSlash != std::string::npos)
    {
        return fullPath.substr(0, lastSlash + 1); // Include the last slash
    }

    // Return an empty string or handle the error if no slash was found
    return "";
}

std::string ResourceManager::resolveShaderPath(const std::string& filename)
{
    std::string path;
    if (root.empty()) {
        path = "shaders/" + filename;
    } else {
        path = root + "/shaders/" + filename;
    }

    // Check if the file exists
    std::ifstream fileCheck(path);
    if (!fileCheck.good()) {
        std::cout << "WARNING: Shader file does not exist at path: " << path << std::endl;

        // Try bin/shaders as an alternative
        std::string binPath = root + "/bin/shaders/" + filename;
        std::ifstream binFileCheck(binPath);
        if (binFileCheck.good()) {
            std::cout << "Found shader in bin/shaders instead: " << binPath << std::endl;
            path = binPath;
        }
    } else {
        std::cout << "Shader file exists at: " << path << std::endl;
    }

    return path;
}

std::string ResourceManager::resolveTexturePath(const std::string& filename)
{
    std::string path;
    if (root.empty()) {
        path = "textures/" + filename;
    } else {
        path = root + "/textures/" + filename;
    }

    // Check if the file exists
    std::ifstream fileCheck(path);
    if (!fileCheck.good()) {
        std::cout << "WARNING: Texture file does not exist at path: " << path << std::endl;

        // Try bin/textures as an alternative
        std::string binPath = root + "/bin/textures/" + filename;
        std::ifstream binFileCheck(binPath);
        if (binFileCheck.good()) {
            std::cout << "Found texture in bin/textures instead: " << binPath << std::endl;
            path = binPath;
        }
    } else {
        std::cout << "Texture file exists at: " << path << std::endl;
    }

    return path;
}