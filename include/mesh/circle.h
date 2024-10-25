#ifndef ENGINE_MESH_CIRCLE_H
#define ENGINE_MESH_CIRCLE_H

#include "mesh.h"

namespace Engine
{
    /* 

      2D unit circle mesh characterized by its number of edge segments (outer edges) and the number
      of inner ring segments.

    */
    class Circle : public Mesh
    {
    public:
        Circle(unsigned int edgeSegments, unsigned int ringSegments);
    };
}
#endif