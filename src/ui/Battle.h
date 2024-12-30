// Battle.h
#ifndef BATTLE_H
#define BATTLE_H

#include <vector>
#include <memory>
#include <string>
#include "Gui.h"
#include "render/SpriteRenderer.h"
#include "game/Player.h"
#include "game/GameObject.h"

enum class BattleState {
    START,
    PLAYER_TURN,
    ENEMY_TURN,
    WIN,
    LOSE,
    FINISHED
};

struct BattleStats {
    int health;
    int maxHealth;
    int attack;
    int defense;
    int speed;
    std::string name;
};

class Battle {
public:
    Battle(std::shared_ptr<Player> player, GameObject* enemy);
    ~Battle() = default;

    void Update(float dt);
    void Render(SpriteRenderer& renderer);
    void RenderUI();
    
    bool IsActive() const { return isActive; }
    void Start();
    void End();

private:
    void UpdateBattleLogic(float dt);
    void RenderBattleScene(SpriteRenderer& renderer);
    void RenderBattleMenu();
    void RenderHealthBars();
    void RenderMoveSelection();
    void RenderBattleLog();
    void ExecutePlayerMove(const std::string& move);
    void ExecuteEnemyMove();
    
    // Battle properties
    bool isActive;
    BattleState currentState;
    float stateTimer;
    std::vector<std::string> battleLog;
    
    // Combatants
    std::shared_ptr<Player> playerCharacter;
    GameObject* enemyCharacter;
    BattleStats playerStats;
    BattleStats enemyStats;
    
    // Move system
    struct Move {
        std::string name;
        int power;
        float accuracy;
        std::string description;
    };
    std::vector<Move> playerMoves;
    std::vector<Move> enemyMoves;
    
    // UI state
    bool showMoveSelection;
    int selectedMove;
    float animationTimer;
    glm::vec2 playerPosition;
    glm::vec2 enemyPosition;
    
    // Constants
    static constexpr float BATTLE_START_DELAY = 1.0f;
    static constexpr float MOVE_ANIMATION_DURATION = 0.5f;
    static constexpr int MAX_LOG_ENTRIES = 5;
    
    // Helper functions
    void AddLogMessage(const std::string& message);
    int CalculateDamage(const Move& move, const BattleStats& attacker, const BattleStats& defender);
    bool CheckAccuracy(float accuracy);
};

#endif // BATTLE_H