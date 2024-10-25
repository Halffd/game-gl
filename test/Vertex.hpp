#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glad/glad.h>
#include <vector>
#include <utility>
#include <iostream>

#include "vertex.h"
#include "Util.hpp"

class VBO
{

public:
    VBO() = default;
    VBO(const VBO &other) = delete;
    VBO(VBO &&other) { *this = std::move(other); }
    ~VBO()
    {
        if (m_id)
            glDeleteBuffers(1, &m_id);
    }
    VBO &operator=(const VBO &other) = delete;
    VBO &operator=(VBO &&other)
    {
        if (this != &other)
        {
            if (m_id)
                glDeleteBuffers(1, &m_id);
            m_id = std::exchange(other.m_id, 0);
        }
        return *this;
    }

    /**
     * @brief Binds the VBO.
     */
    void bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_id); }

    /**
     * @brief Generates the buffer for the VBO.
     */
    void genBuffer()
    {
        glGenBuffers(1, &m_id);
        glCheckError(__FILE__, __LINE__);
    };
 void setup(const GLfloat *vertices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
        glCheckError(__FILE__, __LINE__);
    }

    template <typename T>
    void setup(const T *vertices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glCheckError(__FILE__, __LINE__);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
        glCheckError(__FILE__, __LINE__);
    }

    template <typename T>
    void setup(const std::vector<T> &vertices, GLenum usage = GL_STATIC_DRAW)
    {
     glCheckError(__FILE__, __LINE__);
    }

    template <typename T>
    void setupSubData(const T *vertices, GLsizeiptr size, GLintptr offset = 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, vertices);
        glCheckError(__FILE__, __LINE__);
    }

    template <typename T>
    void setupSubData(const std::vector<T> &vertices, GLintptr offset = 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferSubData(GL_ARRAY_BUFFER, offset, vertices.size() * sizeof(T), vertices.data());
        glCheckError(__FILE__, __LINE__);
    }
    /**
     * @brief Unbinds the VBO.
     */
    void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

private:
    GLuint m_id{};
};
class VAO
{

public:
    unsigned int ebo = 0;
    TOPOLOGY Topology = TRIANGLES;
    std::vector<unsigned int> Indices;
    size_t vertexCount;

    VAO() = default;
    VAO(const VAO &other) = delete;
    VAO(VAO &&other) { *this = std::move(other); }
    ~VAO()
    {
        if (m_id){
            glDeleteVertexArrays(1, &m_id);
            glCheckError(__FILE__, __LINE__);
        }
    }
    VAO &operator=(const VAO &other) = delete;
    VAO &operator=(VAO &&other)
    {
        if (this != &other)
        {
            if (m_id){
                glDeleteVertexArrays(1, &m_id);
                glCheckError(__FILE__, __LINE__);
            }
            m_id = std::exchange(other.m_id, 0);
        }
        return *this;
    }

    /**
     * @brief Binds the VAO.
     */
    int bind() const {
     glBindVertexArray(m_id);
     glCheckError(__FILE__, __LINE__);
      return ebo;
       }

    void setVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
    {
        bind();
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
        glCheckError(__FILE__, __LINE__);
        glEnableVertexAttribArray(index);
        glCheckError(__FILE__, __LINE__);
        unbind();
    }
    void set(GLuint index, GLint size, GLsizei stride, const void *pointer)
    {
        GLenum type = GL_FLOAT;
        GLboolean normalized = GL_FALSE;
        bind();
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
        glCheckError(__FILE__, __LINE__);
        glEnableVertexAttribArray(index);
        glCheckError(__FILE__, __LINE__);
        unbind();
    }

     void linkAttrib(const VBO &vbo, GLuint layout, GLuint components, GLenum type, GLsizei stride, void *offset, GLboolean normalize = GL_FALSE) const
    {
        vbo.bind();
        glVertexAttribPointer(layout, components, type, normalize, stride, offset);
        glCheckError(__FILE__, __LINE__);
        glEnableVertexAttribArray(layout);
        glCheckError(__FILE__, __LINE__);
        vbo.unbind();
    }

    void linkAttribFast(GLuint layout, GLuint components, GLenum type, GLsizei stride, void *offset, GLboolean normalize = GL_FALSE) const
    {
        glVertexAttribPointer(layout, components, type, normalize, stride, offset);
        glCheckError(__FILE__, __LINE__);
        glEnableVertexAttribArray(layout);
        glCheckError(__FILE__, __LINE__);
    }

    void genVertexArray()
    {
        glGenVertexArrays(1, &m_id);
        glCheckError(__FILE__, __LINE__);
    }

    /**
     * @brief Unbinds the VAO.
     */
    void unbind() const { glBindVertexArray(0); }

private:
    GLuint m_id{};
};

class EBO
{
public:
    EBO() = default;
    ~EBO()
    {
        if (m_id) {
            glDeleteBuffers(1, &m_id);
            glCheckError(__FILE__, __LINE__);
        }
    }

    void genBuffer()
    {
        glGenBuffers(1, &m_id);
        glCheckError(__FILE__, __LINE__);
    }

    void bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
        glCheckError(__FILE__, __LINE__);
    }

    void unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glCheckError(__FILE__, __LINE__);
    }   
    void setup(const std::vector<uint32_t> indices)
    {
        bind();
        // Accessing the underlying raw array
        const uint32_t* arr = indices.data();
        // Get the size of the vector
        size_t size = indices.size() * sizeof(uint32_t);
        #if DEBUG == 1
        std::cout << size << "\n";
        #endif
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, arr, GL_STATIC_DRAW);
        glCheckError(__FILE__, __LINE__);
    }
    void setup(const void* indices, GLsizei size)
    {
        bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
        glCheckError(__FILE__, __LINE__);
    }

private:
    GLuint m_id{};
};
/*struct EBO
{
    EBO() = default;
    EBO(const EBO &other) = delete;
    EBO(EBO &&other) { *this = std::move(other); }
    ~EBO()
    {
        if (m_id)
            glDeleteBuffers(1, &m_id);
    }
    EBO &operator=(const EBO &other) = delete;
    EBO &operator=(EBO &&other)
    {
        if (this != &other)
        {
            if (m_id)
                glDeleteBuffers(1, &m_id);
            m_id = std::exchange(other.m_id, 0);
        }
        return *this;
    }

    void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id); }
    void genBuffer();

    template <typename T>
    void setup(const T *indices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, usage);
        // glCheckError(__FILE__, __LINE__);
    }

    template <typename T>
    void setup(const std::vector<T> &indices, GLenum usage = GL_STATIC_DRAW)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(T), indices.data(), usage);
        glCheckError(__FILE__, __LINE__);
    }

    void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

    GLuint m_id{};
};*/
#endif