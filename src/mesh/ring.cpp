#include "ring.h"
#include <cmath>
#include <algorithm> // For std::clamp

namespace Cell {

    Ring::Ring(float innerRadius, float outerRadius, unsigned int edgeSegments, unsigned int ringSegments) {
        generateMesh(innerRadius, outerRadius, edgeSegments, ringSegments);
    }

    void Ring::generateMesh(float innerRadius, float outerRadius, unsigned int edgeSegments, unsigned int ringSegments) {
        const float TAU = 6.28318530717958647692f; // 2 * PI

        Positions.clear();
        UV.clear();
        Indices.clear();

        // Generate vertices and UV for two circles (inner and outer)
        for (unsigned int y = 0; y <= ringSegments; ++y) {
            float ringDepth = static_cast<float>(y) / static_cast<float>(ringSegments);
            float radius = innerRadius + (outerRadius - innerRadius) * ringDepth;

            for (unsigned int x = 0; x <= edgeSegments; ++x) {
                float theta = static_cast<float>(x) / static_cast<float>(edgeSegments) * TAU;
                float xPos = std::cos(theta);
                float yPos = std::sin(theta);

                Positions.push_back(math::vec3(radius * xPos, radius * yPos, 0.0f));
                UV.push_back(math::vec2(static_cast<float>(x) / edgeSegments, ringDepth));
            }
        }

        // Generate Indices for the ring
        bool oddRow = false;
        for (unsigned int y = 0; y < ringSegments; ++y) {
            if (!oddRow) {
                for (unsigned int x = 0; x <= edgeSegments; ++x) {
                    unsigned int current = y * (edgeSegments + 1) + x;
                    unsigned int next = (y + 1) * (edgeSegments + 1) + x;

                    Indices.push_back(current);
                    Indices.push_back(next);
                }
            } else {
                for (int x = edgeSegments; x >= 0; --x) {
                    unsigned int current = (y + 1) * (edgeSegments + 1) + x;
                    unsigned int next = y * (edgeSegments + 1) + x;

                    Indices.push_back(current);
                    Indices.push_back(next);
                }
            }
            oddRow = !oddRow;
        }

        Topology = TRIANGLE_STRIP;
        Cell::Mesh::Finalize();
    }

}
