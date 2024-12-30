#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "render/SpriteRenderer.h"

// Attack structure for managing individual attacks
struct Move {
    std::string name;
    std::string type; // e.g., "insect", "water", "ground"
    int power;
    float accuracy;
    int quantity;
    std::string description;
};
enum class BattleState {
    START,
    PLAYER_TURN,
    ENEMY_TURN,
    WIN,
    LOSE,
    FINISHED
};

enum class StatusEffect { NONE, POISON, PARALYSIS, BURN };
struct BattleStats {
    int health;
    int maxHealth;
    int attack;
    int defense;
    int speed;
    std::string type; 
    std::string name;
    StatusEffect status = StatusEffect::NONE;
};
// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
    // object state
    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    float       Rotation;

    // render state
    Texture2D   Sprite;	

    bool        IsSolid;
    bool        Destroyed;
    int         interactable;
    int         dialogueID;
    std::string name;
    int         level;
    int         exp;
    int         form;
    std::string type;
    std::vector<Move> moves;
    BattleStats stats;
    bool battleEnd = false;
    bool won = false;
    bool lost = false;

    // constructor(s)
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    // draw sprite
    virtual void Draw(SpriteRenderer &renderer);
};

#endif
