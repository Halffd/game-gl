#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <chrono>

class Random {
public:
    static std::mt19937& getGenerator() {
        static std::mt19937 gen(getSeed());
        return gen;
    }

private:
    static std::mt19937::result_type getSeed() {
        // Combine time and hardware entropy for better randomization
        static std::random_device rd;
        std::mt19937::result_type seed = rd() ^ (
            std::chrono::high_resolution_clock::now().time_since_epoch().count() +
            std::mt19937::result_type(std::random_device{}())
        );
        return seed;
    }
};

#endif // RANDOM_H 