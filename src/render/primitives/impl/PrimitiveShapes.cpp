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
    
    std::cout << "\nGenerated " << primitiveShapes.size() << " primitive shapes in total\n" << std::endl;
}

} // namespace m3D 