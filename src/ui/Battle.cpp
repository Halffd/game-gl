// Battle.cpp
#include "Battle.h"
#include <algorithm>
#include <random>

Battle::Battle(std::shared_ptr<GameObject> player, GameObject* enemy, int width, int height)
    : isActive(false)
    , currentState(BattleState::START)
    , stateTimer(0.0f)
    , playerCharacter(player)
    , enemyCharacter(enemy)
    , showMoveSelection(false)
    , selectedMove(0)
    , animationTimer(0.0f)
    , Width(width)
    , Height(height)
{
    // Set initial positions
    playerPosition = glm::vec2(70.0f, Height - 470.0f);  // Adjust these values based on your screen
    enemyPosition = glm::vec2(Width - 400.0f, 80.0f);   // Adjust these values based on your screen
}

void Battle::Start() {
    isActive = true;
    currentState = BattleState::START;
    stateTimer = BATTLE_START_DELAY;
    battleLog.clear();
    AddLogMessage("Battle started!");
}

void Battle::Update(float dt) {
    if (!isActive) return;

    stateTimer -= dt;
    animationTimer += dt;

    UpdateBattleLogic(dt);
}

void Battle::UpdateBattleLogic(float dt) {
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
    // Render battle background
    // renderer.DrawSprite(backgroundTexture, glm::vec2(0.0f), glm::vec2(800.0f, 600.0f));

    // Render enemy
    enemyCharacter->Position = enemyPosition;
    enemyCharacter->Size = glm::vec2(300.0f, 300.0f);
    enemyCharacter->Rotation = 180.0f;
    enemyCharacter->Draw(renderer);

    // Render player
    playerCharacter->Position = playerPosition;
    playerCharacter->Rotation = 180.0f;
    playerCharacter->Draw(renderer);
}
void Battle::RenderUI() {
    if (!isActive) return;

    // Main battle menu
    ImVec2 size(220, 220);
    ImGui::SetNextWindowPos(ImVec2(Width-size.x, Height - size.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    ImGui::Begin("Battle", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Render main action buttons
    if (ImGui::Button("Attack", ImVec2(200, 50))) {
        showMoveSelection = true;
    }
    if (ImGui::Button("Item", ImVec2(200, 50))) {
        AddLogMessage("You selected Item. (Feature WIP)");
    }
    if (ImGui::Button("Monsters", ImVec2(200, 50))) {
        AddLogMessage("You selected Monsters. (Feature WIP)");
    }
    
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
        }
    }

    ImGui::End();

    // Render player health and stats
    RenderHealthBars();
    RenderBattleLog();

    // Render move selection if active
    if (showMoveSelection && currentState == BattleState::PLAYER_TURN) {
        RenderMoveSelection();
    }
}
void Battle::RenderBattleLog() {
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(620, screenSize.y - 340), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(Width - 920, 305), ImGuiCond_Always);
    ImGui::Begin("Battle Log", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

    // Create a child window with a scrollable area for the battle log
    ImGui::BeginChild("BattleLogChild", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
    
    for (const auto& message : battleLog) {
        ImGui::TextWrapped("%s", message.c_str());
    }

    ImGui::EndChild();
    ImGui::End();
}

void Battle::RenderHealthBars() {
    // Enemy health bar
    ImGui::SetNextWindowPos(ImVec2(Width - 800, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(800, 80), ImGuiCond_Always);
    ImGui::Begin("Enemy", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

    ImGui::Text("%s: %s", enemyCharacter->name.c_str(), std::to_string(enemyCharacter->stats.health).c_str());
    ImGui::ProgressBar((float)enemyCharacter->stats.health / enemyCharacter->stats.maxHealth, ImVec2(-1, 0));
    
    ImGui::End();
    
    // Player health bar
    ImGui::SetNextWindowPos(ImVec2(0, Height - 80), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(800, 80), ImGuiCond_Always);
    ImGui::Begin("Player", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

    ImGui::Text("%s: %s", playerCharacter->name.c_str(), std::to_string(playerCharacter->stats.health).c_str());
    ImGui::ProgressBar((float)playerCharacter->stats.health / playerCharacter->stats.maxHealth, ImVec2(-1, 0));

    ImGui::End();
}

void Battle::RenderMoveSelection() {
    ImGui::SetNextWindowPos(ImVec2(Width - 700, Height - 280), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(250, 280), ImGuiCond_Always);
    ImGui::Begin("MoveSelection", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    ImGui::Text("Select your move:");
    for (size_t i = 0; i < playerCharacter->moves.size(); i++) {
        const auto& attack = playerCharacter->moves[i];
        if (ImGui::Button(attack.name.c_str(), ImVec2(200, 50))) {
            ExecutePlayerMove(attack.name);
            showMoveSelection = false;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s\nType: %s\nPower: %d\nAccuracy: %.0f%%\nLimit: %d\n", 
                              attack.description.c_str(), attack.type.c_str(), attack.power, 
                              attack.accuracy, attack.quantity);
        }
    }
    ImGui::End();
}

void Battle::ExecutePlayerMove(const std::string& moveName) {
    auto move = std::find_if(playerCharacter->moves.begin(), playerCharacter->moves.end(),
        [&moveName](const Move& m) { return m.name == moveName; });

    if (move != playerCharacter->moves.end()) {
        if (CheckAccuracy(move->accuracy)) {
            int damage = CalculateDamage(*move, playerCharacter->stats, enemyCharacter->stats);
            enemyCharacter->stats.health = std::max(0, enemyCharacter->stats.health - damage);
            AddLogMessage(playerCharacter->stats.name + " used " + moveName + "!");
            AddLogMessage("Dealt " + std::to_string(damage) + " damage!");
        } else {
            AddLogMessage(moveName + " missed!");
        }
    }

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
    std::random_device rd;
    std::mt19937 gen(rd());

    // Add weights for moves based on health and strategy
    std::vector<float> weights; // Vector to store the weights of the moves
    
    // Loop through the enemy's available moves
    for (size_t i = 0; i < enemyCharacter->moves.size(); ++i) {
        const auto& move = enemyCharacter->moves[i];
        float weight = move.accuracy;  // Base weight on accuracy

        // Add bonus weight if the move's power is high and enemy health is above 50%
        if (enemyCharacter->stats.health > 0.5f * enemyCharacter->stats.maxHealth && move.power > 50) {
            weight += 20.0f;
        }

        // Adjust weight based on type effectiveness (attack vs. defender type)
        float typeModifier = GetTypeModifier(move.type, playerCharacter->stats.type);
        weight *= typeModifier;

        // Add the weight to the weights vector
        weights.push_back(weight);
    }

    // Select a move based on weighted random choice using the weights vector
    std::discrete_distribution<> dis(weights.begin(), weights.end()); // Correct constructor usage

    // Select the move based on the distribution
    const Move& selectedMove = enemyCharacter->moves[dis(gen)];

    // Execute the move if it hits
    if (CheckAccuracy(selectedMove.accuracy)) {
        int damage = CalculateDamage(selectedMove, enemyCharacter->stats, playerCharacter->stats);
        playerCharacter->stats.health = std::max(0, playerCharacter->stats.health - damage);
        AddLogMessage(enemyCharacter->name + " used " + selectedMove.name + "!");
        AddLogMessage("Dealt " + std::to_string(damage) + " damage!");
    } else {
        AddLogMessage(enemyCharacter->name + "'s " + selectedMove.name + " missed!");
    }

    // Check for battle end if player's health drops to zero
    if (playerCharacter->stats.health <= 0) {
        currentState = BattleState::LOSE;
        stateTimer = BATTLE_START_DELAY;
        AddLogMessage("You lost the battle!");
        printf("You lost the battle!\n");
    }
}

void Battle::AddLogMessage(const std::string& message) {
    battleLog.push_back(message);
    if (battleLog.size() > MAX_LOG_ENTRIES) {
        battleLog.erase(battleLog.begin());
    }
}

bool Battle::CheckAccuracy(float accuracy) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 100.0);
    return dis(gen) < accuracy;
}

void Battle::End() {
    isActive = false;
}