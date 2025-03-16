#ifndef CARTESIAN_PLANE_SHAPE_H
#define CARTESIAN_PLANE_SHAPE_H

#include "../PrimitiveShape.h"
#include <random>
#include <cmath>

namespace m3D {

// CartesianPlane shape - renders a 3D coordinate system with colored grid lines
class CartesianPlane : public PrimitiveShape {
public:
    CartesianPlane(const std::string& name, 
                  const glm::vec3& position = glm::vec3(0.0f),
                  const glm::vec3& rotation = glm::vec3(0.0f),
                  const glm::vec3& scale = glm::vec3(1.0f),
                  float magnitude = 50.0f,
                  float lineWidth = 0.02f,
                  bool useWhiteGrid = false) 
        : PrimitiveShape(name, position, rotation, scale, glm::vec3(1.0f)) {
        
        std::cout << "Creating CartesianPlane: " << name << " at position (" 
                  << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Random color generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> colorDist(0.2f, 1.0f);
        
        // Create origin axes (much thicker lines)
        float axisWidth = lineWidth * 8.0f; // Increased thickness for main axes
        
        // Define axis colors
        glm::vec3 xAxisColor = useWhiteGrid ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(1.0f, 0.2f, 0.2f); // Red
        glm::vec3 yAxisColor = useWhiteGrid ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.2f, 1.0f, 0.2f); // Green
        glm::vec3 zAxisColor = useWhiteGrid ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.2f, 0.2f, 1.0f); // Blue
        
        // X-axis (red)
        createLine(vertices, indices, glm::vec3(-magnitude, 0, 0), glm::vec3(magnitude, 0, 0), 
                  xAxisColor, axisWidth);
        
        // Y-axis (green)
        createLine(vertices, indices, glm::vec3(0, -magnitude, 0), glm::vec3(0, magnitude, 0), 
                  yAxisColor, axisWidth);
        
        // Z-axis (blue)
        createLine(vertices, indices, glm::vec3(0, 0, -magnitude), glm::vec3(0, 0, magnitude), 
                  zAxisColor, axisWidth);
        
        // Add number markers along the axes
        for (int i = -static_cast<int>(magnitude); i <= static_cast<int>(magnitude); i++) {
            // Skip 0
            if (i == 0) continue;
            
            // Only add markers at intervals
            if (i % 5 == 0) {
                float markerSize = axisWidth * 2.0f;
                
                // X-axis markers
                createNumberMarker(vertices, indices, i, glm::vec3(i, 0, 0), xAxisColor, markerSize);
                
                // Y-axis markers
                createNumberMarker(vertices, indices, i, glm::vec3(0, i, 0), yAxisColor, markerSize);
                
                // Z-axis markers
                createNumberMarker(vertices, indices, i, glm::vec3(0, 0, i), zAxisColor, markerSize);
            }
        }
        
        // Create grid lines
        int gridSize = static_cast<int>(magnitude);
        float gridLineWidth = useWhiteGrid ? lineWidth * 2.0f : lineWidth; // Thicker grid lines for white grid
        
        for (int i = -gridSize; i <= gridSize; i++) {
            // Skip the origin lines (already created)
            if (i == 0) continue;
            
            // Generate color for grid line
            glm::vec3 color;
            if (useWhiteGrid) {
                // White grid with varying brightness based on distance from origin
                float brightness = 1.0f - (std::abs(i) / static_cast<float>(gridSize)) * 0.5f;
                color = glm::vec3(brightness);
            } else {
                // Rainbow colored grid
                color = generateUniqueColor(i);
            }
            
            // Make grid lines more visible at regular intervals
            bool isMainGridLine = (i % 5 == 0);
            float currentLineWidth = isMainGridLine ? gridLineWidth * 1.5f : gridLineWidth;
            
            // X-Z plane grid lines (horizontal)
            createLine(vertices, indices, 
                      glm::vec3(-magnitude, 0, i), 
                      glm::vec3(magnitude, 0, i), 
                      color, currentLineWidth);
            
            // X-Y plane grid lines (vertical)
            createLine(vertices, indices, 
                      glm::vec3(-magnitude, i, 0), 
                      glm::vec3(magnitude, i, 0), 
                      color, currentLineWidth);
            
            // Y-Z plane grid lines
            createLine(vertices, indices, 
                      glm::vec3(i, -magnitude, 0), 
                      glm::vec3(i, magnitude, 0), 
                      color, currentLineWidth);
            
            // X-Z plane grid lines (depth)
            createLine(vertices, indices, 
                      glm::vec3(i, 0, -magnitude), 
                      glm::vec3(i, 0, magnitude), 
                      color, currentLineWidth);
            
            // Y-Z plane grid lines (depth)
            createLine(vertices, indices, 
                      glm::vec3(0, -magnitude, i), 
                      glm::vec3(0, magnitude, i), 
                      color, currentLineWidth);
            
            // X-Y plane grid lines (depth)
            createLine(vertices, indices, 
                      glm::vec3(0, i, -magnitude), 
                      glm::vec3(0, i, magnitude), 
                      color, currentLineWidth);
        }
        
