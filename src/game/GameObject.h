#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include <memory>
#include "Move.h"
#include "render/SpriteRenderer.h"

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
    bool        Mirror = false;

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

    bool isVisible = true;  // Add visibility flag
    std::vector<std::shared_ptr<GameObject>> monsters;  // Add monsters container

    // constructor(s)
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    // draw sprite
    virtual void Draw(SpriteRenderer &renderer);
};

#endif
