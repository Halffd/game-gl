#include "EnhancedVertexBuffer.h"
#include <iostream>
#include <cstring>

namespace VO {

EnhancedVertexBuffer::EnhancedVertexBuffer() 
    : VAO(0), VBO(0), EBO(0), elementCount(0), strategy(BufferStrategy::INTERLEAVED),
      positionVBO(0), normalVBO(0), texCoordVBO(0), tangentVBO(0), bitangentVBO(0),
      mappedMemory(nullptr), isMapped(false) {
}

EnhancedVertexBuffer::~EnhancedVertexBuffer() {
    if (isMapped) {
        unmapBuffer();
    }
    
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    
    // Delete individual buffers for batched strategy
    if (positionVBO) glDeleteBuffers(1, &positionVBO);
    if (normalVBO) glDeleteBuffers(1, &normalVBO);
    if (texCoordVBO) glDeleteBuffers(1, &texCoordVBO);
    if (tangentVBO) glDeleteBuffers(1, &tangentVBO);
    if (bitangentVBO) glDeleteBuffers(1, &bitangentVBO);
}

void EnhancedVertexBuffer::initializeInterleaved(const std::vector<VertexData>& vertices, 
                                               const std::vector<unsigned int>& indices) {
    strategy = BufferStrategy::INTERLEAVED;
    
    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Allocate and fill buffer with vertex data
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), 
                 vertices.data(), GL_STATIC_DRAW);
    
    // Setup vertex attributes
    setupInterleavedAttributes();
    
    // Setup element buffer if indices are provided
    if (!indices.empty()) {
        setupElementBuffer(indices);
    }
    
    elementCount = indices.empty() ? vertices.size() : indices.size();
    
    glBindVertexArray(0);
}

void EnhancedVertexBuffer::initializeBatched(const std::vector<glm::vec3>& positions, 
                                          const std::vector<glm::vec3>& normals,
                                          const std::vector<glm::vec2>& texCoords,
                                          const std::vector<glm::vec3>& tangents,
                                          const std::vector<glm::vec3>& bitangents,
                                          const std::vector<unsigned int>& indices) {
    strategy = BufferStrategy::BATCHED;
    
    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Generate individual buffers for each attribute
    glGenBuffers(1, &positionVBO);
    glGenBuffers(1, &normalVBO);
    glGenBuffers(1, &texCoordVBO);
    glGenBuffers(1, &tangentVBO);
    glGenBuffers(1, &bitangentVBO);
    
    // Fill position buffer
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), 
                 positions.data(), GL_STATIC_DRAW);
    
    // Fill normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), 
                 normals.data(), GL_STATIC_DRAW);
    
    // Fill texture coordinate buffer
    glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), 
                 texCoords.data(), GL_STATIC_DRAW);
    
    // Fill tangent buffer
    glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
    glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), 
                 tangents.data(), GL_STATIC_DRAW);
    
    // Fill bitangent buffer
    glBindBuffer(GL_ARRAY_BUFFER, bitangentVBO);
    glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), 
                 bitangents.data(), GL_STATIC_DRAW);
    
    // Setup vertex attributes for batched layout
    setupBatchedAttributes();
    
    // Setup element buffer if indices are provided
    if (!indices.empty()) {
        setupElementBuffer(indices);
    }
    
    elementCount = indices.empty() ? positions.size() : indices.size();
    
    glBindVertexArray(0);
}

void EnhancedVertexBuffer::initializeDynamic(size_t maxVertices) {
    strategy = BufferStrategy::DYNAMIC;
    
    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Generate and bind VBO with dynamic storage
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Allocate buffer without initial data for dynamic usage
    glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(VertexData), 
                 nullptr, GL_DYNAMIC_DRAW);
    
    // Setup vertex attributes
    setupInterleavedAttributes();
    
    glBindVertexArray(0);
}

void EnhancedVertexBuffer::setupInterleavedAttributes() {
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
                         (void*)offsetof(VertexData, Position));
    glEnableVertexAttribArray(0);
    
    // Normal attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
                         (void*)offsetof(VertexData, Normal));
    glEnableVertexAttribArray(1);
    
    // Texture coordinate attribute (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
                         (void*)offsetof(VertexData, TexCoords));
    glEnableVertexAttribArray(2);
    
    // Tangent attribute (location = 3)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
                         (void*)offsetof(VertexData, Tangent));
    glEnableVertexAttribArray(3);
    
    // Bitangent attribute (location = 4)
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
                         (void*)offsetof(VertexData, Bitangent));
    glEnableVertexAttribArray(4);
}

void EnhancedVertexBuffer::setupBatchedAttributes() {
    // Position attribute (location = 0)
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute (location = 1)
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Texture coordinate attribute (location = 2)
    glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    
    // Tangent attribute (location = 3)
    glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);
    
    // Bitangent attribute (location = 4)
    glBindBuffer(GL_ARRAY_BUFFER, bitangentVBO);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(4);
    
    // Rebind to main VBO for other operations
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void EnhancedVertexBuffer::setupElementBuffer(const std::vector<unsigned int>& indices) {
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 indices.data(), GL_STATIC_DRAW);
}

