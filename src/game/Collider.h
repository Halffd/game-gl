#include <tuple>
#include "GameObject.h:

class Collider
{
public:
    enum Direction {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };    
    typedef std::tuple<bool, Direction, glm::vec2> Collision;    
    Direction VectorDirection(glm::vec2 target);
    void Collisions();
    bool CheckCollision(GameObject &one, GameObject &two);
    Collision CheckCollision(BallObject &one, GameObject &two);
}