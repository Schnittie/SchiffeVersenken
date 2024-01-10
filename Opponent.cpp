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
        xPos = GetRandomNumberBetween(0, 9);
        yPos = GetRandomNumberBetween(0, 9);
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
    int makeCalculatedGuessNumber = GetRandomNumberBetween(0, 10) + smartness;
    //LLC wenn das random über 10 ist macht er einen bedachten guess, sonst einfach random
    int xPosGuessedRightField = -1;
    int yPosGuessedRightField = -1;
    std::vector<Coordinates> freeFields;
    for (int xPos = 0; xPos < 10; xPos++) {
        for (int yPos = 0; yPos < 10; yPos++) {
            if (board->guessField[xPos][yPos] == GuessStatus::notGuessed) {
                freeFields.emplace_back(xPos, yPos);
            }
            if (board->guessField[xPos][yPos] == GuessStatus::guessedRight && // Schiff wurde getroffen, aber noch nicht zerstört
                makeCalculatedGuessNumber >= 10) {//LLC guessed right um versenkte schiffe zu vermeiden
                // speichert die Koordinaten an, denen ein Schiff getroffen wurde in die Variablen ab
                xPosGuessedRightField = xPos;
                yPosGuessedRightField = yPos;
            }
        }
    }
    int xPosNextToGuessedRightField;
    int yPosNextToGuessedRightField;
    if (xPosGuessedRightField != -1 && yPosGuessedRightField != -1) {
        // wenn ein Feld gefunden wurde, an dem ein Schiff getroffen aber nicht zerstört wurde, wird geschaut,
        // welches der umliegenden Felder noch nicht beschossen wurde (es muss zwingend 1 geben)
        // → Resultat in Variablen gespeichert
        if (xPosGuessedRightField + 1 <= 9 &&
            board->guessField[xPosGuessedRightField][yPosGuessedRightField - 1] == GuessStatus::notGuessed) {
            xPosNextToGuessedRightField = xPosGuessedRightField + 1;
            yPosNextToGuessedRightField = yPosNextToGuessedRightField;
        } else if (xPosGuessedRightField - 1 >= 0 &&
                   board->guessField[xPosGuessedRightField - 2][yPosGuessedRightField - 1] == GuessStatus::notGuessed) {
            xPosNextToGuessedRightField = xPosGuessedRightField - 1;
            yPosNextToGuessedRightField = yPosNextToGuessedRightField;
        } else if (yPosNextToGuessedRightField + 1 <= 9 &&
                   board->guessField[xPosGuessedRightField - 1][yPosGuessedRightField] == GuessStatus::notGuessed) {
            xPosNextToGuessedRightField = xPosGuessedRightField;
            yPosNextToGuessedRightField = yPosNextToGuessedRightField + 1;
        } else if (yPosNextToGuessedRightField - 1 >= 0 &&
                   board->guessField[xPosGuessedRightField - 1][yPosGuessedRightField - 2] == GuessStatus::notGuessed) {
            xPosNextToGuessedRightField = xPosGuessedRightField;
            yPosNextToGuessedRightField = yPosNextToGuessedRightField - 1;
        }//LLC es werden alle felder um das richtige feld herum angeschaut auf die noch nicht geguessed wurde und es wird das letze davon danach geguessed
        //LLC eine noch schlauerere opponent würde schauen ob es bereits eine Reihe an richtigen guesses gibt und diese fortsetzen
        if (board->makeGuess(xPosNextToGuessedRightField, yPosNextToGuessedRightField) !=
            GuessStatus::guessImpossible) {
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
    return std::move(board);
}

Coordinates::Coordinates(int xCoordinate, int yCoordinate): x(xCoordinate), y(yCoordinate) {}