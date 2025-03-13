#ifndef ARC_H
#define ARC_H

#include <vector>
#include <cmath>
#include "../../include/mesh/mesh.h"

namespace Engine {
    // Create an arc mesh
    Mesh createArc(float radius, float startAngle, float endAngle, int segments);
}

#endif 