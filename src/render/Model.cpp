#include "Model.h"
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <fstream>

using namespace std;

namespace m3D 
{
    Model::Model(const std::string &path, bool gamma) : gammaCorrection(gamma) // No default value here
    {
        std::cout << "Loading model from path: " << path << std::endl;
        
        // Check if the file exists before trying to load it
        std::ifstream fileCheck(path);
        if (!fileCheck.good()) {
            std::cout << "ERROR: Model file does not exist: " << path << std::endl;
            throw std::runtime_error("Model file not found: " + path);
        }
        fileCheck.close();
        
        loadModel(path);
    }

    void Model::Draw(Shader &shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    void Model::loadModel(const std::string &path)
    {
        std::cout << "Starting Assimp import for: " << path << std::endl;
        
        // Check if this is a GLTF file
        bool isGltf = path.find(".gltf") != std::string::npos || path.find(".glb") != std::string::npos;
        
        // Set up import flags
        unsigned int importFlags = aiProcess_Triangulate | 
                                  aiProcess_GenSmoothNormals | 
                                  aiProcess_FlipUVs | 
                                  aiProcess_CalcTangentSpace;
        
        // For GLTF files, add additional processing flags
        if (isGltf) {
            std::cout << "Detected GLTF format, applying special processing" << std::endl;
            importFlags |= aiProcess_PreTransformVertices; // Pre-transform vertices
        }
        
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, importFlags);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            throw std::runtime_error("Failed to load model: " + path + " - " + importer.GetErrorString());
        }

        directory = path.substr(0, path.find_last_of('/'));
        std::cout << "Model directory set to: " << directory << std::endl;
        
