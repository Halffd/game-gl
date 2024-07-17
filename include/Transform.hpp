#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glad/glad.h>
#include "Vertex.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <type_traits>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp> // Ensure this is included
// Function to draw an object with a given transformation
template<typename T>
void draw(const Shader &shader, Texture &texture, T &vao, const glm::mat4 &transform, int vertices)
{
    shader.setMat4("model", transform);
    texture.Activate(GL_TEXTURE0);
    vao.bind();
    glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, 0);
}

template<typename T>
void draw(Shader &shader, Texture* textures, int texturesN, T &vao, const glm::mat4 &transform, int vertices)
{
    
    shader.use();
    shader.setMat4("model", transform);

    // Bind textures
    for (size_t i = 0; i < texturesN; ++i)
    {
        textures[i].Activate(GL_TEXTURE0 + i);           // Activate texture unit
        shader.setInt("texture" + std::to_string(i + 1), i); // Set the uniform for the texture
    }

    vao.bind();
    glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, 0);
}
// Function to set up the transformation for an object
glm::mat4 transform(float translateX, float translateY, float scaleX, float scaleY, float rotationAngle = 0.0f, glm::vec3 rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f))
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(translateX, translateY, 0.0f));
    if (rotationAngle != 0.0f)
    {
        model = glm::rotate(model, rotationAngle, rotationAxis);
    }
    model = glm::scale(model, glm::vec3(scaleX, scaleY, 1.0f));
    return model;
}

// Helper function to check if a type is glm::vec3
template<typename T>
struct is_glm_vec3 : std::false_type {};

template<>
struct is_glm_vec3<glm::vec3> : std::true_type {};

// Helper function to check if a type is glm::quat
template<typename T>
struct is_glm_quat : std::false_type {};

template<>
struct is_glm_quat<glm::quat> : std::true_type {};

// Main transform function template
template<typename T1, typename T2, typename T3>
glm::mat4 transform(const T1& position, const T2& scale, const T3& rotation) {
    glm::mat4 model = glm::mat4(1.0f);

    if constexpr (std::is_same_v<T1, float>) {
        if (position == 0.0f) {
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        } else if (position == 1.0f) {
            model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
        }
    } else if constexpr (is_glm_vec3<T1>::value) {
        model = glm::translate(model, position);
    }

    if constexpr (std::is_same_v<T2, float>) {
        model = glm::scale(model, glm::vec3(scale, scale, scale));
    } else if constexpr (is_glm_vec3<T2>::value) {
        model = glm::scale(model, scale);
    }

    if constexpr (std::is_same_v<T3, float>) {
        if (rotation != 0.0f) {
            model = glm::rotate(model, glm::radians(15.0f * rotation), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(15.0f * rotation), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(15.0f * rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        }
    } else if constexpr (is_glm_vec3<T3>::value) {
        if (glm::length(rotation) != 0.0f) {
             model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
             model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        }
    } else if constexpr (is_glm_quat<T3>::value) {
        model *= glm::mat4_cast(rotation);
    }

    return model;
}
// Custom printing function for glm::vec
template <typename T, glm::precision P, glm::length_t L>
std::ostream &operator<<(std::ostream &os, const glm::vec<L, T, P> &v)
{
    os << "(";
    for (glm::length_t i = 0; i < L; ++i)
    {
        os << std::fixed << std::setprecision(2) << v[i];
        if (i < L - 1)
            os << ", ";
    }
    os << ")";
    return os;
}

// Custom printing function for glm::mat
template <typename T, glm::precision P, glm::length_t C, glm::length_t R>
std::ostream &operator<<(std::ostream &os, const glm::mat<C, R, T, P> &m)
{
    os << std::fixed << std::setprecision(2) << std::endl;
    for (glm::length_t i = 0; i < R; ++i)
    {
        os << "[";
        for (glm::length_t j = 0; j < C; ++j)
        {
            os << m[j][i];
            if (j < C - 1)
                os << ", ";
        }
        os << "]" << std::endl;
    }
    return os;
}
// Custom printing function for glm::quat
template <typename T, glm::precision P>
std::ostream &operator<<(std::ostream &os, const glm::qua<T, P> &q)
{
    os << "(" << std::fixed << std::setprecision(2) << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
    return os;
}

// Function to decompose a transformation matrix into position, scale, and rotation (in Euler angles)
void printTransform(const glm::mat4 &transform)
{
    glm::vec3 scale, translation, skew;
    glm::vec4 perspective;
    glm::quat rotation;

    glm::decompose(transform, scale, rotation, translation, skew, perspective);

    // Convert quaternion to Euler angles (in degrees)
    glm::vec3 eulerAngles = glm::eulerAngles(rotation);
    eulerAngles = glm::degrees(eulerAngles);

    // Print position, scale, and rotation
    std::cout << "Position: " << translation << std::endl;
    std::cout << "Scale: " << scale << std::endl;
    std::cout << "Rotation (Euler angles): " << eulerAngles << std::endl;
    std::cout << "Rotation (quaternion): " << rotation << std::endl;
    std::cout << "Perspective: " << perspective << std::endl;
    std::cout << "Skew: " << skew << std::endl;
}

#endif