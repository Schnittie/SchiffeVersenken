//
// Created by laure on 22.12.2023.
//

#include "Opponent.h"
#include "CppRandom.hpp"

std::unique_ptr<Board> Opponent::addRandomShipOfGivenSize(std::unique_ptr<Board> board, int shipSize) {
    // probiert zufällige Kombinationen aus bis Schiff platzierbar //LLC ist es eine Idee erst zu schauen wo möglich und dann random zu setzen
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
    for (int shipSize = 5; shipSize > 1; shipSize--) { //LLC also es gibt ein schiff mit jeder größe 2-5 ein mal
        for (int shipNumber = 0; shipNumber < 6-shipSize; shipNumber++) {
            board = addRandomShipOfGivenSize(std::move(board), shipSize);
        }
    }
    return board;
}

std::unique_ptr<Board> Opponent::makeGuess(std::unique_ptr<Board> board) {
    int makeCalculatedGuessNumber = GetRandomNumberBetween(0, 10) + smartness;//LLC wenn das random über 10 ist macht er einen bedachten guess, sonst einfach random
    if (makeCalculatedGuessNumber >= 10) {
        int xPosGuessedRightField = 0;
        int yPosGuessedRightField = 0;
        for (int xPos = 1; xPos < 11; ++xPos) {
            for (int yPos = 1; yPos < 11; ++yPos) {
                if (board->guessField[xPos - 1][yPos - 1] == GuessStatus::guessedRight) {//LLC guessed right um versenkte schiffe zu vermeiden
                    xPosGuessedRightField = xPos;
                    yPosGuessedRightField = yPos;
                    break;
                }
            }
            if (xPosGuessedRightField != 0 && yPosGuessedRightField != 0) {
                break; //LLC wofür ist das hier
            }
        }//LLC wir haben die koordinaten vom ersten richtigen feld welches noch nicht versenkt wurde
        int xPosNextToGuessedRightField = 0;
        int yPosNextToGuessedRightField = 0;
        if (xPosGuessedRightField != 0 && yPosGuessedRightField != 0) {
            if (xPosGuessedRightField + 1 < 11 &&
                board->guessField[xPosGuessedRightField][yPosGuessedRightField - 1] == GuessStatus::notGuessed) {
                xPosNextToGuessedRightField = xPosGuessedRightField + 1;
                yPosNextToGuessedRightField = yPosNextToGuessedRightField;
            } else if (xPosGuessedRightField - 1 > 0 &&
                       board->guessField[xPosGuessedRightField - 2][yPosGuessedRightField - 1] ==
                       GuessStatus::notGuessed) {
                xPosNextToGuessedRightField = xPosGuessedRightField - 1;
                yPosNextToGuessedRightField = yPosNextToGuessedRightField;
            } else if (yPosNextToGuessedRightField + 1 < 11 &&
                       board->guessField[xPosGuessedRightField - 1][yPosGuessedRightField] == GuessStatus::notGuessed) {
                xPosNextToGuessedRightField = xPosGuessedRightField;
                yPosNextToGuessedRightField = yPosNextToGuessedRightField + 1;
            } else if (yPosNextToGuessedRightField - 1 > 0 &&
                       board->guessField[xPosGuessedRightField - 1][yPosGuessedRightField - 2] ==
                       GuessStatus::notGuessed) {
                xPosNextToGuessedRightField = xPosGuessedRightField;
                yPosNextToGuessedRightField = yPosNextToGuessedRightField - 1;
            }//LLC es werden alle felder um das richtige feld herum angeschaut auf die noch nicht geguessed wurde und es wird das letze davon danach geguessed
            //LLC eine noch schlauerere opponent würde schauen ob es bereits eine Reihe an richtigen guesses gibt und diese fortsetzen
            if (xPosNextToGuessedRightField != 0 && yPosNextToGuessedRightField != 0) {
                if (board->makeGuess(xPosNextToGuessedRightField, yPosNextToGuessedRightField) !=
                    GuessStatus::guessImpossible) {
                    return std::move(board);
                } else {
                    return guessRandom(std::move(board));
                }
            } else {
                return guessRandom(std::move(board));
            }
        } else {
            return guessRandom(std::move(board));
        }
    } else {
        return guessRandom(std::move(board));
    }
}

std::unique_ptr<Board> Opponent::guessRandom(std::unique_ptr<Board> board) {
    int randomXPos = GetRandomNumberBetween(1, 10);
    int randomYPos = GetRandomNumberBetween(1, 10);
    while (board->makeGuess(randomXPos, randomYPos) == GuessStatus::guessImpossible) {
        randomXPos = GetRandomNumberBetween(1, 10);
        randomYPos = GetRandomNumberBetween(1, 10);
    }
    return std::move(board);
}

