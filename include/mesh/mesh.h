#ifndef CELL_MESH_H
#define CELL_MESH_H

#include <vector>
#include <functional>
#include <cstdint>

#include "math/vector.h"
#include "vertex.h"

namespace Cell
{
    /* 

      Base Mesh class. A mesh in its simplest form is purely a list of vertices, with some added 
      functionality for easily setting up the hardware configuration relevant for rendering.

    */
    class Mesh : public VAO
    {
    public:
        VBO m_VBO;
        EBO m_EBO;
    public:
        std::vector<math::vec2> UV;
        std::vector<math::vec3> Normals;
        std::vector<math::vec3> Tangents;
        std::vector<math::vec3> Bitangents;

        // support multiple ways of initializing a mesh

        Mesh() = default;

        // Delete copy constructor and assignment operator
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        // Define move constructor and move assignment operator
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        Mesh(std::vector<math::vec3> positions, std::vector<unsigned int> indices);
        Mesh(std::vector<math::vec3> positions, std::vector<math::vec2> uv, std::vector<unsigned int> indices);
        Mesh(std::vector<math::vec3> positions, std::vector<math::vec2> uv, std::vector<math::vec3> normals, std::vector<unsigned int> indices);
        Mesh(std::vector<math::vec3> positions, std::vector<math::vec2> uv, std::vector<math::vec3> normals, std::vector<math::vec3> tangents, std::vector<math::vec3> bitangents, std::vector<unsigned int> indices);

        // set vertex data manually
        void SetPositions(std::vector<math::vec3> positions);
        void SetUVs(std::vector<math::vec2> uv);
        void SetNormals(std::vector<math::vec3> normals);
        void SetTangents(std::vector<math::vec3> tangents, std::vector<math::vec3> bitangents); // NOTE: you can only set both tangents and bitangents at the same time to prevent mismatches

        // commits all buffers and attributes to the GPU driver
        void Finalize(bool interleaved = true);

        // generate triangulated mesh from signed distance field
        void FromSDF(std::function<float(math::vec3)>& sdf, float maxDistance, uint16_t gridResolution);

    private:
        void calculateNormals(bool smooth = true);
        void calculateTangents();
    };
}
#endif
