#ifndef BATTLE_RNG_H
#define BATTLE_RNG_H

#include <random>

class BattleRNG {
private:
    static std::mt19937& getGenerator() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

public:
    static bool checkAccuracy(float accuracy) {
        std::uniform_real_distribution<> dis(0.0, 100.0);
        return dis(getGenerator()) < accuracy;
    }
    
    static size_t selectMove(const std::vector<float>& weights) {
        if (weights.empty()) return 0;
        std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
        return dist(getGenerator());
    }
    
    static float getRandomFactor() {
        std::uniform_real_distribution<float> dis(0.85f, 1.0f);
        return dis(getGenerator());
    }
};

#endif // BATTLE_RNG_H 