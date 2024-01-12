//
// Created by laure on 11.01.2024.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__COORDINATES_H
#define SCHIFFEVERSENKEN_PROJEKT__COORDINATES_H


#include "Board.h"

enum class Direction {
    left,
    right,
    up,
    down
};

class Coordinates{

public:
    int x;
    int y;

    Coordinates(int, int);

    explicit Coordinates(Direction);
    static Coordinates applyDirectionChange(Coordinates, Direction);
    static std::vector<Direction> getListOfAllDirections();
};


#endif //SCHIFFEVERSENKEN_PROJEKT__COORDINATES_H
