#include "mesh/cube.h"

namespace Engine {
    // --------------------------------------------------------------------------------------------
    Cube::Cube() {
        Positions = {
            // Front face
            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3(0.5f, -0.5f, -0.5f),
            math::vec3(0.5f, 0.5f, -0.5f),
            math::vec3(0.5f, 0.5f, -0.5f),
            math::vec3(-0.5f, 0.5f, -0.5f),
            math::vec3(-0.5f, -0.5f, -0.5f),

            // Back face
            math::vec3(-0.5f, -0.5f, 0.5f),
            math::vec3(0.5f, -0.5f, 0.5f),
            math::vec3(0.5f, 0.5f, 0.5f),
            math::vec3(0.5f, 0.5f, 0.5f),
            math::vec3(-0.5f, 0.5f, 0.5f),
            math::vec3(-0.5f, -0.5f, 0.5f),

            // Left face
            math::vec3(-0.5f, 0.5f, 0.5f),
            math::vec3(-0.5f, 0.5f, -0.5f),
            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3(-0.5f, -0.5f, 0.5f),
            math::vec3(-0.5f, 0.5f, 0.5f),

            // Right face
            math::vec3(0.5f, 0.5f, 0.5f),
            math::vec3(0.5f, 0.5f, -0.5f),
            math::vec3(0.5f, -0.5f, -0.5f),
            math::vec3(0.5f, -0.5f, -0.5f),
            math::vec3(0.5f, -0.5f, 0.5f),
            math::vec3(0.5f, 0.5f, 0.5f),

            // Bottom face
            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3(0.5f, -0.5f, -0.5f),
            math::vec3(0.5f, -0.5f, 0.5f),
            math::vec3(0.5f, -0.5f, 0.5f),
            math::vec3(-0.5f, -0.5f, 0.5f),
            math::vec3(-0.5f, -0.5f, -0.5f),

            // Top face
            math::vec3(-0.5f, 0.5f, -0.5f),
            math::vec3(0.5f, 0.5f, -0.5f),
            math::vec3(0.5f, 0.5f, 0.5f),
            math::vec3(0.5f, 0.5f, 0.5f),
            math::vec3(-0.5f, 0.5f, 0.5f),
            math::vec3(-0.5f, 0.5f, -0.5f)
        };

        Normals = {
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
            math::vec3(0.0f, 1.0f, 0.0f)
        };


        UV = {
            // Front face
            math::vec2(0.0f, 0.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 0.0f),

            // Back face
            math::vec2(1.0f, 0.0f),
            math::vec2(0.0f, 0.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 0.0f),

            // Left face
            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 0.0f),
            math::vec2(1.0f, 0.0f),

            // Right face
            math::vec2(0.0f, 0.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(0.0f, 0.0f),

            // Bottom face
            math::vec2(0.0f, 0.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 0.0f),

            // Top face
            math::vec2(0.0f, 1.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(0.0f, 0.0f),
            math::vec2(0.0f, 1.0f)
        };
        Colors = {
            // Front face (yellow)
            {1.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},

            // Back face (red)
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},

            // Left face (green)
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},

            // Right face (blue)
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},

            // Top face (white)
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},

            // Bottom face (cyan)
            {0.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 1.0f},
        };

        Topology = VO::TRIANGLES;
        Finalize();
    }
}
