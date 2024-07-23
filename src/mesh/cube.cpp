#include "cube.h"

namespace Cell
{
    // --------------------------------------------------------------------------------------------
    Cube::Cube()
    {
        Positions = std::vector<math::vec3>{
            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3(0.5f, 0.5f, -0.5f),
            math::vec3(0.5f, -0.5f, -0.5f),
            math::vec3(0.5f, 0.5f, -0.5f),
            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3(-0.5f, 0.5f, -0.5f),

            math::vec3(-0.5f, -0.5f, 0.5f),
            math::vec3(0.5f, -0.5f, 0.5f),
            math::vec3(0.5f, 0.5f, 0.5f),
            math::vec3(0.5f, 0.5f, 0.5f),
            math::vec3(-0.5f, 0.5f, 0.5f),
            math::vec3(-0.5f, -0.5f, 0.5f),

            math::vec3(-0.5f, 0.5f, 0.5f),
            math::vec3(-0.5f, 0.5f, -0.5f),
            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3(-0.5f, -0.5f, 0.5f),
            math::vec3(-0.5f, 0.5f, 0.5f),

            math::vec3(0.5f, 0.5f, 0.5f),
            math::vec3(0.5f, -0.5f, -0.5f),
            math::vec3(0.5f, 0.5f, -0.5f),
            math::vec3(0.5f, -0.5f, -0.5f),
            math::vec3(0.5f, 0.5f, 0.5f),
            math::vec3(0.5f, -0.5f, 0.5f),

            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3(0.5f, -0.5f, -0.5f),
            math::vec3(0.5f, -0.5f, 0.5f),
            math::vec3(0.5f, -0.5f, 0.5f),
            math::vec3(-0.5f, -0.5f, 0.5f),
            math::vec3(-0.5f, -0.5f, -0.5f),

            math::vec3(-0.5f, 0.5f, -0.5f),
            math::vec3(0.5f, 0.5f, 0.5f),
            math::vec3(0.5f, 0.5f, -0.5f),
            math::vec3(0.5f, 0.5f, 0.5f),
            math::vec3(-0.5f, 0.5f, -0.5f),
            math::vec3(-0.5f, 0.5f, 0.5f),
        };
        UV = std::vector<math::vec2>{
            math::vec2(0.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(0.0f, 0.0f),
            math::vec2(0.0f, 1.0f),

            math::vec2(0.0f, 0.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 0.0f),

            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 0.0f),
            math::vec2(1.0f, 0.0f),

            math::vec2(1.0f, 0.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(0.0f, 0.0f),

            math::vec2(0.0f, 1.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(0.0f, 0.0f),
            math::vec2(0.0f, 1.0f),

            math::vec2(0.0f, 1.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 0.0f),
        };
        Normals = std::vector<math::vec3>{
            math::vec3(0.0f, 0.0f, -1.0f),
            math::vec3(0.0f, 0.0f, -1.0f),
            math::vec3(0.0f, 0.0f, -1.0f),
            math::vec3(0.0f, 0.0f, -1.0f),
            math::vec3(0.0f, 0.0f, -1.0f),
            math::vec3(0.0f, 0.0f, -1.0f),

            math::vec3(0.0f, 0.0f, 1.0f),
            math::vec3(0.0f, 0.0f, 1.0f),
            math::vec3(0.0f, 0.0f, 1.0f),
            math::vec3(0.0f, 0.0f, 1.0f),
            math::vec3(0.0f, 0.0f, 1.0f),
            math::vec3(0.0f, 0.0f, 1.0f),

            math::vec3(-1.0f, 0.0f, 0.0f),
            math::vec3(-1.0f, 0.0f, 0.0f),
            math::vec3(-1.0f, 0.0f, 0.0f),
            math::vec3(-1.0f, 0.0f, 0.0f),
            math::vec3(-1.0f, 0.0f, 0.0f),
            math::vec3(-1.0f, 0.0f, 0.0f),

            math::vec3(1.0f, 0.0f, 0.0f),
            math::vec3(1.0f, 0.0f, 0.0f),
            math::vec3(1.0f, 0.0f, 0.0f),
            math::vec3(1.0f, 0.0f, 0.0f),
            math::vec3(1.0f, 0.0f, 0.0f),
            math::vec3(1.0f, 0.0f, 0.0f),

            math::vec3(0.0f, -1.0f, 0.0f),
            math::vec3(0.0f, -1.0f, 0.0f),
            math::vec3(0.0f, -1.0f, 0.0f),
            math::vec3(0.0f, -1.0f, 0.0f),
            math::vec3(0.0f, -1.0f, 0.0f),
            math::vec3(0.0f, -1.0f, 0.0f),

            math::vec3(0.0f, 1.0f, 0.0f),
            math::vec3(0.0f, 1.0f, 0.0f),
            math::vec3(0.0f, 1.0f, 0.0f),
            math::vec3(0.0f, 1.0f, 0.0f),
            math::vec3(0.0f, 1.0f, 0.0f),
            math::vec3(0.0f, 1.0f, 0.0f),
        };
        Colors = {
            // Front face (yellow)
            {1.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            // Back face (red)
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            // Left face (green)
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            // Right face (blue)
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            // Top face (white)
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            // Bottom face (cyan)
            {0.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 1.0f},
        };

        Topology = TRIANGLES;
        Finalize();
    }
}