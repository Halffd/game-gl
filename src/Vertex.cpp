#include <glad/glad.h>
#include <vector>
#include <utility>
#include <iostream>

#include "vertex.h"
#include "Util.hpp"

GLenum getPrimitive(TOPOLOGY topology) {
    switch (topology) {
        case NONE:
            return GL_INVALID_ENUM;
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
    if (id)
        glDeleteBuffers(1, &id);
}

VBO &VBO::operator=(VBO &&other) {
    if (this != &other) {
        if (id)
            glDeleteBuffers(1, &id);
        id = std::exchange(other.id, 0);
    }
    return *this;
}

void VBO::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::genBuffer() {
    glGenBuffers(1, &id);
    glCheckError(__FILE__, __LINE__);
}

void VBO::setup(const GLfloat *vertices, GLsizeiptr size, GLenum usage) {
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
    glCheckError(__FILE__, __LINE__);
}

template <typename T>
void VBO::setup(const T *vertices, GLsizeiptr size, GLenum usage) {
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
    glCheckError(__FILE__, __LINE__);
}

template <typename T>
void VBO::setup(const std::vector<T> &vertices, GLenum usage) {
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), usage);
    glCheckError(__FILE__, __LINE__);
}

template <typename T>
void VBO::setupSubData(const T *vertices, GLsizeiptr size, GLintptr offset) {
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, vertices);
    glCheckError(__FILE__, __LINE__);
}

template <typename T>
void VBO::setupSubData(const std::vector<T> &vertices, GLintptr offset) {
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, vertices.size() * sizeof(T), vertices.data());
    glCheckError(__FILE__, __LINE__);
}

void VBO::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


VAO::~VAO() {
    if (id)
        glDeleteVertexArrays(1, &id);
}
VAO::VAO(std::vector<math::vec3> positions, std::vector<unsigned int> indices)
{
    Positions = positions;
    Indices = indices;
}
VAO &VAO::operator=(VAO &&other) {
    if (this != &other) {
        if (id)
            glDeleteVertexArrays(1, &id);
        id = std::exchange(other.id, 0);
    }
    return *this;
}
// --------------------------------------------------------------------------------------------
void VAO::SetPositions(std::vector<math::vec3> positions)
{
    Positions = positions;
}
void VAO::SetIndices(std::vector<unsigned int> indices){
    Indices = indices;
}

// --------------------------------------------------------------------------------------------

int VAO::bind() const {
    glBindVertexArray(id);
    return Indices.size();
}
bool VAO::exists() const {
    return id != 0;
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
    glGenVertexArrays(1, &id);
    glCheckError(__FILE__, __LINE__);
}

void VAO::unbind() const {
    glBindVertexArray(0);
}


EBO::~EBO() {
    if (id) {
        glDeleteBuffers(1, &id);
    }
}

void EBO::genBuffer() {
    glGenBuffers(1, &id);
    glCheckError(__FILE__, __LINE__);
}

void EBO::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
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
        case TOPOLOGY::NONE:
            os << "NONE";
            break;
        case TOPOLOGY::POINTS:
            os << "POINTS";
            break;
        case TOPOLOGY::LINES:
            os << "LINES";
            break;
        case TOPOLOGY::LINE_STRIP:
            os << "LINE_STRIP";
            break;
        case TOPOLOGY::LINE_LOOP:
            os << "LINE_LOOP";
            break;
        case TOPOLOGY::TRIANGLES:
            os << "TRIANGLES";
            break;
        case TOPOLOGY::TRIANGLE_STRIP:
            os << "TRIANGLE_STRIP";
            break;
        case TOPOLOGY::TRIANGLE_FAN:
            os << "TRIANGLE_FAN";
            break;
    }
    return os;
}