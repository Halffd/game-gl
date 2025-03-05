// Battle.h
#ifndef BATTLE_H
#define BATTLE_H

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "Gui.h"
#include "render/SpriteRenderer.h"
#include "game/Player.h"
#include "game/GameObject.h"
#include "game/BattleRNG.h"

extern bool debug;  // Declare the debug variable

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

    void UpdateViewport(int width, int height) {
        viewportWidth = width;
        viewportHeight = height;
        
        // Update positions
        playerPosition = glm::vec2(
            width * UILayout::PLAYER_X,
            height * UILayout::PLAYER_Y
        );
        enemyPosition = glm::vec2(
            width * UILayout::ENEMY_X,
            height * UILayout::ENEMY_Y
        );
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
    bool CheckAccuracy(float accuracy);  // Just declare it

    // UI Layout constants (as percentages of screen)
    struct UILayout {
        static constexpr float PLAYER_X = 0.15f;  // 15% from left
        static constexpr float PLAYER_Y = 0.65f;  // 65% from bottom
        static constexpr float ENEMY_X = 0.75f;   // 75% from left
        static constexpr float ENEMY_Y = 0.20f;   // 20% from top
        
        // Menu positions
        static constexpr float BATTLE_MENU_X = 0.80f;
        static constexpr float BATTLE_MENU_Y = 0.80f;
        static constexpr float BATTLE_MENU_WIDTH = 220.0f;
        static constexpr float BATTLE_MENU_HEIGHT = 220.0f;
        
        // Health bars
        static constexpr float HEALTH_BAR_WIDTH = 0.4f;  // 40% of screen width
        static constexpr float HEALTH_BAR_HEIGHT = 80.0f;
        static constexpr float PLAYER_HEALTH_Y = 0.95f;  // 95% from top
        static constexpr float ENEMY_HEALTH_Y = 0.05f;   // 5% from top
        
        // Battle log
        static constexpr float LOG_X = 0.35f;
        static constexpr float LOG_Y = 0.80f;
        static constexpr float LOG_WIDTH = 0.4f;
        static constexpr float LOG_HEIGHT = 0.15f;
    };

    // Screen dimensions
    int viewportWidth;
    int viewportHeight;

    // Helper to get actual positions
    ImVec2 getScreenPos(float relX, float relY) const {
        return ImVec2(viewportWidth * relX, viewportHeight * relY);
    }

    ImVec2 getScreenSize(float relWidth, float height) const {
        return ImVec2(viewportWidth * relWidth, height);
    }

    // Store original positions
    glm::vec2 playerOriginalPosition;
    std::shared_ptr<GameObject> battleMonster;
};

#endif // BATTLE_H