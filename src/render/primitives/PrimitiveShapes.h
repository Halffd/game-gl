#ifndef PRIMITIVE_SHAPES_ALL_H
#define PRIMITIVE_SHAPES_ALL_H

#include "PrimitiveShape.h"
#include "../../scene/Scene.h" // Changed include path
#include <vector>
#include <memory>
#include <glm/glm.hpp>

// Basic shapes
#include "basic/Cube.h"
#include "basic/CartesianPlane.h"

// Curved shapes
#include "curved/Sphere.h"
#include "curved/HighQualitySphere.h"
#include "curved/Cylinder.h"
#include "curved/Cone.h"
#include "curved/Torus.h"
#include "curved/ParametricCurve.h"

// Prisms
#include "prisms/TriangularPrism.h"
#include "prisms/RectangularPrism.h"
#include "prisms/PentagonalPrism.h"
#include "prisms/HexagonalPrism.h"

// Polyhedra
#include "polyhedra/Tetrahedron.h"
#include "polyhedra/Icosphere.h"
#include "polyhedra/Octahedron.h"
#include "polyhedra/Dodecahedron.h"
#include "polyhedra/SquarePyramid.h"

// Dynamic transformations
#include "util/DynamicTransform.h"

namespace m3D {

// Function to generate primitive shapes
void generatePrimitiveShapes(Scene& scene, 
                            std::vector<std::shared_ptr<PrimitiveShape>>& primitiveShapes,
                            std::vector<glm::vec3>& rotationSpeeds);

// Function to generate dynamic shapes with mathematical transformations
void generateDynamicShapes(Scene& scene,
                          std::vector<std::shared_ptr<PrimitiveShape>>& primitiveShapes,
                          std::vector<std::shared_ptr<DynamicTransform>>& dynamicTransforms);

} // namespace m3D

#endif // PRIMITIVE_SHAPES_ALL_H