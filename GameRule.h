//
// Created by jonif on 06.01.2024.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__GAMERULE_H
#define SCHIFFEVERSENKEN_PROJEKT__GAMERULE_H

#include "Board.h"

class Board;

enum class Direction;

class GameRule {
public:
    // gib true zurück wenn Schiff in Bord platzierbar, sonst false
    bool shipAddCorrect(int, int, int, Direction, std::unique_ptr<Board>);
    bool shipDestroyed(int, int, std::unique_ptr<Board>);
    bool insideField(int);
    bool insideField(int, int);
private:
    bool shipInThisDirectionDestroyed(int, int, std::unique_ptr<Board>, Direction);
};


#endif //SCHIFFEVERSENKEN_PROJEKT__GAMERULE_H
