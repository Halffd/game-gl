#include "mesh/mesh.h"

#include "glad/glad.h"

#include <math/linear_algebra/operation.h>
#include <utility/logging/log.h>
#include <iostream>

namespace Cell
{
     Mesh::Mesh(std::vector<math::vec3> positions, std::vector<math::vec2> uv, std::vector<math::vec3> normals, std::vector<math::vec3> tangents, std::vector<math::vec3> bitangents, std::vector<math::vec3> colors, std::vector<unsigned int> indices)
{
    // Initialize members in the constructor block
    this->Positions = std::move(positions);
    this->UV = std::move(uv);
    this->Normals = std::move(normals);
    this->Tangents = std::move(tangents);
    this->Bitangents = std::move(bitangents);
    this->Colors = std::move(colors);
    this->Indices = std::move(indices);
    
    // Additional constructor logic here, if any
}

      // Move constructor
    Mesh::Mesh(Mesh&& other) noexcept
        : VAO(std::move(other)),  // Move base class resources
          m_VBO(std::move(other.m_VBO)),
          m_EBO(std::move(other.m_EBO)),
          UV(std::move(other.UV)),
          Normals(std::move(other.Normals)),
          Tangents(std::move(other.Tangents)),
          Bitangents(std::move(other.Bitangents))
    {
        // After moving, the source object's resources should be left in a valid but unspecified state
    }

