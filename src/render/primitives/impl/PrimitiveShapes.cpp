#include "../PrimitiveShapes.h"
#include <iostream>
#include <random>
#include <string>
#include <stdexcept>

namespace m3D {

// Function to generate primitive shapes
void generatePrimitiveShapes(Scene& scene, 
                            std::vector<std::shared_ptr<PrimitiveShape>>& primitiveShapes,
                            std::vector<glm::vec3>& rotationSpeeds) {
    std::cout << "\n=== Generating primitive shapes ===\n" << std::endl;
    
    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Position distributions
    std::uniform_real_distribution<float> posDistX(-10.0f, 10.0f);
    std::uniform_real_distribution<float> posDistY(-10.0f, 10.0f);
    std::uniform_real_distribution<float> posDistZ(-10.0f, 10.0f);
    
    // Rotation distributions
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
    
    // Scale distributions
    std::uniform_real_distribution<float> scaleDist(0.3f, 0.8f);
    
    // Color distributions
    std::uniform_real_distribution<float> colorDist(0.2f, 1.0f);
    
    // Rotation speed distributions
    std::uniform_real_distribution<float> speedDist(-50.0f, 50.0f);
    
    // Generate cubes
    std::cout << "Generating 5 cubes..." << std::endl;
    for (int i = 0; i < 5; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        std::string name = "Cube_" + std::to_string(i);
        std::cout << "Creating cube " << i << ": " << name << std::endl;
        
        try {
            auto cube = std::make_shared<Cube>(name, position, rotation, glm::vec3(scale), color);
            primitiveShapes.push_back(cube);
            scene.AddObject(cube);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Cube " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating cube " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate spheres
    std::cout << "Generating 5 spheres..." << std::endl;
    for (int i = 0; i < 5; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        // Calculate roundness - vary from 0.5 to 1.0 across the spheres
        float roundness = 0.5f + (float)i / 5.0f * 0.5f;
        
        std::string name = "Sphere_" + std::to_string(i);
        std::cout << "Creating sphere " << i << ": " << name << " with roundness " << roundness << std::endl;
        
        try {
            // Use higher resolution for spheres (48 segments and rings) with varying roundness
            auto sphere = std::make_shared<Sphere>(name, position, rotation, glm::vec3(scale), color, 48, 48, roundness);
            primitiveShapes.push_back(sphere);
            scene.AddObject(sphere);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Sphere " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating sphere " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate high-quality spheres
    std::cout << "Generating 3 high-quality spheres..." << std::endl;
    for (int i = 0; i < 3; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        // Calculate roundness values: 0.7, 0.85, 1.0
        float roundness = 0.7f + (float)i * 0.15f;
        
        std::string name = "HighQualitySphere_" + std::to_string(i);
        std::cout << "Creating high-quality sphere " << i << ": " << name << " with roundness " << roundness << std::endl;
        
        try {
            // Use higher resolution for high-quality spheres (64 segments and rings)
            auto hqSphere = std::make_shared<HighQualitySphere>(name, position, rotation, glm::vec3(scale), color, 64, 64, roundness);
            primitiveShapes.push_back(hqSphere);
            scene.AddObject(hqSphere);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "High-quality sphere " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating high-quality sphere " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate icospheres
    std::cout << "Generating 3 icospheres..." << std::endl;
    for (int i = 0; i < 3; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        // Subdivision levels: 2, 3, 4
        int subdivisions = 2 + i;
        
        std::string name = "Icosphere_" + std::to_string(i);
        std::cout << "Creating icosphere " << i << ": " << name << " with subdivision level " << subdivisions << std::endl;
        
        try {
            auto icosphere = std::make_shared<IcosphereShape>(name, position, rotation, glm::vec3(scale), color, subdivisions);
            primitiveShapes.push_back(icosphere);
            scene.AddObject(icosphere);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Icosphere " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating icosphere " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate triangular prisms
    std::cout << "Generating 3 triangular prisms..." << std::endl;
    for (int i = 0; i < 3; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        std::string name = "TriangularPrism_" + std::to_string(i);
        std::cout << "Creating triangular prism " << i << ": " << name << std::endl;
        
        try {
            auto prism = std::make_shared<TriangularPrism>(name, position, rotation, glm::vec3(scale), color);
            primitiveShapes.push_back(prism);
            scene.AddObject(prism);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Triangular prism " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating triangular prism " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate rectangular prisms
    std::cout << "Generating 3 rectangular prisms..." << std::endl;
    for (int i = 0; i < 3; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        // Create different dimensions for each prism
        glm::vec3 dimensions(1.0f + i * 0.2f, 1.0f, 1.0f + i * 0.3f);
        
        std::string name = "RectangularPrism_" + std::to_string(i);
        std::cout << "Creating rectangular prism " << i << ": " << name << std::endl;
        
        try {
            auto prism = std::make_shared<RectangularPrism>(name, position, rotation, glm::vec3(scale), color, dimensions);
            primitiveShapes.push_back(prism);
            scene.AddObject(prism);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Rectangular prism " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating rectangular prism " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate pentagonal prisms
    std::cout << "Generating 2 pentagonal prisms..." << std::endl;
    for (int i = 0; i < 2; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        std::string name = "PentagonalPrism_" + std::to_string(i);
        std::cout << "Creating pentagonal prism " << i << ": " << name << std::endl;
        
        try {
            auto prism = std::make_shared<PentagonalPrism>(name, position, rotation, glm::vec3(scale), color);
            primitiveShapes.push_back(prism);
            scene.AddObject(prism);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Pentagonal prism " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating pentagonal prism " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate hexagonal prisms
    std::cout << "Generating 2 hexagonal prisms..." << std::endl;
    for (int i = 0; i < 2; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        std::string name = "HexagonalPrism_" + std::to_string(i);
        std::cout << "Creating hexagonal prism " << i << ": " << name << std::endl;
        
        try {
            auto prism = std::make_shared<HexagonalPrism>(name, position, rotation, glm::vec3(scale), color);
            primitiveShapes.push_back(prism);
            scene.AddObject(prism);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Hexagonal prism " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating hexagonal prism " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate cylinders
    std::cout << "Generating 3 cylinders..." << std::endl;
    for (int i = 0; i < 3; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        // Vary the number of segments
        unsigned int segments = 16 + i * 8;
        
        std::string name = "Cylinder_" + std::to_string(i);
        std::cout << "Creating cylinder " << i << ": " << name << " with " << segments << " segments" << std::endl;
        
        try {
            auto cylinder = std::make_shared<Cylinder>(name, position, rotation, glm::vec3(scale), color, segments);
            primitiveShapes.push_back(cylinder);
            scene.AddObject(cylinder);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Cylinder " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating cylinder " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate cones
    std::cout << "Generating 3 cones..." << std::endl;
    for (int i = 0; i < 3; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        // Vary the number of segments
        unsigned int segments = 16 + i * 8;
        
        std::string name = "Cone_" + std::to_string(i);
        std::cout << "Creating cone " << i << ": " << name << " with " << segments << " segments" << std::endl;
        
        try {
            auto cone = std::make_shared<Cone>(name, position, rotation, glm::vec3(scale), color, segments);
            primitiveShapes.push_back(cone);
            scene.AddObject(cone);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Cone " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating cone " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate tetrahedrons
    std::cout << "Generating 3 tetrahedrons..." << std::endl;
    for (int i = 0; i < 3; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        std::string name = "Tetrahedron_" + std::to_string(i);
        std::cout << "Creating tetrahedron " << i << ": " << name << std::endl;
        
        try {
            auto tetrahedron = std::make_shared<Tetrahedron>(name, position, rotation, glm::vec3(scale), color);
            primitiveShapes.push_back(tetrahedron);
            scene.AddObject(tetrahedron);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Tetrahedron " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating tetrahedron " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate octahedrons
    std::cout << "Generating 2 octahedrons..." << std::endl;
    for (int i = 0; i < 2; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        std::string name = "Octahedron_" + std::to_string(i);
        std::cout << "Creating octahedron " << i << ": " << name << std::endl;
        
        try {
            auto octahedron = std::make_shared<Octahedron>(name, position, rotation, glm::vec3(scale), color);
            primitiveShapes.push_back(octahedron);
            scene.AddObject(octahedron);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Octahedron " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating octahedron " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate dodecahedrons
    std::cout << "Generating 2 dodecahedrons..." << std::endl;
    for (int i = 0; i < 2; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        std::string name = "Dodecahedron_" + std::to_string(i);
        std::cout << "Creating dodecahedron " << i << ": " << name << std::endl;
        
        try {
            auto dodecahedron = std::make_shared<Dodecahedron>(name, position, rotation, glm::vec3(scale), color);
            primitiveShapes.push_back(dodecahedron);
            scene.AddObject(dodecahedron);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Dodecahedron " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating dodecahedron " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate square pyramids
    std::cout << "Generating 2 square pyramids..." << std::endl;
    for (int i = 0; i < 2; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        // Vary the height
        float height = 1.0f + i * 0.5f;
        
        std::string name = "SquarePyramid_" + std::to_string(i);
        std::cout << "Creating square pyramid " << i << ": " << name << " with height " << height << std::endl;
        
        try {
            auto pyramid = std::make_shared<SquarePyramid>(name, position, rotation, glm::vec3(scale), color, height);
            primitiveShapes.push_back(pyramid);
            scene.AddObject(pyramid);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Square pyramid " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating square pyramid " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate tori
    std::cout << "Generating 2 tori..." << std::endl;
    for (int i = 0; i < 2; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        // Vary the radii
        float majorRadius = 0.35f + i * 0.05f;
        float minorRadius = 0.15f + i * 0.05f;
        
        std::string name = "Torus_" + std::to_string(i);
        std::cout << "Creating torus " << i << ": " << name << " with major radius " << majorRadius 
                  << " and minor radius " << minorRadius << std::endl;
        
        try {
            auto torus = std::make_shared<Torus>(name, position, rotation, glm::vec3(scale), color, 
                                                majorRadius, minorRadius);
            primitiveShapes.push_back(torus);
            scene.AddObject(torus);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Torus " << i << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating torus " << i << ": " << e.what() << std::endl;
        }
    }
    
    // Generate parametric curves
    std::cout << "Generating 4 parametric curves..." << std::endl;
    
    // Different curve types
    std::vector<std::pair<ParametricCurve::ParametricFunction, std::string>> curveTypes = {
        {&ParametricCurve::helix, "Helix"},
        {&ParametricCurve::trefoilKnot, "TrefoilKnot"},
        {&ParametricCurve::torusKnot, "TorusKnot"},
        {&ParametricCurve::lissajous, "Lissajous"}
    };
    
    for (int i = 0; i < 4; i++) {
        glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
        glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
        float scale = scaleDist(gen);
        glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
        
        auto& [curveFunc, curveTypeName] = curveTypes[i];
        
        std::string name = "ParametricCurve_" + curveTypeName;
        std::cout << "Creating parametric curve: " << name << std::endl;
        
        try {
            auto curve = std::make_shared<ParametricCurve>(name, position, rotation, glm::vec3(scale), color, 
                                                         curveFunc, 0.05f, 100, 8, 0.0f, 6.0f * glm::pi<float>());
            primitiveShapes.push_back(curve);
            scene.AddObject(curve);
            
            // Add random rotation speed
            rotationSpeeds.push_back(glm::vec3(speedDist(gen), speedDist(gen), speedDist(gen)));
            std::cout << "Parametric curve " << name << " created successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error creating parametric curve " << name << ": " << e.what() << std::endl;
        }
    }
    
    std::cout << "\nGenerated " << primitiveShapes.size() << " primitive shapes in total\n" << std::endl;
}

// Function to generate dynamic shapes with mathematical transformations
void generateDynamicShapes(Scene& scene,
                          std::vector<std::shared_ptr<PrimitiveShape>>& primitiveShapes,
                          std::vector<std::shared_ptr<DynamicTransform>>& dynamicTransforms) {
    // Clear existing shapes and dynamic transforms
    primitiveShapes.clear();
    dynamicTransforms.clear();
    
    // Create a fixed CartesianPlane with colored grid (no dynamic transform)
    auto cartesianPlane = std::make_shared<CartesianPlane>(
        "FixedCartesianPlane",
        glm::vec3(0.0f),  // Position at origin
        glm::vec3(0.0f),  // No rotation
        glm::vec3(1.5f),  // Increased scale (1.5x larger)
        30.0f,            // Increased grid magnitude (from 20 to 30)
        0.05f,            // Thicker line width
        false             // Use colored grid (not white)
    );
    
    // Add the CartesianPlane to the scene (but not to dynamic shapes)
    scene.AddObject(cartesianPlane);
    
    // Create a white gridded cube version of CartesianPlane (also fixed)
    auto whiteGridCube = std::make_shared<CartesianPlane>(
        "FixedWhiteGridCube",
        glm::vec3(60.0f, 0.0f, 0.0f),  // Position offset to the right
        glm::vec3(0.0f),               // No rotation
        glm::vec3(1.5f),               // Increased scale (1.5x larger)
        30.0f,                         // Increased grid magnitude (from 20 to 30)
        0.03f,                         // Line width
        true                           // Use white grid
    );
    
    // Add the white grid cube to the scene (but not to dynamic shapes)
    scene.AddObject(whiteGridCube);
    
    // Create other dynamic shapes with different transform modes
    // Random number generation for positions, rotations, colors, etc.
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Position distributions
    std::uniform_real_distribution<float> posDistX(-50.0f, 50.0f);
    std::uniform_real_distribution<float> posDistY(-50.0f, 50.0f);
    std::uniform_real_distribution<float> posDistZ(-50.0f, 50.0f);
    
    // Rotation distribution
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
    
    // Scale distribution
    std::uniform_real_distribution<float> scaleDist(0.5f, 2.0f);
    
    // Color distribution
    std::uniform_real_distribution<float> colorDist(0.2f, 1.0f);
    
    // Quantity distribution
    std::uniform_int_distribution<int> quantityDist(2, 5);
    
    // Transform mode distribution
    std::uniform_int_distribution<int> modeDist(0, 3); // 0=STANDARD, 1=WAVE, 2=ORBIT, 3=SPIRAL
    
    // Shape types and their quantities
    struct ShapeInfo {
        std::string name;
        int quantity;
    };
    
    std::vector<ShapeInfo> shapeTypes = {
        {"Cube", quantityDist(gen)},
        {"Sphere", quantityDist(gen)},
        {"HighQualitySphere", quantityDist(gen)},
        {"Icosphere", quantityDist(gen)},
        {"TriangularPrism", quantityDist(gen)},
        {"RectangularPrism", quantityDist(gen)},
        {"PentagonalPrism", quantityDist(gen)},
        {"HexagonalPrism", quantityDist(gen)},
        {"Cylinder", quantityDist(gen)},
        {"Cone", quantityDist(gen)},
        {"Tetrahedron", quantityDist(gen)},
        {"Octahedron", quantityDist(gen)},
        {"Dodecahedron", quantityDist(gen)},
        {"SquarePyramid", quantityDist(gen)},
        {"Torus", quantityDist(gen)},
        {"ParametricCurve", quantityDist(gen)}
    };
    
    int totalShapes = 0;
    
    // Generate shapes of each type
    for (const auto& shapeInfo : shapeTypes) {
        std::cout << "Generating " << shapeInfo.quantity << " " << shapeInfo.name << " shapes..." << std::endl;
        
        for (int i = 0; i < shapeInfo.quantity; i++) {
            // Generate random base values
            glm::vec3 position(posDistX(gen), posDistY(gen), posDistZ(gen));
            glm::vec3 rotation(rotDist(gen), rotDist(gen), rotDist(gen));
            float scale = scaleDist(gen);
            glm::vec3 color(colorDist(gen), colorDist(gen), colorDist(gen));
            
            // Generate random color gradient
            glm::vec3 colorGradient(colorDist(gen), colorDist(gen), colorDist(gen));
            
            // Create dynamic transform
            auto transform = std::make_shared<DynamicTransform>(
                position, rotation, glm::vec3(scale), color, colorGradient
            );
            
            // Create shape based on type
            std::shared_ptr<PrimitiveShape> shape;
            std::string name = shapeInfo.name + "_Dynamic_" + std::to_string(i);
            
            try {
                if (shapeInfo.name == "Cube") {
                    shape = std::make_shared<Cube>(name, position, rotation, glm::vec3(scale), color);
                }
                else if (shapeInfo.name == "Sphere") {
                    float roundness = 0.7f + 0.3f * colorDist(gen);
                    shape = std::make_shared<Sphere>(name, position, rotation, glm::vec3(scale), color, roundness);
                }
                else if (shapeInfo.name == "HighQualitySphere") {
                    float roundness = 0.7f + 0.3f * colorDist(gen);
                    shape = std::make_shared<HighQualitySphere>(name, position, rotation, glm::vec3(scale), color, roundness);
                }
                else if (shapeInfo.name == "Icosphere") {
                    int subdivisions = 2 + static_cast<int>(colorDist(gen) * 3);
                    shape = std::make_shared<IcosphereShape>(name, position, rotation, glm::vec3(scale), color, subdivisions);
                }
                else if (shapeInfo.name == "TriangularPrism") {
                    shape = std::make_shared<TriangularPrism>(name, position, rotation, glm::vec3(scale), color);
                }
                else if (shapeInfo.name == "RectangularPrism") {
                    glm::vec3 dimensions(0.5f + colorDist(gen), 0.5f + colorDist(gen), 0.5f + colorDist(gen));
                    shape = std::make_shared<RectangularPrism>(name, position, rotation, glm::vec3(scale), color, dimensions);
                }
                else if (shapeInfo.name == "PentagonalPrism") {
                    shape = std::make_shared<PentagonalPrism>(name, position, rotation, glm::vec3(scale), color);
                }
                else if (shapeInfo.name == "HexagonalPrism") {
                    shape = std::make_shared<HexagonalPrism>(name, position, rotation, glm::vec3(scale), color);
                }
                else if (shapeInfo.name == "Cylinder") {
                    unsigned int segments = 16 + static_cast<int>(colorDist(gen) * 16);
                    shape = std::make_shared<Cylinder>(name, position, rotation, glm::vec3(scale), color, segments);
                }
                else if (shapeInfo.name == "Cone") {
                    unsigned int segments = 16 + static_cast<int>(colorDist(gen) * 16);
                    shape = std::make_shared<Cone>(name, position, rotation, glm::vec3(scale), color, segments);
                }
                else if (shapeInfo.name == "Tetrahedron") {
                    shape = std::make_shared<Tetrahedron>(name, position, rotation, glm::vec3(scale), color);
                }
                else if (shapeInfo.name == "Octahedron") {
                    shape = std::make_shared<Octahedron>(name, position, rotation, glm::vec3(scale), color);
                }
                else if (shapeInfo.name == "Dodecahedron") {
                    shape = std::make_shared<Dodecahedron>(name, position, rotation, glm::vec3(scale), color);
                }
                else if (shapeInfo.name == "SquarePyramid") {
                    float height = 1.0f + colorDist(gen);
                    shape = std::make_shared<SquarePyramid>(name, position, rotation, glm::vec3(scale), color, height);
                }
                else if (shapeInfo.name == "Torus") {
                    float majorRadius = 0.3f + 0.2f * colorDist(gen);
                    float minorRadius = 0.1f + 0.1f * colorDist(gen);
                    shape = std::make_shared<Torus>(name, position, rotation, glm::vec3(scale), color, majorRadius, minorRadius);
                }
                else if (shapeInfo.name == "ParametricCurve") {
                    // Choose a random parametric function
                    std::vector<std::pair<ParametricCurve::ParametricFunction, std::string>> curveFunctions = {
                        {&ParametricCurve::helix, "Helix"},
                        {&ParametricCurve::trefoilKnot, "TrefoilKnot"},
                        {&ParametricCurve::torusKnot, "TorusKnot"},
                        {&ParametricCurve::lissajous, "Lissajous"}
                    };
                    
                    int funcIndex = static_cast<int>(colorDist(gen) * curveFunctions.size()) % curveFunctions.size();
                    auto& [curveFunc, curveTypeName] = curveFunctions[funcIndex];
                    
                    float thickness = 0.03f + 0.04f * colorDist(gen);
                    unsigned int segments = 50 + static_cast<int>(colorDist(gen) * 50);
                    
                    shape = std::make_shared<ParametricCurve>(
                        name + "_" + curveTypeName, position, rotation, glm::vec3(scale), color,
                        curveFunc, thickness, segments, 8, 0.0f, 6.0f * glm::pi<float>()
                    );
                }
                
                if (shape) {
                    primitiveShapes.push_back(shape);
                    scene.AddObject(shape);
                    dynamicTransforms.push_back(transform);
                    std::cout << "Created " << name << " successfully" << std::endl;
                    totalShapes++;
                }
            } catch (const std::exception& e) {
                std::cout << "Error creating " << name << ": " << e.what() << std::endl;
            }
        }
    }
    
    std::cout << "\nGenerated " << totalShapes << " dynamic shapes with mathematical transformations\n" << std::endl;
}

} // namespace m3D 