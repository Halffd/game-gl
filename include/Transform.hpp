#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glad/glad.h>
#include "Vertex.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// Function to draw an object with a given transformation
void draw(const Shader& shader, Texture& texture, const VAO& vao, const glm::mat4& transform)
{
    shader.setMat4("model", transform);
    texture.Activate(GL_TEXTURE0);
    vao.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

// Function to set up the transformation for an object
glm::mat4 setupTransformation(float translateX, float translateY, float scaleX, float scaleY, float rotationAngle = 0.0f, glm::vec3 rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f))
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
glm::mat4 transform(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    if (glm::length(rotation) != 0.0f)
    {
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    model = glm::scale(model, scale);
    return model;
}
#endif