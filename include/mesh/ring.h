#ifndef RING_H
#define RING_H

#include "mesh.h"
#include <vector>

namespace Cell
{
    class Ring : public Mesh
    {
    public:
        Ring(float innerRadius, float outerRadius, unsigned int edgeSegments);

    private:
        void generate();

        float innerRadius;
        float outerRadius;
        unsigned int edgeSegments;
    };
}

#endif // RING_H
