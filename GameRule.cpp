//
// Created by jonif on 06.01.2024.
//

#include "GameRule.h"


bool GameRule::shipAddCorrect(int size, int xPos, int yPos, Direction direction, Board board) {
    // teste, ob Ursprungspunkt des Schiffs auf dem Feld
    if (size < 6 && size > 0 && xPos < 11 && xPos > 0 && yPos < 11 && yPos > 0) {
        // testen, ob weitere Schiffsfelder innerhalb von Board und nicht neben anderem Schiffsfeld
        int fieldsToSet = size;
        int recentXPos = xPos;
        int recentYPos = yPos;
        while (fieldsToSet > 0) {
            // teste, ob aktuell zu behandelndes Feld unbelegt ist
            if (recentXPos > 0 && recentYPos > 0 && recentXPos < 11 && recentYPos < 11 &&
                !board.shipField[recentXPos - 1][recentYPos - 1]) {
                // teste, ob Feld links davon unbelegt ist oder außerhalb des Feldes liegt
                bool leftFieldFree = recentXPos == 1 || !board.shipField[recentXPos - 2][recentYPos - 1];
                // teste, ob Feld links davon unbelegt ist oder außerhalb des Feldes liegt
                bool rightFieldFree = recentXPos == 10 || !board.shipField[recentXPos][recentYPos - 1];
                // teste, ob Feld links davon unbelegt ist oder außerhalb des Feldes liegt
                bool upperFieldFree = recentYPos == 1 || !board.shipField[recentXPos - 1][recentYPos - 2];
                // teste, ob Feld links davon unbelegt ist oder außerhalb des Feldes liegt
                bool lowerFieldFree = recentYPos == 10 || !board.shipField[recentXPos - 1][recentYPos];

                if (leftFieldFree && rightFieldFree && upperFieldFree && lowerFieldFree) {
                    // passe Variablen für (je nach Direction) neuem Feld an
                    switch (direction) {
                        case Direction::left:
                            recentXPos--;
                        case Direction::right:
                            recentXPos++;
                        case Direction::up:
                            recentYPos--;
                        case Direction::down:
                            recentYPos++;
                    }
                    fieldsToSet--;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}