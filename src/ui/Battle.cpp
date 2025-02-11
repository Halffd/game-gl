// Battle.cpp
#include "Battle.h"
#include <algorithm>
#include <random>
#include "game/BattleRNG.h"  // Include the header instead of redefining

extern bool debug;  // Make the debug variable accessible

Battle::Battle(std::shared_ptr<GameObject> player, GameObject* enemy, int width, int height)
    : isActive(false)
    , currentState(BattleState::START)
    , stateTimer(0.0f)
    , playerCharacter(player)
    , enemyCharacter(enemy)
    , showMoveSelection(false)
    , selectedMove(0)
    , animationTimer(0.0f)
    , viewportWidth(width)
    , viewportHeight(height)
{
    UpdateViewport(width, height);
}

void Battle::Start() {
    if (debug) std::cout << "\n=== Starting Battle ===" << std::endl;
    
    isActive = true;
    currentState = BattleState::START;
    stateTimer = BATTLE_START_DELAY;
    battleLog.clear();
    showMoveSelection = false;
    
    // Store player's original position
    playerOriginalPosition = playerCharacter->Position;
    
    // Set initial battle positions
    UpdateViewport(viewportWidth, viewportHeight);
    
    // Hide player character
    playerCharacter->isVisible = false;
    
    // Set up battle monster
    if (playerCharacter) {
        // If no monsters, use player as battle monster
        if (playerCharacter->monsters.empty()) {
            battleMonster = playerCharacter;
        } else if (static_cast<size_t>(playerCharacter->form) < playerCharacter->monsters.size()) {
            battleMonster = playerCharacter->monsters[playerCharacter->form];
        }

        if (battleMonster) {
            battleMonster->Position = playerPosition;
            battleMonster->isVisible = true;
            battleMonster->Size = glm::vec2(300.0f, 300.0f);
            
            // Ensure battle monster has moves
            if (battleMonster->moves.empty()) {
                battleMonster->moves = {
                    Move("Tackle", "A basic attack", "Normal", 40, 95.0f, 35),
                    Move("Scratch", "A basic scratch attack", "Normal", 35, 100.0f, 35)
                };
            }
        }
    }
    
    // Set up enemy
    if (enemyCharacter) {
        enemyCharacter->Position = enemyPosition;
        enemyCharacter->isVisible = true;
        enemyCharacter->Size = glm::vec2(300.0f, 300.0f);
        
        // Initialize enemy moves if empty
        if (enemyCharacter->moves.empty()) {
            enemyCharacter->moves = {
                Move("Tackle", "A basic attack", "Normal", 40, 95.0f, 35),
                Move("Scratch", "A basic scratch attack", "Normal", 35, 100.0f, 35),
                Move("Bite", "A biting attack", "Normal", 45, 90.0f, 25)
            };
        }
    }

    if (debug) {
        std::cout << "Battle initialized:" << std::endl;
        std::cout << "- Player monster: " << (battleMonster ? battleMonster->name : "None") << std::endl;
        std::cout << "- Enemy monster: " << (enemyCharacter ? enemyCharacter->name : "None") << std::endl;
        std::cout << "- Player moves: " << (battleMonster ? std::to_string(battleMonster->moves.size()) : "0") << std::endl;
        std::cout << "- Enemy moves: " << (enemyCharacter ? std::to_string(enemyCharacter->moves.size()) : "0") << std::endl;
    }
    
    AddLogMessage("Battle started!");
}

void Battle::Update(float dt) {
    if (!isActive) {
        if (debug) std::cout << "Battle update skipped - battle not active" << std::endl;
        return;
    }

    if (debug) std::cout << "\n--- Battle Update Frame Start ---" << std::endl;
    
    stateTimer -= dt;
    animationTimer += dt;
    
    if (debug) {
        std::cout << "Current battle state: " << static_cast<int>(currentState) << std::endl;
        std::cout << "State timer: " << stateTimer << std::endl;
        std::cout << "Animation timer: " << animationTimer << std::endl;
    }

    UpdateBattleLogic(dt);
    
    if (debug) std::cout << "--- Battle Update Frame End ---\n" << std::endl;
}

