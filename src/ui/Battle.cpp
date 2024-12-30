// Battle.cpp
#include "Battle.h"
#include <algorithm>
#include <random>

Battle::Battle(std::shared_ptr<GameObject> player, GameObject* enemy)
    : isActive(false)
    , currentState(BattleState::START)
    , stateTimer(0.0f)
    , playerCharacter(player)
    , enemyCharacter(enemy)
    , showMoveSelection(false)
    , selectedMove(0)
    , animationTimer(0.0f)
{
    // Initialize player stats
    playerStats = {
        100,    // health
        100,    // maxHealth
        50,     // attack
        40,     // defense
        45,     // speed
        player->name
    };

    // Initialize enemy stats
    enemyStats = {
        80,     // health
        80,     // maxHealth
        45,     // attack
        35,     // defense
        40,     // speed
        enemy->name
    };

    // Initialize player moves
    playerMoves = {
        {"Attack", 50, 95.0f, "A basic attack"},
        {"Power Strike", 80, 75.0f, "A powerful but less accurate strike"},
        {"Quick Attack", 40, 100.0f, "A fast, accurate attack"},
        {"Special Move", 100, 65.0f, "A very powerful but risky attack"}
    };

    // Initialize enemy moves
    enemyMoves = {
        {"Attack", 45, 95.0f, "A basic attack"},
        {"Charge", 70, 80.0f, "A charging attack"}
    };

    // Set initial positions
    playerPosition = glm::vec2(200.0f, 300.0f);  // Adjust these values based on your screen
    enemyPosition = glm::vec2(600.0f, 150.0f);   // Adjust these values based on your screen
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
                showMoveSelection = true;
                AddLogMessage("Your turn!");
            }
            break;

        case BattleState::ENEMY_TURN:
            if (stateTimer <= 0.0f) {
                ExecuteEnemyMove();
                currentState = BattleState::PLAYER_TURN;
                showMoveSelection = true;
                AddLogMessage("Your turn!");
            }
            break;

        case BattleState::WIN:
        case BattleState::LOSE:
            if (stateTimer <= 0.0f) {
                currentState = BattleState::FINISHED;
                isActive = false;
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
    enemyCharacter->Draw(renderer);

    // Render player
    playerCharacter->Position = playerPosition;
    playerCharacter->Draw(renderer);
}
void Battle::RenderUI() {
    if (!isActive) return;

    // Main battle menu
    ImGui::SetNextWindowPos(ImVec2(600, 450), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(200, 150), ImGuiCond_Always);
    ImGui::Begin("BattleMenu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

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
    if (ImGui::Button("Run", ImVec2(200, 50))) {
        currentState = BattleState::LOSE;
        AddLogMessage("You ran away!");
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

void Battle::RenderHealthBars() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(800, 60), ImGuiCond_Always);
    ImGui::Begin("HealthBars", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Enemy health bar
    ImGui::Text("Enemy: %s", enemyStats.name.c_str());
    ImGui::ProgressBar((float)enemyStats.health / enemyStats.maxHealth, ImVec2(-1, 0));

    // Player health bar
    ImGui::Text("Player: %s", playerStats.name.c_str());
    ImGui::ProgressBar((float)playerStats.health / playerStats.maxHealth, ImVec2(-1, 0));

    ImGui::End();
}

void Battle::RenderMoveSelection() {
    ImGui::BeginChild("Moves", ImVec2(0, 0), true);
    ImGui::Text("Select your move:");

    for (size_t i = 0; i < playerMoves.size(); i++) {
        if (ImGui::Button(playerMoves[i].name.c_str(), ImVec2(-1, 0))) {
            ExecutePlayerMove(playerMoves[i].name);
            showMoveSelection = false;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s\nPower: %d\nAccuracy: %.0f%%", 
                playerMoves[i].description.c_str(), 
                playerMoves[i].power, 
                playerMoves[i].accuracy);
        }
    }
    ImGui::EndChild();
}

void Battle::RenderBattleLog() {
    ImGui::BeginChild("BattleLog", ImVec2(0, 0), true);
    for (const auto& message : battleLog) {
        ImGui::TextWrapped("%s", message.c_str());
    }
    ImGui::EndChild();
}

void Battle::RenderMoveSelection() {
    ImGui::SetNextWindowPos(ImVec2(600, 300), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(200, 150), ImGuiCond_Always);
    ImGui::Begin("MoveSelection", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::Text("Select your move:");

    for (size_t i = 0; i < playerCharacter->attacks.size(); i++) {
        const auto& attack = playerCharacter->attacks[i];
        if (ImGui::Button(attack.name.c_str(), ImVec2(200, 50))) {
            ExecutePlayerMove(attack.name);
            showMoveSelection = false;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s\nType: %s\nPower: %d\nAccuracy: %.0f%%\nSpeed: %d\nEndurance: %d", 
                              attack.description.c_str(), attack.type.c_str(), attack.power, 
                              attack.accuracy, attack.speed, attack.endurance);
        }
    }
    ImGui::End();
}

void Battle::ExecutePlayerMove(const std::string& moveName) {
    auto move = std::find_if(playerMoves.begin(), playerMoves.end(),
        [&moveName](const Move& m) { return m.name == moveName; });

    if (move != playerMoves.end()) {
        if (CheckAccuracy(move->accuracy)) {
            int damage = CalculateDamage(*move, playerStats, enemyStats);
            enemyStats.health = std::max(0, enemyStats.health - damage);
            AddLogMessage(playerStats.name + " used " + moveName + "!");
            AddLogMessage("Dealt " + std::to_string(damage) + " damage!");
        } else {
            AddLogMessage(moveName + " missed!");
        }
    }

    // Check for battle end
    if (enemyStats.health <= 0) {
        currentState = BattleState::WIN;
        stateTimer = BATTLE_START_DELAY;
        AddLogMessage("You won the battle!");
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
        playerStats.status = effect;
        AddLogMessage(playerStats.name + " is now " + StatusEffectToString(effect) + "!");
    } else {
        enemyStats.status = effect;
        AddLogMessage(enemyStats.name + " is now " + StatusEffectToString(effect) + "!");
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
    std::vector<std::pair<int, float>> moveWeights;
    for (size_t i = 0; i < enemyMoves.size(); ++i) {
        const auto& move = enemyMoves[i];
        float weight = move.accuracy; // Base weight on accuracy

        // Add bonus weight if the move's power is high and enemy health is safe
        if (enemyStats.health > 0.5f * enemyStats.maxHealth && move.power > 50) {
            weight += 20.0f;
        }

        // Adjust weight based on type effectiveness
        float typeModifier = GetTypeModifier(move.type, playerStats.type);
        weight *= typeModifier;

        moveWeights.emplace_back(i, weight);
    }

    // Select a move based on weighted random choice
    std::discrete_distribution<> dis(
        moveWeights.begin(), moveWeights.end(),
        [](const std::pair<int, float>& pair) { return pair.second; });

    const Move& selectedMove = enemyMoves[dis(gen)];

    // Execute the move
    if (CheckAccuracy(selectedMove.accuracy)) {
        int damage = CalculateDamage(selectedMove, enemyStats, playerStats);
        playerStats.health = std::max(0, playerStats.health - damage);
        AddLogMessage(enemyStats.name + " used " + selectedMove.name + "!");
        AddLogMessage("Dealt " + std::to_string(damage) + " damage!");
    } else {
        AddLogMessage(enemyStats.name + "'s " + selectedMove.name + " missed!");
    }

    // Check for battle end
    if (playerStats.health <= 0) {
        currentState = BattleState::LOSE;
        stateTimer = BATTLE_START_DELAY;
        AddLogMessage("You lost the battle!");
    }
}

void Battle::AddLogMessage(const std::string& message) {
    battleLog.push_back(message);
    if (battleLog.size() > MAX_LOG_ENTRIES) {
        battleLog.erase(battleLog.begin());
    }
}

int Battle::CalculateDamage(const Move& move, const BattleStats& attacker, const BattleStats& defender) {
    float attackPower = (float)move.power * ((float)attacker.attack / defender.defense);
    float randomFactor = (std::rand() % 15 + 85) / 100.0f; // 0.85 to 1.00
    return static_cast<int>(attackPower * randomFactor);
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