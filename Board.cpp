//
// Created by laure on 22.12.2023.
//

#include "Board.h"

Board::Board() {
    for (int xPos = 0; xPos < 10; xPos++) {
        for (int yPos = 0; yPos < 10; yPos++) {
            shipField[xPos][yPos] = false;
            guessField[xPos][yPos] = GuessStatus::notGuessed;
        }
    }
}

bool Board::addShip(int size, int xPos, int yPos, Direction direction) {
    if (size > 5 || size < 0 || xPos > 10 || xPos < 1 || yPos > 10 || yPos < 1) {
        return false;
    } else {
        // Felder testen
        int fieldsToSet = size;
        int recentXPos = xPos;
        int recentYPos = yPos;
        while (fieldsToSet > 0) {
            // teste, ob aktuell zu behandelndes Feld unbelegt ist
            if (recentXPos > 0 && recentYPos > 0 && recentXPos < 11 && recentYPos < 11 && !shipField[recentXPos-1][recentYPos-1]) {
                // teste, ob Feld links davon unbelegt ist oder außerhalb des Feldes liegt
                bool leftFieldFree = recentXPos == 1 || !shipField[recentXPos-2][recentYPos-1];
                // teste, ob Feld links davon unbelegt ist oder außerhalb des Feldes liegt
                bool rightFieldFree = recentXPos == 10 || !shipField[recentXPos][recentYPos-1];
                // teste, ob Feld links davon unbelegt ist oder außerhalb des Feldes liegt
                bool upperFieldFree = recentYPos == 1 || !shipField[recentXPos-1][recentYPos-2];
                // teste, ob Feld links davon unbelegt ist oder außerhalb des Feldes liegt
                bool lowerFieldFree = recentYPos == 10 || !shipField[recentXPos-1][recentYPos];

                if (leftFieldFree && rightFieldFree && upperFieldFree && lowerFieldFree) {
                    // passe Variablen für (je nach Direction) neuem Feld an
                    switch (direction) {
                        case Direction::left: recentXPos--;
                        case Direction::right: recentXPos++;
                        case Direction::up: recentYPos--;
                        case Direction::down: recentYPos++;
                    }
                    fieldsToSet--;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        // wenn ohne return false durchgekommen, dann alle Felder möglich -> Felder setzen
        fieldsToSet = size;
        recentXPos = xPos;
        recentYPos = yPos;
        while (fieldsToSet > 0) {
            shipField[recentXPos-1][recentYPos-1] = true;
            // passe Variablen für (je nach Direction) neuem Feld an
            switch (direction) {
                case Direction::left: recentXPos--;
                case Direction::right: recentXPos++;
                case Direction::up: recentYPos--;
                case Direction::down: recentYPos++;
            }
            fieldsToSet--;
        }
        return true;
    }
}