void Battle::UpdateBattleLogic([[maybe_unused]] float dt) {
    switch (currentState) {
        case BattleState::START:
            if (stateTimer <= 0.0f) {
                currentState = BattleState::PLAYER_TURN;
                AddLogMessage("Your turn!");
            }
            break;

        case BattleState::PLAYER_TURN:
            HandleStatusEffects(currentState == BattleState::PLAYER_TURN ? playerCharacter->stats : enemyCharacter->stats);
            break;

        case BattleState::ENEMY_TURN:
            HandleStatusEffects(currentState == BattleState::PLAYER_TURN ? playerCharacter->stats : enemyCharacter->stats);
            ExecuteEnemyMove();
            break;

        case BattleState::LOSE:
            playerCharacter->battleEnd = true;
            [[fallthrough]];  // Explicitly indicate fallthrough
            
        case BattleState::WIN:
            if (stateTimer <= 0.0f) {
                isActive = false;
                currentState = BattleState::FINISHED;
                enemyCharacter->stats.health = enemyCharacter->stats.maxHealth;
            }
            break;

        default:
            break;
    }
}


void Battle::Render(SpriteRenderer& renderer) {
    if (!isActive) return;

    RenderBattleScene(renderer);
}
float Battle::GetTypeModifier(const std::string& attackType, const std::string& defenderType) {
    if ((attackType == "Water" && defenderType == "Insect") ||
        (attackType == "Ground" && defenderType == "Water") ||
        (attackType == "Insect" && defenderType == "Ground")) {
        return 1.5f; // Advantage
    } else if ((attackType == "Insect" && defenderType == "Water") ||
               (attackType == "Water" && defenderType == "Ground") ||
               (attackType == "Ground" && defenderType == "Insect")) {
        return 0.5f; // Disadvantage
    }
    return 1.0f; // Neutral
}


