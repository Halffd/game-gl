#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObject.h"

class CircleObject : public GameObject
{
public:
    // ball state	
    float   Radius;
    // constructor(s)
    CircleObject();
    CircleObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);
    // moves the ball, keeping it constrained within the window bounds (except bottom edge); returns new position
    glm::vec2 Move(float dt, unsigned int window_width);
    // resets the ball to original state with given position and velocity
    void      Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif
