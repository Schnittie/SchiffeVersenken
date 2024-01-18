//
// Created by laure on 11.01.2024.
//

#include <iostream>
#include "Coordinates.h"

Coordinates::Coordinates(int xCoordinate, int yCoordinate) : x(xCoordinate), y(yCoordinate) {}

// ändert die Koordinaten so, dass sie nun auf dem angrenzenden Feld in der angegebenen Richtung vom Ausgangsfeld liegen
Coordinates Coordinates::applyDirectionChange(Coordinates originalCoordinates, Direction direction) {
    switch (direction) {
        case Direction::down:
            return Coordinates(originalCoordinates.x, originalCoordinates.y + 1);
        case Direction::up:
            return Coordinates(originalCoordinates.x, originalCoordinates.y - 1);
        case Direction::left:
            return Coordinates(originalCoordinates.x - 1, originalCoordinates.y);
        case Direction::right:
            return Coordinates(originalCoordinates.x + 1, originalCoordinates.y);
    }
    return Coordinates(originalCoordinates.x, originalCoordinates.y + 1);
}

// gibt alle Richtungen, die man auf dem Board von einem Feld aus gehen kann in einem Vektor zurück
std::vector<Direction> Coordinates::getListOfAllDirections() {
    std::vector<Direction> allDirections;
    allDirections.push_back(Direction::left);
    allDirections.push_back(Direction::right);
    allDirections.push_back(Direction::up);
    allDirections.push_back(Direction::down);
    return allDirections;
}

Direction Coordinates::getOppositeDirection(Direction direction) {
    switch (direction) {
        case Direction::down:
            return Direction::up;
        case Direction::up:
            return Direction::down;
        case Direction::left:
            return Direction::right;
        case Direction::right:
            return Direction::left;
    }
    return Direction::down;
}

Direction Coordinates::charIntoDirection(char c) {
    switch (c) {
        case 'd':
            return Direction::down;
        case 'u':
            return Direction::up;
        case 'l':
            return Direction::left;
        case 'r':
            return Direction::right;
        default:
            std::cout << std::endl << "Faulty input!" << std::endl;
            return Direction::down;
    }
}
