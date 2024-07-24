#ifndef VERTEX_H
#define VERTEX_H
#include <glad/glad.h>
#include <vector>
#include <cstdint>
#include <ostream>
#include "math/vector.h"

enum TOPOLOGY
{
    NONE,
    POINTS,
    LINES,
    LINE_STRIP,
    LINE_LOOP,
    TRIANGLES,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
};

// Overload operator<< to print a math::vec3
std::ostream& operator<<(std::ostream& os, const math::vec3& v);

// Overload operator<< to print a math::vec2
std::ostream& operator<<(std::ostream& os, const math::vec2& v);

// Overload operator<< to print a std::vector of math::vec3
std::ostream& operator<<(std::ostream& os, const std::vector<math::vec3>& vec);

// Overload operator<< to print a std::vector of math::vec2
std::ostream& operator<<(std::ostream& os, const std::vector<math::vec2>& vec);

// Overload operator<< to print a std::vector of unsigned int
std::ostream& operator<<(std::ostream& os, const std::vector<unsigned int>& vec);

// Overload operator<< to print the TOPOLOGY enum
std::ostream& operator<<(std::ostream& os, const TOPOLOGY& t);

GLenum getPrimitive(TOPOLOGY topology);

class VBO {
public:
    VBO() = default;
    VBO(const VBO &other) = delete;
    VBO(VBO &&other) { *this = std::move(other); }
    ~VBO();

    VBO &operator=(const VBO &other) = delete;
    VBO &operator=(VBO &&other);

    void bind() const;
    void genBuffer();
    void setup(const GLfloat *vertices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);
    template <typename T>
    void setup(const T *vertices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);
    template <typename T>
    void setup(const std::vector<T> &vertices, GLenum usage = GL_STATIC_DRAW);
    template <typename T>
    void setupSubData(const T *vertices, GLsizeiptr size, GLintptr offset = 0);
    template <typename T>
    void setupSubData(const std::vector<T> &vertices, GLintptr offset = 0);
    void unbind() const;

private:
    unsigned int id;
};

class VAO {
public:
    TOPOLOGY Topology = TRIANGLES;
    std::vector<math::vec3> Positions;
    std::vector<unsigned int> Indices;
    size_t vertexCount;

    VAO() = default;
    VAO(std::vector<math::vec3> positions, std::vector<unsigned int> indices);
    VAO(const VAO &other) = delete;
    VAO(VAO &&other) { *this = std::move(other); }
    ~VAO();
    VAO &operator=(const VAO &other) = delete;
    VAO &operator=(VAO &&other);

    int bind() const;
    bool exists() const;
    void setVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
    void set(GLuint index, GLint size, GLsizei stride, const void *pointer);
    void linkAttrib(const VBO &vbo, GLuint layout, GLuint components, GLenum type, GLsizei stride, void *offset, GLboolean normalize = GL_FALSE) const;
    void linkAttribFast(GLuint layout, GLuint components, GLenum type, GLsizei stride, void *offset, GLboolean normalize = GL_FALSE) const;
    void genVertexArray();
    void unbind() const;
    void SetPositions(std::vector<math::vec3> positions);
    void SetIndices(std::vector<unsigned int> indices);
private:
    unsigned int id = 0;
};

class EBO {
public:
    EBO() = default;
    ~EBO();

    void genBuffer();
    void bind();
    void unbind();
    void setup(const std::vector<uint32_t> indices);
    void setup(const void* indices, GLsizei size);

private:
    unsigned int id;
};
#endif
