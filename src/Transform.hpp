#ifndef TRANSFORM_H
#define TRANSFORM_H

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
#include <glm/gtx/quaternion.hpp> // Ensure this is included

template<typename V>
void draw(Shader &shader, V &vao, const glm::mat4 &transform, int vertices = -1, VO::TOPOLOGY topology = VO::NONE) {
    draw(shader, nullptr, 0, vao, transform, nullptr, nullptr, vertices, topology);
}
template<typename V>
void draw(Shader &shader, Texture2D* textures, int texturesN, V &vao, const glm::mat4 &transform, Texture2D* diffuse = nullptr, Texture2D* specular = nullptr, int vertices = -1, VO::TOPOLOGY topology = VO::NONE)
{
    shader.Use();
    shader.SetMatrix4("model", transform);

    // Bind textures
    if(texturesN > 1){
        if(textures != nullptr) {
            for (int i = 0; i < texturesN; ++i) {
                textures[i].Bind();             // Activate texture unit
                shader.SetInteger(("texture" + std::to_string(i + 1)).c_str(), i); // Set the uniform for the texture
            }
        }
    } else if(texturesN == 1){
        if(textures != nullptr) {
            glActiveTexture(GL_TEXTURE0 + textures->ID);
            textures->Bind();             // Activate texture unit
            shader.SetInteger("texture1", textures->ID); // Set the uniform for the texture
        }
    }
    if(diffuse != nullptr) {
        glActiveTexture(GL_TEXTURE0 + diffuse->ID);
        diffuse->Bind();
        shader.SetInteger("material.diffuse", diffuse->ID);
    }
    if(specular != nullptr) {
        glActiveTexture(GL_TEXTURE0 + specular->ID);
        specular->Bind();
        shader.SetInteger("material.specular", specular->ID);
    }
    int ind = vao->bind();
    unsigned int drawTopology = (topology != VO::NONE) ? topology : getPrimitive(vao->Topology);
        //lo << "Draw topology: " << drawTopology;
        //lo << "Vertex count: " << (vertices > 0 ? vertices : vao.vertexCount);
    if (ind > 0)
    {
        glDrawElements(drawTopology, (vertices > 0 ? vertices : vao->vertexCount), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(drawTopology, 0, (vertices > 0 ? vertices : vao->vertexCount));
    }
    glCheckError(__FILE__, __LINE__);
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

template <typename T>
T clamp(T value, T min, T max) {
    return std::min(std::max(value, min), max);
}
// Function to scale the output of a given operation within specified ranges
float clamp(const float value, float (*operation)(float),
            float min = 0.1f, float max = 1.0f,
            float minOpRange=-1.0f, float maxOpRange=1.0f) {
    // Apply the operation
    float opValue = operation(value);

    // Scale the operation output from [minOpRange, maxOpRange] to [min, max]
    float scaledValue = min + (opValue - minOpRange) / (maxOpRange - minOpRange) * (max - min);

    // Clamp the final value
    return clamp(scaledValue, min, max);
}
float clampSin(const float value, float (*operation)(float), const float min = 0.1f, const float max = 1.0f) {
    return clamp(min + (operation(value) + 1.0f) * 0.5f * (max - min), min, max);
}
float lerp(float a, float b, float f)
{
    return a + f * (b - a);
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
    std::cout << "Position: " << translation << "\n";
    std::cout << "Scale: " << scale << "\n";
    std::cout << "Rotation (Euler angles): " << eulerAngles << "\n";
    std::cout << "Rotation (quaternion): " << rotation << "\n";
    std::cout << "Perspective: " << perspective << "\n";
    std::cout << "Skew: " << skew << "\n";
}

#endif