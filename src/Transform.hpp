#pragma once

#include <glad/glad.h>
#include "render/Shader.h"
#include "asset/Texture2D.h"
#include "util/Util.h"
#include "render/Vertex.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <type_traits>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "util/common.h"

// Helper function to check if a type is glm::vec3
template<typename T>
struct is_vec3 : std::false_type {};

template<>
struct is_vec3<glm::vec3> : std::true_type {};

// Helper function to check if a type is glm::mat4
template<typename T>
struct is_mat4 : std::false_type {};

template<>
struct is_mat4<glm::mat4> : std::true_type {};

// Helper function to get primitive type
inline unsigned int getPrimitive(VO::TOPOLOGY topology)
{
    switch (topology)
    {
    case VO::POINTS:
        return GL_POINTS;
    case VO::LINES:
        return GL_LINES;
    case VO::LINE_STRIP:
        return GL_LINE_STRIP;
    case VO::LINE_LOOP:
        return GL_LINE_LOOP;
    case VO::TRIANGLES:
        return GL_TRIANGLES;
    case VO::TRIANGLE_STRIP:
        return GL_TRIANGLE_STRIP;
    case VO::TRIANGLE_FAN:
        return GL_TRIANGLE_FAN;
    default:
        return GL_TRIANGLES;
    }
}

template<typename V>
void draw(Shader &shader, Texture2D *texture, int textureUnit, V &vao, const glm::mat4 &model,
          Texture2D *texture2 = nullptr, Texture2D *texture3 = nullptr, int numInstances = 1,
          VO::TOPOLOGY topology = VO::NONE)
{
    shader.Use();

    if (texture)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        texture->Bind();
        shader.SetInteger("texture1", textureUnit);
    }

    if (texture2)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit + 1);
        texture2->Bind();
        shader.SetInteger("texture2", textureUnit + 1);
    }

    if (texture3)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit + 2);
        texture3->Bind();
        shader.SetInteger("texture3", textureUnit + 2);
    }

    shader.SetMatrix4("model", model);

    if constexpr (is_mat4<V>::value)
    {
        shader.SetMatrix4("transform", vao);
    }
    else if constexpr (is_vec3<V>::value)
    {
        shader.SetVector3f("color", vao);
    }
    else
    {
        vao->bind();
        if (topology != VO::NONE)
        {
            vao->Topology = topology;
        }
        vao->Draw(shader);
        vao->unbind();
    }

    glCheckError(__FILE__, __LINE__);
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
    os << std::fixed << std::setprecision(2);
    for (glm::length_t i = 0; i < R; ++i)
    {
        os << "[";
        for (glm::length_t j = 0; j < C; ++j)
        {
            os << m[j][i];
            if (j < C - 1)
                os << ", ";
        }
        os << "]";
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
inline void decomposeTransformDetails(const glm::mat4 &transform)
{
    glm::vec3 scale, translation, skew;
    glm::vec4 perspective;
    glm::quat rotation;

    glm::decompose(transform, scale, rotation, translation, skew, perspective);

    // Convert quaternion to Euler angles (in degrees)
    glm::vec3 eulerAngles = glm::eulerAngles(rotation);
    eulerAngles = glm::degrees(eulerAngles);

    // Print position, scale, and rotation
    std::cout << "Position: " << translation << "\n";
    std::cout << "Scale: " << scale << "\n";
    std::cout << "Rotation (Euler angles): " << eulerAngles << "\n";
    std::cout << "Rotation (quaternion): " << rotation << "\n";
    std::cout << "Perspective: " << perspective << "\n";
    std::cout << "Skew: " << skew << "\n";
}