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

class Battle {
public:
    Battle(std::shared_ptr<GameObject> player, GameObject* enemy, int width, int height);
    ~Battle() = default;

    void Update(float dt);
    void Render(SpriteRenderer& renderer);
    void RenderUI();
    
    bool IsActive() const { return isActive; }
    void Start();
    void End();
    std::string StatusEffectToString(StatusEffect effect);
    int GetEnemyHealth() const {
        return enemyCharacter ? enemyCharacter->stats.health : 0;
    }

private:
    void ApplyStatusEffect(StatusEffect effect, bool isPlayer);
    void HandleStatusEffects(BattleStats& stats);
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
    std::shared_ptr<GameObject> playerCharacter;
    GameObject* enemyCharacter;
    BattleStats playerStats;
    BattleStats enemyStats;
    
    // UI state
    bool showMoveSelection;
    int selectedMove;
    float animationTimer;
    glm::vec2 playerPosition;
    glm::vec2 enemyPosition;
    int Width;
    int Height;
    
    // Constants
    static constexpr float BATTLE_START_DELAY = 2.0f;
    static constexpr float MOVE_ANIMATION_DURATION = 0.5f;
    static constexpr int MAX_LOG_ENTRIES = 10;
    float GetTypeModifier(const std::string& attackType, const std::string& defenderType);
    bool CheckCriticalHit();
    // Helper functions
    void AddLogMessage(const std::string& message);
    int CalculateDamage(const Move& move, const BattleStats& attacker, const BattleStats& defender);
    bool CheckAccuracy(float accuracy);
};

#endif // BATTLE_H