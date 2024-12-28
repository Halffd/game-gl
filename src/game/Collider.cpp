#include "Collider.h"
#include <tuple>

// Corrected function name
Collider::Direction Collider::VectorDirection(glm::vec2 target) {
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++) {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

bool Collider::CheckCollision(GameObject &one, GameObject &two) { 
    // AABB - AABB collision
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                       two.Position.x + two.Size.x >= one.Position.x;
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                       two.Position.y + two.Size.y >= one.Position.y;
    return collisionX && collisionY; // collision only if on both axes
}

Collider::Collision Collider::CheckCollision(BallObject &one, GameObject &two) {
    // Get center point of the circle
    glm::vec2 center(one.Position + glm::vec2(one.Radius)); // Corrected center calculation
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position + aabb_half_extents);
    
    // Difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    
    // Closest point on AABB to the circle
    glm::vec2 closest = aabb_center + clamped;
    difference = closest - center;

    if (glm::length(difference) < one.Radius) {
        return std::make_tuple(true, VectorDirection(difference), difference);
    } else {
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
    }
}

void Collider::Collisions() {
    for (GameObject &box : this->Levels[this->Level].Bricks) {
        if (!box.Destroyed) {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) { // If collision is true
                // Destroy block if not solid
                if (!box.IsSolid) {
                    box.Destroyed = true;
                    audio->play2D((ResourceManager::root + "/audio/solid.wav").c_str());
                } else {
                    audio->play2D((ResourceManager::root + "/audio/bleep.wav").c_str());
                }

                // Collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                float penetration;

                if (dir == LEFT || dir == RIGHT) { // Horizontal collision
                    Ball->Velocity.y = -abs(Ball->Velocity.y); // Change direction

                    // Relocate
                    penetration = Ball->Radius - std::abs(diff_vector.x;
                    Ball->Position.x += (dir == LEFT) ? penetration : -penetration; // Move ball
                } else { // Vertical collision
                    Ball->Velocity.y = -Ball->Velocity.y; // Reverse vertical velocity

                    // Relocate
                    penetration = Ball->Radius - std::abs(diff_vector.y);
                    Ball->Position.y += (dir == UP) ? -penetration : penetration; // Move ball
                }
            }
        }    
    }
    
    // Check collisions for player paddle (unless stuck)
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result)) {
        // Calculate hit position and change velocity accordingly
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        
        // Adjust velocity based on where it hit the paddle
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // Maintain speed
        Ball->Velocity.y = -abs(Ball->Velocity.y); // Ensure upward movement
    }
}