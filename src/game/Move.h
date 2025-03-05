#ifndef GAME_MOVE_H
#define GAME_MOVE_H

#include <string>

struct Move {
    std::string name;
    std::string description;
    std::string type;
    int power;
    float accuracy;
    int quantity;

    Move(std::string n, std::string desc, std::string t, int p, float acc, int q)
        : name(std::move(n))
        , description(std::move(desc))
        , type(std::move(t))
        , power(p)
        , accuracy(acc)
        , quantity(q)
    {}
};

#endif // GAME_MOVE_H 