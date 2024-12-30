// Battle.cpp
#include "Battle.h"
#include <algorithm>
#include <random>

Battle::Battle(std::shared_ptr<Player> player, GameObject* enemy)
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
        "Wild Enemy"
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

    // Main battle window
    ImGui::SetNextWindowPos(ImVec2(0, 450), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(800, 150), ImGuiCond_Always);
    ImGui::Begin("Battle", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Split the window into two columns
    ImGui::Columns(2, "BattleColumns", false);
    
    // Left column: Battle log
    RenderBattleLog();
    
    ImGui::NextColumn();
    
    // Right column: Move selection or battle status
    if (showMoveSelection && currentState == BattleState::PLAYER_TURN) {
        RenderMoveSelection();
    }
    
    ImGui::End();

    // Render health bars in a separate window
    RenderHealthBars();
}

void Battle::RenderHealthBars() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(800, 60), ImGuiCond_Always);
    ImGui::Begin("Health Bars", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Enemy health bar
    ImGui::Text("%s: %d/%d", enemyStats.name.c_str(), enemyStats.health, enemyStats.maxHealth);
    ImGui::ProgressBar((float)enemyStats.health / enemyStats.maxHealth, ImVec2(-1, 0));

    // Player health bar
    ImGui::Text("%s: %d/%d", playerStats.name.c_str(), playerStats.health, playerStats.maxHealth);
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

void Battle::ExecuteEnemyMove() {
    // Randomly select enemy move
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, enemyMoves.size() - 1);
    const Move& selectedMove = enemyMoves[dis(gen)];

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