        // If using white grid, create a cube outline at the edges
        if (useWhiteGrid) {
            float cubeSize = magnitude * 0.8f; // Slightly smaller than the full grid
            float cubeLineWidth = lineWidth * 3.0f; // Thicker lines for the cube outline
            glm::vec3 cubeColor(1.0f, 1.0f, 1.0f); // White color for the cube
            
            // Create the 12 edges of the cube
            // Bottom face
            createLine(vertices, indices, 
                      glm::vec3(-cubeSize, -cubeSize, -cubeSize), 
                      glm::vec3(cubeSize, -cubeSize, -cubeSize), 
                      cubeColor, cubeLineWidth);
            
            createLine(vertices, indices, 
                      glm::vec3(cubeSize, -cubeSize, -cubeSize), 
                      glm::vec3(cubeSize, -cubeSize, cubeSize), 
                      cubeColor, cubeLineWidth);
            
            createLine(vertices, indices, 
                      glm::vec3(cubeSize, -cubeSize, cubeSize), 
                      glm::vec3(-cubeSize, -cubeSize, cubeSize), 
                      cubeColor, cubeLineWidth);
            
            createLine(vertices, indices, 
                      glm::vec3(-cubeSize, -cubeSize, cubeSize), 
                      glm::vec3(-cubeSize, -cubeSize, -cubeSize), 
                      cubeColor, cubeLineWidth);
            
            // Top face
            createLine(vertices, indices, 
                      glm::vec3(-cubeSize, cubeSize, -cubeSize), 
                      glm::vec3(cubeSize, cubeSize, -cubeSize), 
                      cubeColor, cubeLineWidth);
            
            createLine(vertices, indices, 
                      glm::vec3(cubeSize, cubeSize, -cubeSize), 
                      glm::vec3(cubeSize, cubeSize, cubeSize), 
                      cubeColor, cubeLineWidth);
            
            createLine(vertices, indices, 
                      glm::vec3(cubeSize, cubeSize, cubeSize), 
                      glm::vec3(-cubeSize, cubeSize, cubeSize), 
                      cubeColor, cubeLineWidth);
            
            createLine(vertices, indices, 
                      glm::vec3(-cubeSize, cubeSize, cubeSize), 
                      glm::vec3(-cubeSize, cubeSize, -cubeSize), 
                      cubeColor, cubeLineWidth);
            
            // Vertical edges
            createLine(vertices, indices, 
                      glm::vec3(-cubeSize, -cubeSize, -cubeSize), 
                      glm::vec3(-cubeSize, cubeSize, -cubeSize), 
                      cubeColor, cubeLineWidth);
            
            createLine(vertices, indices, 
                      glm::vec3(cubeSize, -cubeSize, -cubeSize), 
                      glm::vec3(cubeSize, cubeSize, -cubeSize), 
                      cubeColor, cubeLineWidth);
            
            createLine(vertices, indices, 
                      glm::vec3(cubeSize, -cubeSize, cubeSize), 
                      glm::vec3(cubeSize, cubeSize, cubeSize), 
                      cubeColor, cubeLineWidth);
            
            createLine(vertices, indices, 
                      glm::vec3(-cubeSize, -cubeSize, cubeSize), 
                      glm::vec3(-cubeSize, cubeSize, cubeSize), 
                      cubeColor, cubeLineWidth);
        }
        
        // Create a texture with the specified color
        std::vector<Texture> textures = createColorTextures();
        
        std::cout << "Creating cartesian plane mesh with " << vertices.size() << " vertices and " 
                  << indices.size() << " indices" << std::endl;
        