void EnhancedVertexBuffer::updateInterleavedSubData(size_t offset, size_t count, const VertexData* data) {
    if (strategy != BufferStrategy::INTERLEAVED && strategy != BufferStrategy::DYNAMIC) {
        std::cerr << "Error: Cannot update interleaved sub-data with non-interleaved strategy" << std::endl;
        return;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(VertexData), 
                    count * sizeof(VertexData), data);
}

void EnhancedVertexBuffer::updateBatchedSubData(const std::vector<glm::vec3>* positions,
                                              const std::vector<glm::vec3>* normals,
                                              const std::vector<glm::vec2>* texCoords,
                                              const std::vector<glm::vec3>* tangents,
                                              const std::vector<glm::vec3>* bitangents) {
    if (strategy != BufferStrategy::BATCHED) {
        std::cerr << "Error: Cannot update batched sub-data with non-batched strategy" << std::endl;
        return;
    }
    
    // Update position buffer if provided
    if (positions && !positions->empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, positions->size() * sizeof(glm::vec3), 
                       positions->data());
    }
    
    // Update normal buffer if provided
    if (normals && !normals->empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normals->size() * sizeof(glm::vec3), 
                       normals->data());
    }
    
    // Update texture coordinate buffer if provided
    if (texCoords && !texCoords->empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texCoords->size() * sizeof(glm::vec2), 
                       texCoords->data());
    }
    
    // Update tangent buffer if provided
    if (tangents && !tangents->empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, tangents->size() * sizeof(glm::vec3), 
                       tangents->data());
    }
    
    // Update bitangent buffer if provided
    if (bitangents && !bitangents->empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, bitangentVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, bitangents->size() * sizeof(glm::vec3), 
                       bitangents->data());
    }
}

void* EnhancedVertexBuffer::mapBuffer(GLenum access) {
    if (isMapped) {
        std::cerr << "Warning: Buffer already mapped" << std::endl;
        return nullptr;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    mappedMemory = glMapBuffer(GL_ARRAY_BUFFER, access);
    
    if (mappedMemory) {
        isMapped = true;
        return mappedMemory;
    }
    
    std::cerr << "Error: Failed to map buffer" << std::endl;
    return nullptr;
}

bool EnhancedVertexBuffer::unmapBuffer() {
    if (!isMapped) {
        std::cerr << "Warning: Buffer not mapped" << std::endl;
        return false;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    bool success = glUnmapBuffer(GL_ARRAY_BUFFER) == GL_TRUE;
    
    if (success) {
        isMapped = false;
        mappedMemory = nullptr;
    } else {
        std::cerr << "Error: Failed to unmap buffer" << std::endl;
    }
    
    return success;
}

void EnhancedVertexBuffer::copyFrom(const EnhancedVertexBuffer& source, 
                                  size_t readOffset, size_t writeOffset, size_t size) {
    // Bind source and destination buffers to copy targets
    glBindBuffer(GL_COPY_READ_BUFFER, source.VBO);
    glBindBuffer(GL_COPY_WRITE_BUFFER, VBO);
    
    // Copy data from source to destination
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 
                       readOffset, writeOffset, size);
    
    // Unbind
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void EnhancedVertexBuffer::bind() const {
    glBindVertexArray(VAO);
}

void EnhancedVertexBuffer::unbind() const {
    glBindVertexArray(0);
}

void EnhancedVertexBuffer::draw() const {
    glBindVertexArray(VAO);
    if (EBO) {
        glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, elementCount);
    }
    glBindVertexArray(0);
}

void EnhancedVertexBuffer::drawInstanced(unsigned int instanceCount) const {
    glBindVertexArray(VAO);
    if (EBO) {
        glDrawElementsInstanced(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0, instanceCount);
    } else {
        glDrawArraysInstanced(GL_TRIANGLES, 0, elementCount, instanceCount);
    }
    glBindVertexArray(0);
}

void EnhancedVertexBuffer::generateTangentsBitangents(std::vector<VertexData>& vertices, 
                                                    const std::vector<unsigned int>& indices) {
    // Initialize tangents and bitangents to zero
    for (auto& vertex : vertices) {
        vertex.Tangent = glm::vec3(0.0f);
        vertex.Bitangent = glm::vec3(0.0f);
    }
    
    // Calculate tangents and bitangents for each triangle
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];
        
        VertexData& v0 = vertices[i0];
        VertexData& v1 = vertices[i1];
        VertexData& v2 = vertices[i2];
        
        // Edges of the triangle
        glm::vec3 edge1 = v1.Position - v0.Position;
        glm::vec3 edge2 = v2.Position - v0.Position;
        
        // UV deltas
        glm::vec2 deltaUV1 = v1.TexCoords - v0.TexCoords;
        glm::vec2 deltaUV2 = v2.TexCoords - v0.TexCoords;
        
        // Calculate tangent and bitangent
        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        
        glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);
        
        // Add to existing values
        v0.Tangent += tangent;
        v1.Tangent += tangent;
        v2.Tangent += tangent;
        
        v0.Bitangent += bitangent;
        v1.Bitangent += bitangent;
        v2.Bitangent += bitangent;
    }
    
    // Normalize the accumulated tangents and bitangents
    for (auto& vertex : vertices) {
        vertex.Tangent = glm::normalize(vertex.Tangent);
        vertex.Bitangent = glm::normalize(vertex.Bitangent);
    }
}

} // namespace VO