#include <glad/glad.h>
#include <vector>
#include <utility>
#include <iostream>

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

    /**
     * @brief Sets up the data for the buffer.
     * @param vertices A pointer to an array of floats to use as data.
     * @param size The size in bytes of the data.
     * @param usage The usage type, default GL_STATIC_DRAW.
     */
    void setup(const GLfloat *vertices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

    /**
     * @brief Sets up the data for the buffer.
     * @tparam T Things stored *contiguously* in memory.
     * @param vertices A pointer to an array of Ts to use as data.
     * @param size The size in bytes of the data.
     * @param usage The usage type, default GL_STATIC_DRAW.
     */
    template <typename T>
    void setup(const T *vertices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
        glCheckError(__FILE__, __LINE__);
    }

    /**
     * @brief Sets up the data for the buffer.
     * @tparam T You might want this to some numeric, vector, etc type for good result,
     * @param vertices A std::vector of something to use as data.
     * @param usage The usage type, default GL_STATIC_DRAW.
     */
    template <typename T>
    void setup(const std::vector<T> &vertices, GLenum usage = GL_STATIC_DRAW)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), usage);
        glCheckError(__FILE__, __LINE__);
    }

    /**
     * @brief Sets up the data for the buffer avoiding the cost of reallocating
     *  the data store.
     * @tparam T Things stored *contiguously* in memory.
     * @param vertices A pointer to an array of Ts to use as data.
     * @param size The size in bytes of the data.
     * @param offset The offset into the buffer object's data store where data
     *  replacement will begin, measured in bytes.
     */
    template <typename T>
    void setupSubData(const T *vertices, GLsizeiptr size, GLintptr offset = 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, vertices);
        glCheckError(__FILE__, __LINE__);
    }

    /**
     * @brief Sets up the data for the buffer avoiding the cost of reallocating
     *  the data store.
     * @tparam T You might want this to some numeric, vector, etc type for good result,
     * @param vertices A std::vector of floats to use as data.
     * @param offset The offset into the buffer object's data store where data
     *  replacement will begin, measured in bytes.
     */
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
    VAO() = default;
    VAO(const VAO &other) = delete;
    VAO(VAO &&other) { *this = std::move(other); }
    ~VAO()
    {
        if (m_id)
            glDeleteVertexArrays(1, &m_id);
    }
    VAO &operator=(const VAO &other) = delete;
    VAO &operator=(VAO &&other)
    {
        if (this != &other)
        {
            if (m_id)
                glDeleteVertexArrays(1, &m_id);
            m_id = std::exchange(other.m_id, 0);
        }
        return *this;
    }

    /**
     * @brief Binds the VAO.
     */
    void bind() const { glBindVertexArray(m_id); }

    void setVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
    {
        bind();
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
        glEnableVertexAttribArray(index);
        unbind();
    }
    void set(GLuint index, GLint size, GLsizei stride, const void *pointer)
    {
        GLenum type = GL_FLOAT;
        GLboolean normalized = GL_FALSE;
        bind();
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
        glEnableVertexAttribArray(index);
        unbind();
    }

    /**
     * @brief Specifies how OpenGL should interpret the vertex buffer data whenever a draw call is made.
     * @param vbo The vertex buffer object to be binded.
     * @param layout Specifies the index of the generic vertex attribute to be modified. Must match the layout in the shader.
     * @param components Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
     * @param type Type of the data.
     * @param stride Specifies the byte offset between consecutive generic vertex attributes.
     * @param offset Specifies a offset of the first component of the first generic vertex attribute in the array in the data store.
     * @param normalize Specifies whether fixed-point data values should be normalized.
     */
    void linkAttrib(const VBO &vbo, GLuint layout, GLuint components, GLenum type, GLsizei stride, void *offset, GLboolean normalize = GL_FALSE) const;

    /**
     * @brief Specifies how OpenGL should interpret the vertex buffer data whenever a draw call is made. IT DOESN'T BIND ANYTHING!
     * @param layout Specifies the index of the generic vertex attribute to be modified. Must match the layout in the shader.
     * @param components Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
     * @param type Type of the data.
     * @param stride Specifies the byte offset between consecutive generic vertex attributes.
     * @param offset Specifies a offset of the first component of the first generic vertex attribute in the array in the data store.
     * @param normalize Specifies whether fixed-point data values should be normalized.
     */
    void linkAttribFast(GLuint layout, GLuint components, GLenum type, GLsizei stride, void *offset, GLboolean normalize = GL_FALSE) const;

    /**
     * @brief Generates the vertex array buffer.
     */
    void genVertexArray();

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
    template <typename T>
    void setup(const std::vector<T> indices)
    {
        bind();
        // Accessing the underlying raw array
        const uint32_t* arr = reinterpret_cast<const uint32_t*>(indices.data());
        // Get the size of the vector
        size_t size = indices.size() * sizeof(uint32_t);
        #if DEBUG == 1
        std::cout << size << "\n";
        #endif
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, arr, GL_STATIC_DRAW);
    }
    void setup(const void* indices, GLsizei size)
    {
        bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
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