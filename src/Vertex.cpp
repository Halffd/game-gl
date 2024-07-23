#include <glad/glad.h>
#include <vector>
#include <utility>
#include <iostream>

#include "vertex.h"
#include "Util.hpp"

GLenum getPrimitive(TOPOLOGY topology) {
    switch (topology) {
        case POINTS:
            return GL_POINTS;
        case LINES:
            return GL_LINES;
        case LINE_STRIP:
            return GL_LINE_STRIP;
        case LINE_LOOP:
            return GL_LINE_LOOP;
        case TRIANGLES:
            return GL_TRIANGLES;
        case TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        case TRIANGLE_FAN:
            return GL_TRIANGLE_FAN;
        default:
            return GL_INVALID_ENUM;
    }
}

VBO::~VBO() {
    if (m_id)
        glDeleteBuffers(1, &m_id);
}

VBO &VBO::operator=(VBO &&other) {
    if (this != &other) {
        if (m_id)
            glDeleteBuffers(1, &m_id);
        m_id = std::exchange(other.m_id, 0);
    }
    return *this;
}

void VBO::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VBO::genBuffer() {
    glGenBuffers(1, &m_id);
    glCheckError(__FILE__, __LINE__);
}

void VBO::setup(const GLfloat *vertices, GLsizeiptr size, GLenum usage) {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
    glCheckError(__FILE__, __LINE__);
}

template <typename T>
void VBO::setup(const T *vertices, GLsizeiptr size, GLenum usage) {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
    glCheckError(__FILE__, __LINE__);
}

template <typename T>
void VBO::setup(const std::vector<T> &vertices, GLenum usage) {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), usage);
    glCheckError(__FILE__, __LINE__);
}

template <typename T>
void VBO::setupSubData(const T *vertices, GLsizeiptr size, GLintptr offset) {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, vertices);
    glCheckError(__FILE__, __LINE__);
}

template <typename T>
void VBO::setupSubData(const std::vector<T> &vertices, GLintptr offset) {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, vertices.size() * sizeof(T), vertices.data());
    glCheckError(__FILE__, __LINE__);
}

void VBO::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


VAO::~VAO() {
    if (m_id)
        glDeleteVertexArrays(1, &m_id);
}

VAO &VAO::operator=(VAO &&other) {
    if (this != &other) {
        if (m_id)
            glDeleteVertexArrays(1, &m_id);
        m_id = std::exchange(other.m_id, 0);
    }
    return *this;
}

int VAO::bind() const {
    glBindVertexArray(m_id);
    return ebo;
}

void VAO::setVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
    bind();
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(index);
    unbind();
}

void VAO::set(GLuint index, GLint size, GLsizei stride, const void *pointer) {
    GLenum type = GL_FLOAT;
    GLboolean normalized = GL_FALSE;
    bind();
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(index);
    unbind();
}

void VAO::linkAttrib(const VBO &vbo, GLuint layout, GLuint components, GLenum type, GLsizei stride, void *offset, GLboolean normalize) const {
    vbo.bind();
    glVertexAttribPointer(layout, components, type, normalize, stride, offset);
    glEnableVertexAttribArray(layout);
    vbo.unbind();
}

void VAO::linkAttribFast(GLuint layout, GLuint components, GLenum type, GLsizei stride, void *offset, GLboolean normalize) const {
    glVertexAttribPointer(layout, components, type, normalize, stride, offset);
    glEnableVertexAttribArray(layout);
}

void VAO::genVertexArray() {
    glGenVertexArrays(1, &m_id);
    glCheckError(__FILE__, __LINE__);
}

void VAO::unbind() const {
    glBindVertexArray(0);
}


EBO::~EBO() {
    if (m_id) {
        glDeleteBuffers(1, &m_id);
    }
}

void EBO::genBuffer() {
    glGenBuffers(1, &m_id);
    glCheckError(__FILE__, __LINE__);
}

void EBO::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    glCheckError(__FILE__, __LINE__);
}

void EBO::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glCheckError(__FILE__, __LINE__);
}

void EBO::setup(const std::vector<uint32_t> indices) {
    bind();
    const uint32_t* arr = indices.data();
    size_t size = indices.size() * sizeof(uint32_t);
#if DEBUG == 1
    std::cout << size << "\n";
#endif
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, arr, GL_STATIC_DRAW);
}

void EBO::setup(const void* indices, GLsizei size) {
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

// Overload operator<< to print a math::vec3
std::ostream& operator<<(std::ostream& os, const math::vec3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

// Overload operator<< to print a math::vec2
std::ostream& operator<<(std::ostream& os, const math::vec2& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

// Overload operator<< to print a std::vector of math::vec3
std::ostream& operator<<(std::ostream& os, const std::vector<math::vec3>& vec) {
    os << "[";
    bool first = true;
    for (const auto& v : vec) {
        if (!first) {
            os << ", ";
        }
        os << v;
        first = false;
    }
    os << "]";
    return os;
}

// Overload operator<< to print a std::vector of math::vec2
std::ostream& operator<<(std::ostream& os, const std::vector<math::vec2>& vec) {
    os << "[";
    bool first = true;
    for (const auto& v : vec) {
        if (!first) {
            os << ", ";
        }
        os << v;
        first = false;
    }
    os << "]";
    return os;
}

// Overload operator<< to print a std::vector of unsigned int
std::ostream& operator<<(std::ostream& os, const std::vector<unsigned int>& vec) {
    os << "[";
    bool first = true;
    for (const auto& i : vec) {
        if (!first) {
            os << ", ";
        }
        os << i;
        first = false;
    }
    os << "]";
    return os;
}

// Overload operator<< to print the TOPOLOGY enum
std::ostream& operator<<(std::ostream& os, const TOPOLOGY& t) {
    switch (t) {
        case TRIANGLES:
            os << "TRIANGLES";
            break;
        case LINE_STRIP:
            os << "LINE_STRIP";
            break;
        case LINES:
            os << "LINES";
            break;
        case POINTS:
            os << "POINTS";
            break;
    }
    return os;
}
