#ifndef RING_H
#define RING_H


#include "mesh.h"
#include <cmath>

namespace Engine {

    class Ring : public Mesh {
    public:
        Ring(float innerRadius, float outerRadius, unsigned int edgeSegments, unsigned int ringSegments);

        void generateMesh(float innerRadius, float outerRadius, unsigned int edgeSegments, unsigned int ringSegments);
    };

}

#endif // RING_H
