#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

#include "render/Vertex.h"
#include <glm/glm.hpp>

namespace VO {
class Quad {
private:
    VAO m_vao;
    VBO m_vbo;
    EBO m_ebo;
    
public:
    Quad() {
        setup();
    }
    
    void setup() {
        // Full-screen quad vertices (pos + texcoords)
        float vertices[] = {
            // positions   // texture coords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
        
        uint32_t indices[] = {
            0, 1, 2,
            0, 2, 3
        };
        
        m_vao.genVertexArray();
        m_vbo.genBuffer();
        m_ebo.genBuffer();
        
        m_vao.bind();
        m_vbo.setup(vertices, sizeof(vertices));
        m_ebo.setup(indices, sizeof(indices));
        
        // Position attribute
        m_vao.linkAttribFast(0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
        // Texture coord attribute
        m_vao.linkAttribFast(1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        
        m_vao.unbind();
        m_vao.vertexCount = 6;
    }
    
    void draw() {
        m_vao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        m_vao.unbind();
    }
};

class Triangle {
private:
    VAO m_vao;
    VBO m_vbo;
    
public:
    Triangle(const glm::vec3& p1 = glm::vec3(-0.5f, -0.5f, 0.0f),
             const glm::vec3& p2 = glm::vec3(0.5f, -0.5f, 0.0f),
             const glm::vec3& p3 = glm::vec3(0.0f, 0.5f, 0.0f)) {
        setup(p1, p2, p3);
    }
    
    void setup(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
        float vertices[] = {
            p1.x, p1.y, p1.z,
            p2.x, p2.y, p2.z,
            p3.x, p3.y, p3.z
        };
        
        m_vao.genVertexArray();
        m_vbo.genBuffer();
        
        m_vao.bind();
        m_vbo.setup(vertices, sizeof(vertices));
        
        // Position attribute
        m_vao.linkAttribFast(0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
        
        m_vao.unbind();
        m_vao.vertexCount = 3;
    }
    
    void draw() {
        m_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        m_vao.unbind();
    }
};

class Line {
private:
    VAO m_vao;
    VBO m_vbo;
    
public:
    Line(const glm::vec3& start = glm::vec3(-1.0f, 0.0f, 0.0f),
         const glm::vec3& end = glm::vec3(1.0f, 0.0f, 0.0f)) {
        setup(start, end);
    }
    
    void setup(const glm::vec3& start, const glm::vec3& end) {
        float vertices[] = {
            start.x, start.y, start.z,
            end.x,   end.y,   end.z
        };
        
        m_vao.genVertexArray();
        m_vbo.genBuffer();
        
        m_vao.bind();
        m_vbo.setup(vertices, sizeof(vertices));
        
        // Position attribute
        m_vao.linkAttribFast(0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
        
        m_vao.unbind();
        m_vao.vertexCount = 2;
    }
    
    void setLineWidth(float width) {
        glLineWidth(width);
    }
    
    void draw() {
        m_vao.bind();
        glDrawArrays(GL_LINES, 0, 2);
        m_vao.unbind();
    }
};
}
#endif