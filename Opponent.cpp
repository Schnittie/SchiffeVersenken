//
// Created by laure on 22.12.2023.
//

#include "Opponent.h"
#include "CppRandom.hpp"

std::unique_ptr<Board> Opponent::getShipPosition(std::unique_ptr<Board> board, int shipSize) {
    // probiert zufällige Kombinationen aus bis Schiff platzierbar
    int xPos;
    int yPos;
    int directionSelector;
    Direction direction;
    do {
        xPos = GetRandomNumberBetween(0, 10);
        yPos = GetRandomNumberBetween(0, 10);
        directionSelector = GetRandomNumberBetween(1, 4);
        switch (directionSelector) {
            case 1:
                direction = Direction::up;
            case 2:
                direction = Direction::right;
            case 3:
                direction = Direction::down;
            case 4:
                direction = Direction::left;
        }
    } while (!board->addShip(shipSize, xPos, yPos, direction));
    return board;
}
