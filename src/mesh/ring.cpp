#include "ring.h"
#include <cmath>

namespace Cell
{
    Ring::Ring(float innerRadius, float outerRadius, unsigned int edgeSegments)
        : innerRadius(innerRadius), outerRadius(outerRadius), edgeSegments(edgeSegments)
    {
        generate();
        Topology = TRIANGLE_STRIP;
        Cell::Mesh::Finalize();
    }

    void Ring::generate()
    {
         float angleStep = 2.0f * M_PI / edgeSegments;

        // Clear previous data
        Positions.clear();
        Indices.clear();

        // Generate vertices
        for (unsigned int i = 0; i <= edgeSegments; ++i)
        {
            float angle = i * angleStep;
            float xInner = innerRadius * cos(angle);
            float zInner = innerRadius * sin(angle);
            float xOuter = outerRadius * cos(angle);
            float zOuter = outerRadius * sin(angle);

            // Inner and outer vertices
            Positions.push_back(math::vec3(xInner, 0.0f, zInner));
            Positions.push_back(math::vec3(xOuter, 0.0f, zOuter));
        }

        // Generate indices
        for (unsigned int i = 0; i < edgeSegments; ++i)
        {
            unsigned int next = (i + 1) % edgeSegments;

            // Two triangles per segment
            Indices.push_back(i * 2);
            Indices.push_back(next * 2 + 1);
            Indices.push_back(next * 2);

            Indices.push_back(i * 2);
            Indices.push_back(i * 2 + 1);
            Indices.push_back(next * 2 + 1);
        }
    }
}
