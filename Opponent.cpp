//
// Created by laure on 22.12.2023.
//

#include "Opponent.h"
#include "CppRandom.hpp"

std::unique_ptr<Board> Opponent::addRandomShipOfGivenSize(std::unique_ptr<Board> board, int shipSize) {
    // probiert zufällige Kombinationen aus bis Schiff platzierbar
    int xPos;
    int yPos;
    int directionSelector;
    Direction direction;
    do {
        xPos = GetRandomNumberBetween(1, 10);
        yPos = GetRandomNumberBetween(1, 10);
        directionSelector = GetRandomNumberBetween(1, 4);
        if (directionSelector == 1) {
            direction = Direction::up;
        } else if (directionSelector == 2) {
            direction = Direction::right;
        } else if (directionSelector == 3) {
            direction = Direction::down;
        } else {
            direction = Direction::left;
        }
    } while (!board->addShip(shipSize, xPos, yPos, direction));
    return std::move(board);
}

std::unique_ptr<Board> Opponent::placeAllShips(std::unique_ptr<Board> board) {
    for (int shipSize = 5; shipSize > 1; shipSize--) {
        for (int shipNumber = 0; shipNumber < 6-shipSize; shipNumber++) {
            board = addRandomShipOfGivenSize(std::move(board), shipSize);
        }
    }
    return board;
}

std::unique_ptr<Board> Opponent::makeGuess(std::unique_ptr<Board> board) {
    int makeCalculatedGuessNumber = GetRandomNumberBetween(0, 10) + smartness;
    int xPosGuessedRightField = 0;
    int yPosGuessedRightField = 0;
    std::vector<Coordinates> freeFields;
    for (int xPos = 1; xPos < 11; xPos++) {
        for (int yPos = 1; yPos < 11; yPos++) {
            if (board->guessField[xPos - 1][yPos - 1] == GuessStatus::notGuessed) {
                freeFields.emplace_back(xPos, yPos);
            }
            if (board->guessField[xPos - 1][yPos - 1] == GuessStatus::guessedRight && makeCalculatedGuessNumber >= 10) {
                xPosGuessedRightField = xPos;
                yPosGuessedRightField = yPos;
//                break;
            }
        }
        if (xPosGuessedRightField != 0 && yPosGuessedRightField != 0) {
//            break;
        }
    }
    int xPosNextToGuessedRightField = 0;
    int yPosNextToGuessedRightField = 0;
    if (xPosGuessedRightField != 0 && yPosGuessedRightField != 0) {
        if (xPosGuessedRightField + 1 < 11 &&
        board->guessField[xPosGuessedRightField][yPosGuessedRightField - 1] == GuessStatus::notGuessed) {
            xPosNextToGuessedRightField = xPosGuessedRightField + 1;
            yPosNextToGuessedRightField = yPosNextToGuessedRightField;
        } else if (xPosGuessedRightField - 1 > 0 &&
        board->guessField[xPosGuessedRightField - 2][yPosGuessedRightField - 1] == GuessStatus::notGuessed) {
            xPosNextToGuessedRightField = xPosGuessedRightField - 1;
            yPosNextToGuessedRightField = yPosNextToGuessedRightField;
        } else if (yPosNextToGuessedRightField + 1 < 11 &&
        board->guessField[xPosGuessedRightField - 1][yPosGuessedRightField] == GuessStatus::notGuessed) {
            xPosNextToGuessedRightField = xPosGuessedRightField;
            yPosNextToGuessedRightField = yPosNextToGuessedRightField + 1;
        } else if (yPosNextToGuessedRightField - 1 > 0 &&
        board->guessField[xPosGuessedRightField - 1][yPosGuessedRightField - 2] == GuessStatus::notGuessed) {
            xPosNextToGuessedRightField = xPosGuessedRightField;
            yPosNextToGuessedRightField = yPosNextToGuessedRightField - 1;
        }
        if (board->makeGuess(xPosNextToGuessedRightField, yPosNextToGuessedRightField) != GuessStatus::guessImpossible) {
            return std::move(board);
        } else {
            return guessRandom(std::move(board), freeFields);
        }
    } else {
        return guessRandom(std::move(board), freeFields);
    }
}

std::unique_ptr<Board> Opponent::guessRandom(std::unique_ptr<Board> board, std::vector<Coordinates> freeFields) {
    int randomFreeField = GetRandomNumberBetween(0, freeFields.size()-1);
    board->makeGuess(freeFields.at(randomFreeField).x, freeFields.at(randomFreeField).y);
//    while (board->makeGuess(randomXPos, randomYPos) == GuessStatus::guessImpossible) {
//        randomXPos = GetRandomNumberBetween(1, 10);
//        randomYPos = GetRandomNumberBetween(1, 10);
//    }
    return std::move(board);
}

Coordinates::Coordinates(int xCoordinate, int yCoordinate): x(xCoordinate), y(yCoordinate) {}