//
// Created by jonif on 06.01.2024.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__GAMERULE_H
#define SCHIFFEVERSENKEN_PROJEKT__GAMERULE_H

#include "Board.h"

class Board;

enum class Direction;
class Coordinates;

class GameRule {
public:
    // gib true zurück wenn Schiff in Bord platzierbar, sonst false
    static bool shipAddCorrect(int, Coordinates, Direction, std::unique_ptr<Board>);

    static bool shipDestroyed(Coordinates, std::unique_ptr<Board>);
    static bool insideBoard(Coordinates, int);
    static int getNumberOfShipsOfThisSizeWhenThisBoardSize(int, int);
private:
    static bool shipInThisDirectionUncovered(Coordinates coordinates, std::unique_ptr<Board> board, Direction direction);
};


#endif //SCHIFFEVERSENKEN_PROJEKT__GAMERULE_H
