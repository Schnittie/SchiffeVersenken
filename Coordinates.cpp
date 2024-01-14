//
// Created by laure on 11.01.2024.
//

#include "Coordinates.h"

Coordinates::Coordinates(int xCoordinate, int yCoordinate) : x(xCoordinate), y(yCoordinate) {}

//Coordinates::Coordinates(Direction dir) {
//    switch (dir) {
//        case Direction::down:
//            x = 0;
//            y = 1;
//            break;
//        case Direction::up:
//            x = 0;
//            y = -1;
//            break;
//        case Direction::left:
//            x = -1;
//            y = 0;
//            break;
//        case Direction::right:
//            x = 1;
//            y = 0;
//            break;
//    }
//}

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