        // Create the mesh
        mesh = std::make_shared<Mesh>(vertices, indices, textures);
        std::cout << "Cartesian plane mesh created successfully" << std::endl;
    }
    
private:
    // Helper method to create a line segment
    void createLine(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                   const glm::vec3& start, const glm::vec3& end, 
                   const glm::vec3& color, float width) {
        
        // Calculate direction vector
        glm::vec3 direction = glm::normalize(end - start);
        
        // Create a perpendicular vector
        glm::vec3 perpendicular;
        if (std::abs(direction.y) < 0.999f) {
            perpendicular = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
        } else {
            perpendicular = glm::normalize(glm::cross(direction, glm::vec3(1.0f, 0.0f, 0.0f)));
        }
        
        // Create another perpendicular vector
        glm::vec3 perpendicular2 = glm::normalize(glm::cross(direction, perpendicular));
        
        // Calculate the four corners of the line segment (start)
        glm::vec3 startCorner1 = start + width * perpendicular;
        glm::vec3 startCorner2 = start - width * perpendicular;
        glm::vec3 startCorner3 = start + width * perpendicular2;
        glm::vec3 startCorner4 = start - width * perpendicular2;
        
        // Calculate the four corners of the line segment (end)
        glm::vec3 endCorner1 = end + width * perpendicular;
        glm::vec3 endCorner2 = end - width * perpendicular;
        glm::vec3 endCorner3 = end + width * perpendicular2;
        glm::vec3 endCorner4 = end - width * perpendicular2;
        
        // Get the current vertex count
        unsigned int baseIndex = vertices.size();
        
        // Add vertices for the line segment
        // First face (perpendicular)
        addVertex(vertices, startCorner1, perpendicular, glm::vec2(0.0f, 0.0f), color);
        addVertex(vertices, startCorner2, perpendicular, glm::vec2(1.0f, 0.0f), color);
        addVertex(vertices, endCorner2, perpendicular, glm::vec2(1.0f, 1.0f), color);
        addVertex(vertices, endCorner1, perpendicular, glm::vec2(0.0f, 1.0f), color);
        
        // Second face (perpendicular2)
        addVertex(vertices, startCorner3, perpendicular2, glm::vec2(0.0f, 0.0f), color);
        addVertex(vertices, startCorner4, perpendicular2, glm::vec2(1.0f, 0.0f), color);
        addVertex(vertices, endCorner4, perpendicular2, glm::vec2(1.0f, 1.0f), color);
        addVertex(vertices, endCorner3, perpendicular2, glm::vec2(0.0f, 1.0f), color);
        
        // Add indices for the line segment (two triangles per face)
        // First face
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
        
        // Second face
        indices.push_back(baseIndex + 4);
        indices.push_back(baseIndex + 5);
        indices.push_back(baseIndex + 6);
        
        indices.push_back(baseIndex + 4);
        indices.push_back(baseIndex + 6);
        indices.push_back(baseIndex + 7);
    }
    
    // Helper method to create a number marker (using 3D geometry)
    void createNumberMarker(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                           int number, const glm::vec3& position, const glm::vec3& color, float size) {
        // Determine which axis this marker is on
        bool isXAxis = (position.y == 0 && position.z == 0);
        bool isYAxis = (position.x == 0 && position.z == 0);
        bool isZAxis = (position.x == 0 && position.y == 0);
        
        // Create a cube marker at the position
        createCubeMarker(vertices, indices, position, color, size * 0.5f);
        
        // Create number segments based on the absolute value of the number
        int absNumber = std::abs(number);
        
        // Offset for the number display
        glm::vec3 offset;
        glm::vec3 digitDirection1, digitDirection2;
        
        if (isXAxis) {
            // X-axis: display number above the axis
            offset = glm::vec3(0.0f, size * 2.0f, 0.0f);
            digitDirection1 = glm::vec3(0.0f, size * 1.5f, 0.0f);
            digitDirection2 = glm::vec3(0.0f, 0.0f, size * 1.5f);
        } else if (isYAxis) {
            // Y-axis: display number to the right of the axis
            offset = glm::vec3(size * 2.0f, 0.0f, 0.0f);
            digitDirection1 = glm::vec3(size * 1.5f, 0.0f, 0.0f);
            digitDirection2 = glm::vec3(0.0f, 0.0f, size * 1.5f);
        } else if (isZAxis) {
            // Z-axis: display number above and to the right of the axis
            offset = glm::vec3(size * 2.0f, size * 2.0f, 0.0f);
            digitDirection1 = glm::vec3(size * 1.5f, 0.0f, 0.0f);
            digitDirection2 = glm::vec3(0.0f, size * 1.5f, 0.0f);
        }
        
        // Display negative sign if needed
        if (number < 0) {
            createDigitSegment(vertices, indices, position + offset, color, size, 
                              digitDirection1, digitDirection2, -1);
            offset += digitDirection1 * 1.5f; // Move to the right for the digit
        }
        
        // Convert number to digits and display each digit
        std::vector<int> digits;
        do {
            digits.push_back(absNumber % 10);
            absNumber /= 10;
        } while (absNumber > 0);
        
        // Display digits in reverse order (most significant first)
        for (int i = digits.size() - 1; i >= 0; i--) {
            createDigitSegment(vertices, indices, position + offset, color, size, 
                              digitDirection1, digitDirection2, digits[i]);
            offset += digitDirection1 * 1.5f; // Move to the right for the next digit
        }
    }
    
    // Helper method to create a cube marker
    void createCubeMarker(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                         const glm::vec3& position, const glm::vec3& color, float size) {
        // Define the 8 vertices of a cube
        glm::vec3 v1 = position + glm::vec3(-size, -size, -size);
        glm::vec3 v2 = position + glm::vec3(size, -size, -size);
        glm::vec3 v3 = position + glm::vec3(size, size, -size);
        glm::vec3 v4 = position + glm::vec3(-size, size, -size);
        glm::vec3 v5 = position + glm::vec3(-size, -size, size);
        glm::vec3 v6 = position + glm::vec3(size, -size, size);
        glm::vec3 v7 = position + glm::vec3(size, size, size);
        glm::vec3 v8 = position + glm::vec3(-size, size, size);
        
        // Get the current vertex count
        unsigned int baseIndex = vertices.size();
        
        // Add vertices for the cube
        // Front face
        addVertex(vertices, v1, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), color);
        addVertex(vertices, v2, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), color);
        addVertex(vertices, v3, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), color);
        addVertex(vertices, v4, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), color);
        
        // Back face
        addVertex(vertices, v5, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), color);
        addVertex(vertices, v6, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f), color);
        addVertex(vertices, v7, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f), color);
        addVertex(vertices, v8, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f), color);
        
        // Left face
        addVertex(vertices, v1, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), color);
        addVertex(vertices, v4, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), color);
        addVertex(vertices, v8, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), color);
        addVertex(vertices, v5, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), color);
        
        // Right face
        addVertex(vertices, v2, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), color);
        addVertex(vertices, v6, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), color);
        addVertex(vertices, v7, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), color);
        addVertex(vertices, v3, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), color);
        
        // Top face
        addVertex(vertices, v4, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), color);
        addVertex(vertices, v3, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), color);
        addVertex(vertices, v7, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), color);
        addVertex(vertices, v8, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), color);
        
        // Bottom face
        addVertex(vertices, v1, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), color);
        addVertex(vertices, v5, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), color);
        addVertex(vertices, v6, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f), color);
        addVertex(vertices, v2, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f), color);
        
        // Add indices for the cube (two triangles per face)
        for (int i = 0; i < 6; i++) {
            unsigned int faceBaseIndex = baseIndex + i * 4;
            indices.push_back(faceBaseIndex);
            indices.push_back(faceBaseIndex + 1);
            indices.push_back(faceBaseIndex + 2);
            
            indices.push_back(faceBaseIndex);
            indices.push_back(faceBaseIndex + 2);
            indices.push_back(faceBaseIndex + 3);
        }
    }
    
    // Helper method to create a digit segment
    void createDigitSegment(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                           const glm::vec3& position, const glm::vec3& color, float size,
                           const glm::vec3& direction1, const glm::vec3& direction2, int digit) {
        // Define the segments for each digit (0-9) and negative sign (-1)
        // Each digit is composed of up to 7 segments arranged in a figure-8 pattern
        // Segments are: 0=top, 1=top-right, 2=bottom-right, 3=bottom, 4=bottom-left, 5=top-left, 6=middle
        bool segments[10][7] = {
            {true, true, true, true, true, true, false},     // 0
            {false, true, true, false, false, false, false}, // 1
            {true, true, false, true, true, false, true},    // 2
            {true, true, true, true, false, false, true},    // 3
            {false, true, true, false, false, true, true},   // 4
            {true, false, true, true, false, true, true},    // 5
            {true, false, true, true, true, true, true},     // 6
            {true, true, true, false, false, false, false},  // 7
            {true, true, true, true, true, true, true},      // 8
            {true, true, true, true, false, true, true}      // 9
        };
        
        // Special case for negative sign
        if (digit == -1) {
            // Create just the middle segment
            glm::vec3 start = position;
            glm::vec3 end = position + direction1;
            createLine(vertices, indices, start, end, color, size * 0.2f);
            return;
        }
        
        // Ensure digit is in range
        if (digit < 0 || digit > 9) return;
        
        // Create each active segment for the digit
        float segmentWidth = size * 0.2f;
        float segmentLength = size;
        
        // Define segment positions relative to the digit position
        glm::vec3 segmentPositions[7] = {
            position + direction2,                                // 0: top
            position + direction2 + direction1,                   // 1: top-right
            position + direction1,                                // 2: bottom-right
            position,                                             // 3: bottom
            position - direction1,                                // 4: bottom-left
            position + direction2 - direction1,                   // 5: top-left
            position + direction2 * 0.5f                          // 6: middle
        };
        
        glm::vec3 segmentDirections[7] = {
            direction1,                                           // 0: top (horizontal)
            direction2 * -1.0f,                                   // 1: top-right (vertical)
            direction2 * -1.0f,                                   // 2: bottom-right (vertical)
            direction1 * -1.0f,                                   // 3: bottom (horizontal)
            direction2,                                           // 4: bottom-left (vertical)
            direction2,                                           // 5: top-left (vertical)
            direction1                                            // 6: middle (horizontal)
        };
        
        // Create each active segment
        for (int i = 0; i < 7; i++) {
            if (segments[digit][i]) {
                glm::vec3 start = segmentPositions[i];
                glm::vec3 end = segmentPositions[i] + segmentDirections[i];
                createLine(vertices, indices, start, end, color, segmentWidth);
            }
        }
    }
    
    // Helper method to add a vertex with a specific color
    void addVertex(std::vector<Vertex>& vertices, const glm::vec3& position, 
                  const glm::vec3& normal, const glm::vec2& texCoords, 
                  const glm::vec3& vertexColor) {
        Vertex vertex;
        vertex.Position = position;
        vertex.Normal = normal;
        vertex.TexCoords = texCoords;
        // We can't set vertex.Color directly as it doesn't exist in the Vertex struct
        // Instead, we'll use the color when creating the texture
        
        // Calculate tangent and bitangent
        glm::vec3 tangent = glm::normalize(glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)));
        if (glm::length(tangent) < 0.001f) {
            tangent = glm::normalize(glm::cross(normal, glm::vec3(1.0f, 0.0f, 0.0f)));
        }
        
        vertex.Tangent = tangent;
        vertex.Bitangent = glm::normalize(glm::cross(normal, tangent));
        
        // Initialize bone weights to 0
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            vertex.m_BoneIDs[i] = 0;
            vertex.m_Weights[i] = 0.0f;
        }
        
        vertices.push_back(vertex);
    }
    
    // Generate a unique color based on position
    glm::vec3 generateUniqueColor(int position) {
        // Use a hash function to generate a unique color
        float hue = std::fmod(std::abs(position) * 0.618033988749895f, 1.0f);
        
        // Convert HSV to RGB
        float h = hue * 6.0f;
        int i = static_cast<int>(h);
        float f = h - i;
        float p = 0.0f;
        float q = 1.0f - f;
        float t = f;
        
        glm::vec3 color;
        
        switch (i % 6) {
            case 0: color = glm::vec3(1.0f, t, p); break;
            case 1: color = glm::vec3(q, 1.0f, p); break;
            case 2: color = glm::vec3(p, 1.0f, t); break;
            case 3: color = glm::vec3(p, q, 1.0f); break;
            case 4: color = glm::vec3(t, p, 1.0f); break;
            case 5: color = glm::vec3(1.0f, p, q); break;
        }
        
        return color;
    }
};

} // namespace m3D

#endif // CARTESIAN_PLANE_SHAPE_H 