    // Move assignment operator
    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
        if (this != &other)
        {
            // Move base class resources
            VAO::operator=(std::move(other));

            // Move member resources
            m_VBO = std::move(other.m_VBO);
            m_EBO = std::move(other.m_EBO);
            UV = std::move(other.UV);
            Normals = std::move(other.Normals);
            Tangents = std::move(other.Tangents);
            Bitangents = std::move(other.Bitangents);

            // Optionally reset other specific members
        }
        return *this;
    }

    // --------------------------------------------------------------------------------------------
    void Mesh::SetUVs(std::vector<math::vec2> uv)
    {
        UV = uv;
    }
    // --------------------------------------------------------------------------------------------
    void Mesh::SetNormals(std::vector<math::vec3> normals)
    {
        Normals = normals;
    }
    // --------------------------------------------------------------------------------------------
    void Mesh::SetTangents(std::vector<math::vec3> tangents, std::vector<math::vec3> bitangents)
    {
        Tangents = tangents;
        Bitangents = bitangents;
    }
    void Mesh::SetColors(std::vector<math::vec3> color)
    {
        Colors = color;
    }
    // --------------------------------------------------------------------------------------------
    void Mesh::Finalize(bool interleaved)
    {
        if (!exists())
        {
            // Generate and bind the VAO
            genVertexArray();
        }
        bind();
        // Preprocess buffer data as interleaved or separate when specified
        std::vector<float> data;
        if (interleaved)
        {
            for (size_t i = 0; i < Positions.size(); ++i)
            {
                data.push_back(Positions[i].x);
                data.push_back(Positions[i].y);
                data.push_back(Positions[i].z);
                if (UV.size() > 0)
                {
                    data.push_back(UV[i].x);
                    data.push_back(UV[i].y);
                }
                if (Normals.size() > 0)
                {
                    data.push_back(Normals[i].x);
                    data.push_back(Normals[i].y);
                    data.push_back(Normals[i].z);
                }
                if (Tangents.size() > 0)
                {
                    data.push_back(Tangents[i].x);
                    data.push_back(Tangents[i].y);
                    data.push_back(Tangents[i].z);
                }
                if (Bitangents.size() > 0)
                {
                    data.push_back(Bitangents[i].x);
                    data.push_back(Bitangents[i].y);
                    data.push_back(Bitangents[i].z);
                }
                if (Colors.size() > 0)
                {
                    data.push_back(Colors[i].x);
                    data.push_back(Colors[i].y);
                    data.push_back(Colors[i].z);
                }
            }
        }
        else
        {
            for (size_t i = 0; i < Positions.size(); ++i)
            {
                data.push_back(Positions[i].x);
                data.push_back(Positions[i].y);
                data.push_back(Positions[i].z);
            }
            for (size_t i = 0; i < UV.size(); ++i)
            {
                data.push_back(UV[i].x);
                data.push_back(UV[i].y);
            }
            for (size_t i = 0; i < Normals.size(); ++i)
            {
                data.push_back(Normals[i].x);
                data.push_back(Normals[i].y);
                data.push_back(Normals[i].z);
            }
            for (size_t i = 0; i < Tangents.size(); ++i)
            {
                data.push_back(Tangents[i].x);
                data.push_back(Tangents[i].y);
                data.push_back(Tangents[i].z);
            }
            for (size_t i = 0; i < Bitangents.size(); ++i)
            {
                data.push_back(Bitangents[i].x);
                data.push_back(Bitangents[i].y);
                data.push_back(Bitangents[i].z);
            }
            for (size_t i = 0; i < Colors.size(); ++i)
            {
                data.push_back(Colors[i].x);
                data.push_back(Colors[i].y);
                data.push_back(Colors[i].z);
            }
        }

        // Configure vertex attributes (only if vertex data size() > 0)
        m_VBO.genBuffer();
        m_VBO.bind();
        m_VBO.setup(data.data(), data.size() * sizeof(float));

        // Only fill the index buffer if the index array is non-empty
        if (Indices.size() > 0)
        {
            m_EBO.genBuffer();
            m_EBO.bind();
            m_EBO.setup(Indices.data(), Indices.size() * sizeof(unsigned int));
        }

        if (interleaved)
        {
            // Calculate stride from the number of non-empty vertex attribute arrays
            size_t stride = 3 * sizeof(float);
            if (UV.size() > 0)
                stride += 2 * sizeof(float);
            if (Normals.size() > 0)
                stride += 3 * sizeof(float);
            if (Tangents.size() > 0)
                stride += 3 * sizeof(float);
            if (Bitangents.size() > 0)
                stride += 3 * sizeof(float);
            if (Colors.size() > 0)
                stride += 3 * sizeof(float);
            size_t offset = 0;
            setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
            offset += 3 * sizeof(float);
            if (UV.size() > 0)
            {
                setVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
                offset += 2 * sizeof(float);
            }
            if (Normals.size() > 0)
            {
                setVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
                offset += 3 * sizeof(float);
            }
            if (Tangents.size() > 0)
            {
                setVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
                offset += 3 * sizeof(float);
            }
            if (Bitangents.size() > 0)
            {
                setVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
                offset += 3 * sizeof(float);
            }

            if (Colors.size() > 0)
            {
                setVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);
                offset += 3 * sizeof(float);
            }
        }
        else
        {
            size_t offset = 0;
            setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)offset);
            offset += Positions.size() * sizeof(float);
            if (UV.size() > 0)
            {
                setVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)offset);
                offset += UV.size() * sizeof(float);
            }
            if (Normals.size() > 0)
            {
                setVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)offset);
                offset += Normals.size() * sizeof(float);
            }
            if (Tangents.size() > 0)
            {
                setVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)offset);
                offset += Tangents.size() * sizeof(float);
            }
            if (Bitangents.size() > 0)
            {
                setVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)offset);
                offset += Bitangents.size() * sizeof(float);
            }

            if (Colors.size() > 0)
            {
                setVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)offset);
                offset += Colors.size() * sizeof(float);
            }
        }
        unbind();

        vertexCount = Positions.size();
        std::cout << vertexCount << " vertices, Positions: " << Positions << "\nIndices: " << Indices << "\nUV: " << UV << "\n";
    }
    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------
    void Mesh::calculateNormals(bool smooth)
    {
        // TODO(Joey): manually calculate the normals of each vertex
    }
    // --------------------------------------------------------------------------------------------
    void Mesh::calculateTangents()
    {
        // TODO(Joey): walk overall the vertices and calculate the tangent space vectors manually
        // TODO: based on topology, handle some of the winding orders differently.
        // tangents.resize(positions.size());
        // bitangents.resize(positions.size());
        // for (unsigned int i = 0; i < indices.size() - 2; ++i)
        //{
        //    unsigned int index1 = indices[i + 0];
        //    unsigned int index2 = indices[i + 1];
        //    unsigned int index3 = indices[i + 2];
        //    // TODO: do we take different winding order into account for triangle strip?
        //    glm::vec3 pos1 = positions[index1];
        //    glm::vec3 pos2 = positions[index2];
        //    glm::vec3 pos3 = positions[index3];

        //    glm::vec2 uv1 = uv[index1];
        //    glm::vec2 uv2 = uv[index2];
        //    glm::vec2 uv3 = uv[index3];

        //    // due to winding order getting changed each next triangle (as we render as triangle strip) we
        //    // change the order of the cross product to account for winding order switch
        //    glm::vec3 edge1 = pos2 - pos1;
        //    glm::vec3 edge2 = pos3 - pos1;
        //    glm::vec2 deltaUV1 = uv2 - uv1;
        //    glm::vec2 deltaUV2 = uv3 - uv1;

        //    GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        //    glm::vec3 tangent, bitangent;
        //    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        //    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        //    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        //    /*    if (i % 2 == 0)
        //    {*/
        //    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        //    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        //    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        //    //}
        //    /*    else
        //    {
        //    bitangent.x = f * (-deltaUV2.x * edge2.x + deltaUV1.x * edge1.x);
        //    bitangent.y = f * (-deltaUV2.x * edge2.y + deltaUV1.x * edge1.y);
        //    bitangent.z = f * (-deltaUV2.x * edge2.z + deltaUV1.x * edge1.z);
        //    }*/
        //    tangents[index1] += tangent;
        //    tangents[index2] += tangent;
        //    tangents[index3] += tangent;
        //    bitangents[index1] += bitangent;
        //    bitangents[index2] += bitangent;
        //    bitangents[index3] += bitangent;
        //}
        //// normalize all tangents/bi-tangents
        // for (int i = 0; i < tangents.size(); ++i)
        //{
        //     tangents[i] = glm::normalize(tangents[i]);
        //     bitangents[i] = glm::normalize(bitangents[i]);
        // }
    }
}