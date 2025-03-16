#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Mesh.hpp"
#include "Vertex.h"

namespace m3D
{
    unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

    class Model : public VO::VAO 
    {
    public:
        // Model data 
        std::vector<Texture> textures_loaded; // stores all the textures loaded so far
        std::vector<Mesh> meshes;
        std::string directory;
        bool gammaCorrection;

        Model(const std::string &path, bool gamma = false); // Declaration
        void Draw(Shader &shader) override; // Draw function

    private:
        void loadModel(const std::string &path); // Load model function
        void processNode(aiNode *node, const aiScene *scene); // Process node function
        Mesh processMesh(aiMesh *mesh, const aiScene *scene); // Process mesh function
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName); // Load material textures
        unsigned int createColorTexture(float r, float g, float b); // Create a texture from a color
    };
}

#endif // MODEL_H