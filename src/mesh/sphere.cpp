#include "sphere.h" 

#include <math.h>
#include <math/math.h>
#include "sphere.h" 
namespace Engine
{
    Sphere::Sphere(unsigned int latitudeBands, unsigned int longitudeBands)
    {
        const float radius = 0.5f;

        // Generate vertex positions, UVs, and normals
        for (unsigned int latNumber = 0; latNumber <= latitudeBands; ++latNumber)
        {
            float theta = latNumber * math::PI / latitudeBands;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (unsigned int longNumber = 0; longNumber <= longitudeBands; ++longNumber)
            {
                float phi = longNumber * 2 * math::PI / longitudeBands;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                float x = radius * cosPhi * sinTheta;
                float y = radius * cosTheta;
                float z = radius * sinPhi * sinTheta;

                Positions.push_back(math::vec3(x, y, z));
                UV.push_back(math::vec2((float)longNumber / longitudeBands, (float)latNumber / latitudeBands));
                Normals.push_back(math::normalize(math::vec3(x, y, z))); // Normalize normals
            }
        }

        // Generate indices
        for (unsigned int latNumber = 0; latNumber < latitudeBands; ++latNumber)
        {
            for (unsigned int longNumber = 0; longNumber < longitudeBands; ++longNumber)
            {
                unsigned int first = (latNumber * (longitudeBands + 1)) + longNumber;
                unsigned int second = first + longitudeBands + 1;

                Indices.push_back(first);
                Indices.push_back(second);
                Indices.push_back(first + 1);

                Indices.push_back(second);
                Indices.push_back(second + 1);
                Indices.push_back(first + 1);
            }
        }
        
        Topology = VO::TRIANGLES; // Ensure correct topology for rendering
        Finalize(); // Call finalize to set up the buffers
    }
}

