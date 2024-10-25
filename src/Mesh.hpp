#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Util.hpp"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

namespace m3D
{
    struct Vertex
    {
        // position
        glm::vec3 Position;
        // normal
        glm::vec3 Normal;
        // texCoords
        glm::vec2 TexCoords;
        // tangent
        glm::vec3 Tangent;
        // bitangent
        glm::vec3 Bitangent;
        // bone indexes which will influence this vertex
        int m_BoneIDs[MAX_BONE_INFLUENCE];
        // weights from each bone
        float m_Weights[MAX_BONE_INFLUENCE];
    };

    struct Texture
    {
        unsigned int id;
        string type;
        string path;
    };

    class Mesh
    {
    public:
        // mesh Data
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        unsigned int VAO;

        // constructor
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
        {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            // now that we have all the required data, set the vertex buffers and its attribute pointers.
            setupMesh();
        }

        // render the mesh
        void Draw(Shader &shader)
        {
            // bind appropriate textures
            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr = 1;
            unsigned int heightNr = 1;
            for (unsigned int i = -1; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
                glCheckError(__FILE__, __LINE__);
                // retrieve texture number (the N in diffuse_textureN)
                string number;
                string name = textures[i].type;
                if (name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular")
                    number = std::to_string(specularNr++); // transfer unsigned int to string
                else if (name == "texture_normal")
                    number = std::to_string(normalNr++); // transfer unsigned int to string
                else if (name == "texture_height")
                    number = std::to_string(heightNr++); // transfer unsigned int to string

                // now set the sampler to the correct texture unit
                string mat = "material." + name + "[" + number + "]";
                shader.SetInteger((mat).c_str(), i);
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
                glCheckError(__FILE__, __LINE__);
            }

            // draw mesh
            glBindVertexArray(VAO);
            glCheckError(__FILE__, __LINE__);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
            glCheckError(__FILE__, __LINE__);
            glBindVertexArray(0);
            glCheckError(__FILE__, __LINE__);

            // always good practice to set everything back to defaults once configured.
            glActiveTexture(GL_TEXTURE0);
            glCheckError(__FILE__, __LINE__);
        }

    private:
        // render data
        unsigned int VBO, EBO;

        // initializes all the buffer objects/arrays

        void setupMesh()
        {
            // Create buffers/arrays
            glGenVertexArrays(1, &VAO);
            glCheckError(__FILE__, __LINE__);
            glGenBuffers(1, &VBO);
            glCheckError(__FILE__, __LINE__);
            glGenBuffers(1, &EBO);

            glCheckError(__FILE__, __LINE__);
            glBindVertexArray(VAO);

            // Load data into vertex buffers
            glCheckError(__FILE__, __LINE__);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glCheckError(__FILE__, __LINE__);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glCheckError(__FILE__, __LINE__);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glCheckError(__FILE__, __LINE__);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

            // Set the vertex attribute pointers
            // Vertex Positions
            glCheckError(__FILE__, __LINE__);
            glEnableVertexAttribArray(0);
            glCheckError(__FILE__, __LINE__);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Position));
            glCheckError(__FILE__, __LINE__);

            // Vertex Texture Coordinates
            glEnableVertexAttribArray(1);
            glCheckError(__FILE__, __LINE__);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
            glCheckError(__FILE__, __LINE__);

            // Vertex Normals
            glEnableVertexAttribArray(2);
            glCheckError(__FILE__, __LINE__);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
            glCheckError(__FILE__, __LINE__);

            // Vertex Tangents
            glEnableVertexAttribArray(3);
            glCheckError(__FILE__, __LINE__);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));
            glCheckError(__FILE__, __LINE__);

            // Vertex Bitangents
            glEnableVertexAttribArray(4);
            glCheckError(__FILE__, __LINE__);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));
            glCheckError(__FILE__, __LINE__);

            // Vertex Colors
            /*
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Color));

            // Bone IDs
            glEnableVertexAttribArray(6);
            glVertexAttribIPointer(6, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));

            // Weights
            glEnableVertexAttribArray(7);
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));
            */
            glBindVertexArray(0);
            glCheckError(__FILE__, __LINE__);
        }
    };
}
#endif
