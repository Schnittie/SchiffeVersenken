//
// Created by laure on 22.12.2023.
//

#include "Board.h"

Board::Board() {
    // initialisiere beide Felder
    for (int xPos = 0; xPos < 10; xPos++) {
        for (int yPos = 0; yPos < 10; yPos++) {
            shipField[xPos][yPos] = false;
            guessField[xPos][yPos] = GuessStatus::notGuessed;
        }
    }
    // richte unique_ptr auf GameRule ein
    auto* gameRule = new GameRule;
    rule = std::unique_ptr<GameRule>(gameRule);
}

bool Board::addShip(int size, int xPos, int yPos, Direction direction) {
    if (rule->shipAddCorrect(size, xPos, yPos, direction, this)) {
        // wenn shipAddCorrect true zurückgibt, dann Platzierung des Schiffs erlaubt → Felder setzen
        int fieldsToSet = size;
        int recentXPos = xPos;
        int recentYPos = yPos;
        while (fieldsToSet > 0) {
            shipField[recentXPos - 1][recentYPos - 1] = true;
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
        }
        return true;
    } else {
        return false;
    }
}