        // For GLTF files, check for embedded textures
        if (isGltf) {
            std::cout << "Checking for embedded textures in GLTF file..." << std::endl;
            if (scene->HasTextures()) {
                std::cout << "Found " << scene->mNumTextures << " embedded textures in GLTF file" << std::endl;
            } else {
                std::cout << "No embedded textures found in GLTF file" << std::endl;
            }
            
            // Check for materials
            std::cout << "Model has " << scene->mNumMaterials << " materials" << std::endl;
            for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
                aiMaterial* material = scene->mMaterials[i];
                aiString name;
                if (material->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
                    std::cout << "Material " << i << " name: " << name.C_Str() << std::endl;
                }
                
                // Print texture counts by type
                std::cout << "  Diffuse textures: " << material->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;
                std::cout << "  Specular textures: " << material->GetTextureCount(aiTextureType_SPECULAR) << std::endl;
                std::cout << "  Normal textures: " << material->GetTextureCount(aiTextureType_HEIGHT) << std::endl;
            }
        }
        
        std::cout << "Processing model nodes..." << std::endl;
        processNode(scene->mRootNode, scene);
        std::cout << "Model loaded successfully with " << meshes.size() << " meshes" << std::endl;
    }

    void Model::processNode(aiNode *node, const aiScene *scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
    {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        std::cout << "Processing mesh: " << mesh->mName.C_Str() << " with " << mesh->mNumVertices << " vertices" << std::endl;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;

            // Positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // Normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            // Texture coordinates
            if (mesh->mTextureCoords[0]) 
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;

                // Tangents
                if (mesh->HasTangentsAndBitangents()) {
                    vector.x = mesh->mTangents[i].x;
                    vector.y = mesh->mTangents[i].y;
                    vector.z = mesh->mTangents[i].z;
                    vertex.Tangent = vector;

                    // Bitangents
                    vector.x = mesh->mBitangents[i].x;
                    vector.y = mesh->mBitangents[i].y;
                    vector.z = mesh->mBitangents[i].z;
                    vertex.Bitangent = vector;
                } else {
                    // Generate default tangent space if not available
                    vertex.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
                    vertex.Bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
                    std::cout << "Mesh doesn't have tangents/bitangents, using defaults" << std::endl;
                }
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                // Generate default tangent space
                vertex.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
                vertex.Bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }

        // Process material
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            
            // Get material name
            aiString matName;
            if (material->Get(AI_MATKEY_NAME, matName) == AI_SUCCESS) {
                std::cout << "Processing material: " << matName.C_Str() << std::endl;
            }
            
            // Extract material colors if available
            aiColor4D diffuseColor;
            bool hasDiffuseColor = false;
            if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS) {
                std::cout << "Material has diffuse color: " << diffuseColor.r << ", " 
                          << diffuseColor.g << ", " << diffuseColor.b << std::endl;
                hasDiffuseColor = true;
            }
            
            // Load textures
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            
            // If no diffuse textures but has diffuse color, create a texture from the color
            if (diffuseMaps.empty() && hasDiffuseColor) {
                std::cout << "Creating texture from diffuse color" << std::endl;
                Texture colorTexture;
                colorTexture.id = createColorTexture(diffuseColor.r, diffuseColor.g, diffuseColor.b);
                colorTexture.type = "texture_diffuse";
                colorTexture.path = "generated_color";
                textures.push_back(colorTexture);
                textures_loaded.push_back(colorTexture);
            }
            
            // Load other texture types
            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            
            // Try both HEIGHT and NORMALS for normal maps (GLTF often uses NORMALS)
            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            if (normalMaps.empty()) {
                normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
            }
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            
            std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }
        
        std::cout << "Mesh processed with " << textures.size() << " textures" << std::endl;
        return Mesh(vertices, indices, textures);
    }

    vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory, gammaCorrection);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }

    unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
    {
        string filename = string(path);
        
        // Handle both relative and embedded/absolute paths
        if (filename.find('/') == 0 || filename.find(':') != string::npos) {
            // Absolute path or URI - use as is
            std::cout << "Using absolute texture path: " << filename << std::endl;
        } else {
            // Relative path - prepend directory
            filename = directory + '/' + filename;
            std::cout << "Using relative texture path: " << filename << std::endl;
        }

        // Special handling for GLTF embedded textures (data URIs)
        if (filename.find("data:") == 0) {
            std::cout << "Detected embedded texture data URI, creating placeholder texture" << std::endl;
            
            unsigned int textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            
            // Create a simple colored texture as placeholder for embedded textures
            unsigned char embeddedTexture[] = {
                200, 200, 200, 255,  180, 180, 180, 255,
                180, 180, 180, 255,  200, 200, 200, 255
            };
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, embeddedTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            return textureID;
        }
        
        std::cout << "Loading texture: " << filename << std::endl;
        
        // Check if the texture file exists
        std::ifstream fileCheck(filename.c_str());
        if (!fileCheck.good()) {
            std::cout << "ERROR: Texture file does not exist: " << filename << std::endl;
            
            // Try alternative paths for GLTF models
            std::string altPath = directory + "/textures/" + string(path);
            std::ifstream altFileCheck(altPath.c_str());
            if (altFileCheck.good()) {
                std::cout << "Found texture in alternative path: " << altPath << std::endl;
                filename = altPath;
                altFileCheck.close();
            } else {
                // Try bin directory
                std::string binPath = directory + "/bin/" + string(path);
                std::ifstream binFileCheck(binPath.c_str());
                if (binFileCheck.good()) {
                    std::cout << "Found texture in bin path: " << binPath << std::endl;
                    filename = binPath;
                    binFileCheck.close();
                } else {
                    // We'll continue and create a default texture
                    std::cout << "No alternative paths found, using fallback texture" << std::endl;
                }
            }
        } else {
            fileCheck.close();
        }

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            std::cout << "Texture loaded successfully: " << filename << " (" << width << "x" << height 
                      << ", " << nrComponents << " components)" << std::endl;
                      
            GLenum format;
            GLenum internalFormat;
            if (nrComponents == 1) {
                format = GL_RED;
                internalFormat = GL_RED;
            }
            else if (nrComponents == 3) {
                format = GL_RGB;
                internalFormat = gamma ? GL_SRGB : GL_RGB;
            }
            else if (nrComponents == 4) {
                format = GL_RGBA;
                internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
            }
            else {
                cout << "Unsupported number of components: " << nrComponents << " in texture: " << path << endl;
                format = GL_RGB;
                internalFormat = GL_RGB;
            }

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // Adjust texture parameters for better quality
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            // Enable anisotropic filtering if available
            // Note: These constants might not be available in all OpenGL implementations
            // so we'll check if they're defined before using them
#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY
            float aniso = 0.0f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &aniso);
            if (aniso > 0.0f) {
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, aniso);
            }
#endif

            stbi_image_free(data);
        }
        else
        {
            cout << "Texture failed to load at path: " << filename << " - Error: " << stbi_failure_reason() << endl;
            // Create a default texture (checkerboard) to indicate missing texture
            unsigned char checkerboard[] = {
                180, 180, 180, 255,  100, 100, 100, 255,
                100, 100, 100, 255,  180, 180, 180, 255
            };
            
            std::cout << "Created fallback checkerboard texture with ID: " << textureID << std::endl;
            
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerboard);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            if (data) stbi_image_free(data);
        }

        return textureID;
    }

    // Helper function to create a texture from a color
    unsigned int Model::createColorTexture(float r, float g, float b) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        // Create a simple colored texture
        unsigned char colorTexture[4] = {
            static_cast<unsigned char>(r * 255), 
            static_cast<unsigned char>(g * 255), 
            static_cast<unsigned char>(b * 255), 
            255
        };
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        return textureID;
    }
}