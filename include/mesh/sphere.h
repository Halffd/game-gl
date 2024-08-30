#ifndef ENGINE_MESH_SPHERE_H
#define ENGINE_MESH_SPHERE_H

#include "mesh.h"

namespace Engine
{
    /* 

      3D unit sphere charactized by its number of horizontal (xSegments) and vertical (ySegments)
      rings.

    */
    class Sphere : public Mesh
    {
    public:
        Sphere(unsigned int xSegments, unsigned int ySegments);
    };
}
#endif