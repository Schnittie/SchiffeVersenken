//
// Created by laure on 22.12.2023.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__BOARD_H
#define SCHIFFEVERSENKEN_PROJEKT__BOARD_H


#include <vector>
#include <memory>
#include "GameRule.h"
#include "Coordinates.h"

enum class GuessStatus {
    notGuessed,
    guessImpossible,
    guessedWrong,
    guessedRight,
    sunkShip
};


class Board {
private:
    void setShipInThisDirectionSunk(Coordinates, Direction);

public:
    std::unique_ptr<Board> createCopy();

    bool shipField[10][10]{};
    GuessStatus guessField[10][10]{};

    Board();

    //Methode bekommt Größe, Ursprungsposition (x und y) und Ausrichtung von Ursprungsposition aus und gibt bool zurück, ob Platzierung geklappt hat
    bool addShip(int, Coordinates, Direction);
    GuessStatus makeGuess(Coordinates);
    GuessStatus guessStatusOfFieldInDirection(Coordinates, Direction);
    GuessStatus guessFieldStatus(Coordinates);

    void printGuessField();
    void printShipField();
};

#endif //SCHIFFEVERSENKEN_PROJEKT__BOARD_H
