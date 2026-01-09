#ifndef ENHANCED_VERTEX_BUFFER_H
#define ENHANCED_VERTEX_BUFFER_H

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace VO {

// Structure to hold vertex data
struct VertexData {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

// Buffer management strategies
enum class BufferStrategy {
    INTERLEAVED,    // Traditional interleaved layout: 123123123...
    BATCHED,        // Batched layout: 111122223333...
    DYNAMIC         // Dynamic buffer updates
};

class EnhancedVertexBuffer {
private:
    unsigned int VAO, VBO, EBO;
    unsigned int elementCount;
    BufferStrategy strategy;
    
    // Individual buffer objects for batched strategy
    unsigned int positionVBO, normalVBO, texCoordVBO, tangentVBO, bitangentVBO;
    
    // Memory mapping pointer
    void* mappedMemory;
    bool isMapped;

public:
    EnhancedVertexBuffer();
    ~EnhancedVertexBuffer();
    
    // Initialize with different strategies
    void initializeInterleaved(const std::vector<VertexData>& vertices, const std::vector<unsigned int>& indices);
    void initializeBatched(const std::vector<glm::vec3>& positions, 
                          const std::vector<glm::vec3>& normals,
                          const std::vector<glm::vec2>& texCoords,
                          const std::vector<glm::vec3>& tangents,
                          const std::vector<glm::vec3>& bitangents,
                          const std::vector<unsigned int>& indices);
    void initializeDynamic(size_t maxVertices);
    
    // Buffer update methods
    void updateInterleavedSubData(size_t offset, size_t count, const VertexData* data);
    void updateBatchedSubData(const std::vector<glm::vec3>* positions = nullptr,
                             const std::vector<glm::vec3>* normals = nullptr,
                             const std::vector<glm::vec2>* texCoords = nullptr,
                             const std::vector<glm::vec3>* tangents = nullptr,
                             const std::vector<glm::vec3>* bitangents = nullptr);
    
    // Memory mapping methods
    void* mapBuffer(GLenum access = GL_WRITE_ONLY);
    bool unmapBuffer();
    bool isBufferMapped() const { return isMapped; }
    
    // Buffer copying methods
    void copyFrom(const EnhancedVertexBuffer& source, 
                  size_t readOffset, size_t writeOffset, size_t size);
    
    // Rendering methods
    void bind() const;
    void unbind() const;
    void draw() const;
    void drawInstanced(unsigned int instanceCount) const;
    
    // Getters
    unsigned int getVAO() const { return VAO; }
    unsigned int getVBO() const { return VBO; }
    unsigned int getEBO() const { return EBO; }
    unsigned int getElementCount() const { return elementCount; }
    BufferStrategy getStrategy() const { return strategy; }
    
    // Utility methods
    void setStrategy(BufferStrategy newStrategy) { strategy = newStrategy; }
    
private:
    void setupInterleavedAttributes();
    void setupBatchedAttributes();
    void setupElementBuffer(const std::vector<unsigned int>& indices);
    void generateTangentsBitangents(std::vector<VertexData>& vertices, 
                                   const std::vector<unsigned int>& indices);
};

} // namespace VO

#endif // ENHANCED_VERTEX_BUFFER_H