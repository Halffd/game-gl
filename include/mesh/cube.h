#ifndef ENGINE_MESH_CUBE_H
#define ENGINE_MESH_CUBE_H

#include "mesh.h" 

namespace Engine
{
    /* 

      A 3D cube in the range [-0.5, 0.5]. Vertices are hard-coded.

    */
    class Cube : public Mesh
    {
    public:
        Cube();
    };
}
#endif