void Battle::RenderBattleScene(SpriteRenderer& renderer) {
    // Render battle background (if you have one)
    // renderer.DrawSprite(backgroundTexture, glm::vec2(0.0f), glm::vec2(viewportWidth, viewportHeight));

    // Render enemy
    if (enemyCharacter && enemyCharacter->isVisible) {
        enemyCharacter->Position = enemyPosition;
        enemyCharacter->Size = glm::vec2(300.0f, 300.0f);
        enemyCharacter->Draw(renderer);
    }

    // Render player's monster
    if (battleMonster && battleMonster->isVisible) {
        battleMonster->Position = playerPosition;
        battleMonster->Mirror = true;
        battleMonster->Draw(renderer);
        battleMonster->Mirror = false;
    }
}
void Battle::RenderUI() {
    if (!isActive) return;

    // Main battle menu
    ImVec2 menuPos = getScreenPos(UILayout::BATTLE_MENU_X, UILayout::BATTLE_MENU_Y);
    ImVec2 menuSize(UILayout::BATTLE_MENU_WIDTH, UILayout::BATTLE_MENU_HEIGHT);
    
    ImGui::SetNextWindowPos(menuPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(menuSize, ImGuiCond_Always);
    ImGui::Begin("Battle", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove);

    // Render main action buttons
    if (ImGui::Button("Attack", ImVec2(200, 50))) {
        showMoveSelection = true;
    }
    /*if (ImGui::Button("Item", ImVec2(200, 50))) {
        AddLogMessage("You selected Item. (Feature WIP)");
    }
    if (ImGui::Button("Monsters", ImVec2(200, 50))) {
        AddLogMessage("You selected Monsters. (Feature WIP)");
    }*/
    
    // Function to execute the "Run" action with 40% chance
    if (ImGui::Button("Run", ImVec2(200, 50))) {
        // Create a random device and generator for the probability check
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 100);  // Generate random number between 1 and 100

        int chance = dis(gen);  // Random number between 1 and 100

        if (chance <= 40) {  // 40% chance to run away
            currentState = BattleState::LOSE;  // Set the game state to LOST or equivalent
            AddLogMessage("You ran away successfully!");
            End();  // End the battle or transition to another state
        } else {
            AddLogMessage("You failed to run away!");  // If the chance is not met, player fails to run away
            currentState = BattleState::ENEMY_TURN;
        }
    }

    ImGui::End();

    // Health bars
    RenderHealthBars();
    RenderBattleLog();

    if (showMoveSelection && currentState == BattleState::PLAYER_TURN) {
        RenderMoveSelection();
    }
}
void Battle::RenderBattleLog() {
    ImVec2 logPos = getScreenPos(UILayout::LOG_X, UILayout::LOG_Y);
    ImVec2 logSize = getScreenSize(UILayout::LOG_WIDTH, UILayout::LOG_HEIGHT * viewportHeight);
    
    ImGui::SetNextWindowPos(logPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(logSize, ImGuiCond_Always);
    
    // Create a child window with a scrollable area for the battle log
    ImGui::Begin("Battle Log", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

    ImGui::BeginChild("BattleLogChild", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
    
    for (const auto& message : battleLog) {
        ImGui::TextWrapped("%s", message.c_str());
    }

    ImGui::EndChild();
    ImGui::End();
}

void Battle::RenderHealthBars() {
    // Enemy health bar
    ImVec2 enemyHealthPos = getScreenPos(0.0f, UILayout::ENEMY_HEALTH_Y);
    ImVec2 healthBarSize = getScreenSize(UILayout::HEALTH_BAR_WIDTH, UILayout::HEALTH_BAR_HEIGHT);
    
    ImGui::SetNextWindowPos(enemyHealthPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(healthBarSize, ImGuiCond_Always);
    ImGui::Begin("Enemy Health", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoBackground);

    ImGui::Text("%s: %s", enemyCharacter->name.c_str(), std::to_string(enemyCharacter->stats.health).c_str());
    ImGui::ProgressBar((float)enemyCharacter->stats.health / enemyCharacter->stats.maxHealth, ImVec2(-1, 0));
    
    ImGui::End();
    
    // Player health bar
    ImVec2 playerHealthPos = getScreenPos(0.0f, UILayout::PLAYER_HEALTH_Y);
    ImGui::SetNextWindowPos(playerHealthPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(healthBarSize, ImGuiCond_Always);
    
    ImGui::Begin("Player Health", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

    ImGui::Text("%s: %s", playerCharacter->name.c_str(), std::to_string(playerCharacter->stats.health).c_str());
    ImGui::ProgressBar((float)playerCharacter->stats.health / playerCharacter->stats.maxHealth, ImVec2(-1, 0));

    ImGui::End();
}

void Battle::RenderMoveSelection() {
    if (!battleMonster) {
        if (debug) std::cout << "Warning: No battle monster available" << std::endl;
        return;
    }

    if (battleMonster->moves.empty()) {
        if (debug) std::cout << "Warning: Battle monster has no moves" << std::endl;
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(viewportWidth - 700, viewportHeight - 280), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(250, 280), ImGuiCond_Always);
    ImGui::Begin("MoveSelection", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    ImGui::Text("Select your move:");
    for (const auto& move : battleMonster->moves) {
        if (ImGui::Button(move.name.c_str(), ImVec2(200, 50))) {
            ExecutePlayerMove(move.name);
            showMoveSelection = false;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s\nType: %s\nPower: %d\nAccuracy: %.0f%%\nPP: %d", 
                            move.description.c_str(), move.type.c_str(), 
                            move.power, move.accuracy, move.quantity);
        }
    }
    ImGui::End();
}

void Battle::ExecutePlayerMove(const std::string& moveName) {
    if (debug) {
        std::cout << "\n=== Executing Player Move ===" << std::endl;
        std::cout << "Move selected: " << moveName << std::endl;
    }
    
    auto move = std::find_if(playerCharacter->moves.begin(), playerCharacter->moves.end(),
        [&moveName](const Move& m) { return m.name == moveName; });

    if (move != playerCharacter->moves.end()) {
        if (debug) {
            std::cout << "Move details:" << std::endl;
            std::cout << "- Power: " << move->power << std::endl;
            std::cout << "- Accuracy: " << move->accuracy << std::endl;
            std::cout << "- Type: " << move->type << std::endl;
        }
        
        if (CheckAccuracy(move->accuracy)) {
            int damage = CalculateDamage(*move, playerCharacter->stats, enemyCharacter->stats);
            if (debug) std::cout << "Calculated damage: " << damage << std::endl;
            
            enemyCharacter->stats.health = std::max(0, enemyCharacter->stats.health - damage);
            if (debug) std::cout << "Enemy health after damage: " << enemyCharacter->stats.health << std::endl;
            
            AddLogMessage(playerCharacter->stats.name + " used " + moveName + "!");
            AddLogMessage("Dealt " + std::to_string(damage) + " damage!");
        } else {
            if (debug) std::cout << "Move missed!" << std::endl;
            AddLogMessage(moveName + " missed!");
        }
    } else {
        if (debug) std::cout << "ERROR: Move not found in player's move list!" << std::endl;
    }
    
    if (debug) std::cout << "=== Player Move Execution Complete ===\n" << std::endl;

    // Check for battle end
    if (enemyCharacter->stats.health <= 0) {
        currentState = BattleState::WIN;
        stateTimer = BATTLE_START_DELAY;
        AddLogMessage("You won the battle!");
        playerCharacter->level++;
        std::random_device rd;  // Obtain a random number from hardware
        std::mt19937 eng(rd()); // Seed the generator

        // Random number distribution for stat increase
        std::uniform_int_distribution<> healthDist(0, 40);
        std::uniform_int_distribution<> attackDist(0, 8);
        std::uniform_int_distribution<> defenseDist(0, 8);
        std::uniform_int_distribution<> speedDist(0, 8);
        int health = healthDist(eng);
        // Increase stats with some randomness
        playerCharacter->stats.maxHealth += health;
        playerCharacter->stats.health = playerCharacter->stats.maxHealth;
        playerCharacter->stats.attack += attackDist(eng);
        playerCharacter->stats.defense += defenseDist(eng);
        playerCharacter->stats.speed += speedDist(eng);

        playerCharacter->won = true;
        return;
    }

    // Switch to enemy turn
    currentState = BattleState::ENEMY_TURN;
    stateTimer = BATTLE_START_DELAY;
}


void Battle::HandleStatusEffects(BattleStats& stats) {
    switch (stats.status) {
        case StatusEffect::POISON:
            stats.health -= 5; // Fixed damage per turn
            AddLogMessage(stats.name + " is hurt by poison!");
            break;
        case StatusEffect::PARALYSIS:
            if (std::rand() % 100 < 25) { // 25% chance to skip turn
                AddLogMessage(stats.name + " is paralyzed and cannot move!");
            }
            break;
        case StatusEffect::BURN:
            stats.attack -= 2; // Reduce attack while burned
            stats.health -= 5; // Fixed damage per turn
            AddLogMessage(stats.name + " is hurt by the burn!");
            break;
        default:
            break;
    }
}
void Battle::ApplyStatusEffect(StatusEffect effect, bool isPlayer) {
    if (isPlayer) {
        playerCharacter->stats.status = effect;
        AddLogMessage(playerCharacter->stats.name + " is now " + StatusEffectToString(effect) + "!");
    } else {
        enemyCharacter->stats.status = effect;
        AddLogMessage(enemyCharacter->stats.name + " is now " + StatusEffectToString(effect) + "!");
    }
}
std::string Battle::StatusEffectToString(StatusEffect effect) {
    switch (effect) {
        case StatusEffect::NONE: return "Normal";
        case StatusEffect::POISON: return "Poisoned";
        case StatusEffect::PARALYSIS: return "Paralyzed";
        case StatusEffect::BURN: return "Burned";
        default: return "Unknown";
    }
}

bool Battle::CheckCriticalHit() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 100.0);
    return dis(gen) < 15; // e.g., 10%
}

int Battle::CalculateDamage(const Move& move, const BattleStats& attacker, const BattleStats& defender) {
    float attackPower = (float)move.power * ((float)attacker.attack / defender.defense);
    float randomFactor = (std::rand() % 15 + 85) / 100.0f; // 0.85 to 1.00
    int damage = static_cast<int>(attackPower * randomFactor);

    if (CheckCriticalHit()) {
        AddLogMessage("Critical hit!");
        damage *= 2; // Double damage
    }

    return damage;
}

void Battle::ExecuteEnemyMove() {
    if (!enemyCharacter || enemyCharacter->moves.empty()) {
        if (debug) std::cout << "Warning: Enemy has no moves or is invalid" << std::endl;
        currentState = BattleState::PLAYER_TURN;
        return;
    }

    // Calculate move probabilities
    std::vector<float> moveWeights;
    moveWeights.reserve(enemyCharacter->moves.size());

    for (const auto& move : enemyCharacter->moves) {
        float weight = move.accuracy;
        if (move.quantity <= 0) {
            weight = 0.0f;
        } else if (playerCharacter->stats.health < playerCharacter->stats.maxHealth * 0.5f) {
            weight *= 1.2f;
        }
        weight *= GetTypeModifier(move.type, playerCharacter->stats.type);
        moveWeights.push_back(weight);
    }

    // Select move
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::discrete_distribution<size_t> dist(moveWeights.begin(), moveWeights.end());
    size_t selectedIndex = dist(gen);

    if (selectedIndex >= enemyCharacter->moves.size()) {
        currentState = BattleState::PLAYER_TURN;
        return;
    }

    const Move& selectedMove = enemyCharacter->moves[selectedIndex];
    
    if (debug) {
        std::cout << "\nEnemy selecting move:" << std::endl;
        std::cout << "- Selected index: " << selectedIndex << std::endl;
        std::cout << "- Move name: " << selectedMove.name << std::endl;
        std::cout << "- Accuracy: " << selectedMove.accuracy << std::endl;
    }

    // Execute the selected move
    if (CheckAccuracy(selectedMove.accuracy)) {
        int damage = CalculateDamage(selectedMove, enemyCharacter->stats, playerCharacter->stats);
        
        if (debug) std::cout << "Calculated damage: " << damage << std::endl;
        
        playerCharacter->stats.health = std::max(0, playerCharacter->stats.health - damage);
        
        AddLogMessage(enemyCharacter->name + " used " + selectedMove.name + "!");
        AddLogMessage("Dealt " + std::to_string(damage) + " damage!");
        
        if (playerCharacter->stats.health <= 0) {
            currentState = BattleState::LOSE;
            stateTimer = 2.0f;
            AddLogMessage("You lost the battle!");
        }
    } else {
        AddLogMessage(enemyCharacter->name + " used " + selectedMove.name + " but missed!");
    }

    currentState = BattleState::PLAYER_TURN;
}

void Battle::AddLogMessage(const std::string& message) {
    battleLog.push_back(message);
    if (battleLog.size() > MAX_LOG_ENTRIES) {
        battleLog.erase(battleLog.begin());
    }
}

bool Battle::CheckAccuracy(float accuracy) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 100.0);
    return dis(gen) < accuracy;
}

void Battle::End() {
    isActive = false;
    
    // Restore original game state
    if (playerCharacter) {
        playerCharacter->isVisible = true;
        playerCharacter->Position = playerOriginalPosition;
    }
    
    if (battleMonster) {
        battleMonster->isVisible = false;
    }
    
    if (enemyCharacter) {
        enemyCharacter->isVisible = false;
    }
}