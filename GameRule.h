//
// Created by jonif on 06.01.2024.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__GAMERULE_H
#define SCHIFFEVERSENKEN_PROJEKT__GAMERULE_H

#include "Board.h."

class GameRule {
public:
    // gib true zurück wenn Schiff in Bord platzierbar, sonst false
    bool shipAddCorrect(int, int, int, Direction, Board);
};


#endif //SCHIFFEVERSENKEN_PROJEKT__GAMERULE_H
