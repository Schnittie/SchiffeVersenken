//
// Created by laure on 22.12.2023.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__BOARD_H
#define SCHIFFEVERSENKEN_PROJEKT__BOARD_H


#include <vector>
#include <memory>
#include "GameRule.h"

enum class Direction {
    left,
    right,
    up,
    down
};

enum class GuessStatus {
    notGuessed,
    alreadyGuessed,
    guessedWrong,
    guessedRight,
    sunkShip
};


class Board {
private:
    std::unique_ptr<GameRule> rule;
public:
    bool shipField[10][10]{};
    GuessStatus guessField[10][10]{};

    Board();

    //Methode bekommt Größe, Ursprungsposition (x und y) und Ausrichtung von Ursprungsposition aus und gibt bool zurück, ob Platzierung geklappt hat
    bool addShip(int, int, int, Direction);


//TODO: was ist ein board?
//TODO: idee: es gibt drei versionen des Spielbretts, eins auf dem gespeichert ist wo die schiffe sind und eins für jeden Spieler auf dem Treffer und Nieten gespeichert sind
// Meine Überlegung (Fleischi): würde sagen, man braucht 2, jeder seins, in das man seine Schiffe speichert, man "guessed" dann sozusagen auf dem gegnerischen Board

};


#endif //SCHIFFEVERSENKEN_PROJEKT__BOARD_H
