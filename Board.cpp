//
// Created by laure on 22.12.2023.
//

#include <iostream>
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
    auto* gameRule = new GameRule; //LLC was ist eine instanz von Gamerule, hatten wir sowas wie static methoden / brauchen wir um methoden hinzuzufügen überhaupt eine klasse
    rule = std::unique_ptr<GameRule>(gameRule);
}

bool Board::addShip(int size, int xPos, int yPos, Direction direction) {
    if (rule->shipAddCorrect(size, xPos, yPos, direction, createCopy())) {
        // wenn shipAddCorrect true zurückgibt, dann Platzierung des Schiffs erlaubt → Felder setzen
        int fieldsToSet = size;
        int recentXPos = xPos;
        int recentYPos = yPos;
        while (fieldsToSet > 0) {
            shipField[recentXPos - 1][recentYPos - 1] = true;
            // passe Variablen für (je nach Direction) neuem Feld an
            if (direction == Direction::left) {
                recentXPos--;
            } else if (direction == Direction::right) {
                recentXPos++;
            } else if (direction == Direction::up) {
                recentYPos--;
            } else{
                recentYPos++;//LLC mein Switch herz leidet
            }
            fieldsToSet--;
        }
        return true;
    } else {
        return false;
    }
}

GuessStatus Board::makeGuess(int xPos, int yPos) {
    // angegebenes Feld ist innerhalb des Boards und wurde noch nicht guessed
    if (xPos < 11 && xPos > 0 && yPos < 11 && yPos > 0 && guessField[xPos-1][yPos-1] == GuessStatus::notGuessed) {
        // an angegebenem Feld befindet sich ein Schiff
        if (shipField[xPos-1][yPos-1]) {
            if (rule->shipDestroyed(xPos, yPos, createCopy())) {
                // wird das angegebene Feld aufgedeckt, wird dadurch ein Schiff zerstört
                guessField[xPos-1][yPos-1] = GuessStatus::sunkShip;
                // setze alle weiteren angrenzenden Schiffsfelder auf Status sunk
                if (xPos - 1 > 0 && shipField[xPos-2][yPos-1]) {
                    setShipInThisDirectionSunk(xPos-1, yPos, Direction::left);
                }
                if (xPos + 1 < 11 && shipField[xPos][yPos-1]) {
                    setShipInThisDirectionSunk(xPos+1, yPos, Direction::right);
                }
                if (yPos - 1 > 0 && shipField[xPos-1][yPos-2]) {
                    setShipInThisDirectionSunk(xPos, yPos-1, Direction::up);
                }
                if (yPos + 1 < 11 && shipField[xPos-1][yPos]) {
                    setShipInThisDirectionSunk(xPos, yPos+1, Direction::down);
                }
                return GuessStatus::sunkShip; //LLC das müsste auch geändert werden für angrenzenden Schiffe
            } else {
                // wird das angegebene Feld aufgedeckt, wird dadurch kein Schiff zerstört
                guessField[xPos-1][yPos-1] = GuessStatus::guessedRight;
                return GuessStatus::guessedRight;
            }
        } else {
            // am angegebenen Feld befindet sich kein Schiff
            guessField[xPos-1][yPos-1] = GuessStatus::guessedWrong;
            return GuessStatus::guessedWrong;
        }
    } else {
        // das angegebene Feld ist entweder außerhalb des Boards oder wurde schon guessed
        return GuessStatus::guessImpossible;
    }
}

// setzt alle Felder in der angegebenem Richtung auf Status sunk bis es auf das Ende des Boards oder ein Feld ohne Schiff stößt
void Board::setShipInThisDirectionSunk(int xPos, int yPos, Direction direction) {
    int recentXPos = xPos;
    int recentYPos = yPos;
    while (true) {
        switch (direction) {
            case Direction::left: //LLC hier wieder der punkt von Info was es bedeutet links zu sein in das enum
                if (recentXPos - 1 > 0 && shipField[recentXPos-2][recentYPos-1]) {
                    guessField[recentXPos-2][recentYPos-1] = GuessStatus::sunkShip;
                    recentXPos--;
                } else {
                    return;
                }
            case Direction::right:
                if (recentXPos + 1 < 11 && shipField[recentXPos][recentYPos-1]) {
                    guessField[recentXPos][recentYPos-1] = GuessStatus::sunkShip;
                    recentXPos++;
                } else {
                    return;
                }
            case Direction::up:
                if (recentYPos - 1 > 0 && shipField[recentXPos-1][recentYPos-2]) {
                    guessField[recentXPos-1][recentYPos-2] = GuessStatus::sunkShip;
                    recentYPos--;
                } else {
                    return;
                }
            case Direction::down:
                if (recentYPos + 1 < 11 && shipField[recentXPos-1][recentYPos]) {
                    guessField[recentXPos-1][recentYPos] = GuessStatus::sunkShip;
                    recentYPos++;
                } else {
                    return;
                }
        }
    }
}

void Board::printGuessField() {
    std::cout << std::endl << "     1  2  3  4  5  6  7  8  9  10    " << std::endl;
    // oberer Rand
    for (char c = 'A'; c <= 'J'; c++) {
        std::string output = " ";
        output +=  c;
        output += "   ";
        // seitlicher Rand
        for (auto & i : guessField) {
            if (i[c-65] == GuessStatus::sunkShip) {
                output += "#  ";
            } else if (i[c-65] == GuessStatus::guessedRight) {
                output += "x  ";
            } else if (i[c-65] == GuessStatus::guessedWrong) {
                output += "   ";
            } else if (i[c-65] == GuessStatus::notGuessed) {
                output += "~  ";
            }
            // Zeile des Guess Fields (c-65 ist Zahl von 0 bis 9, da ASCII-Wert von A = 65 und J = 74) //LLC ich vertraue dir
        }
        std::cout << output << "     " << std::endl;
    }
    std::cout << std::endl;
}

void Board::printShipField() {
    std::cout << std::endl << "     1  2  3  4  5  6  7  8  9  10    " << std::endl;
    // oberer Rand
    for (char c = 'A'; c <= 'J'; c++) {
        std::string output = " ";
        output +=  c;
        output += "   ";
        // seitlicher Rand
        for (auto & i : shipField) {
            if (i[c - 65]) {
                output += "#  ";
            } else {
                output += "~  ";
            }
            // Zeile des Ship Fields (c-65 ist Zahl von 0 bis 9, da ASCII-Wert von A = 65 und J = 74)
        }
        std::cout << output << "     " << std::endl;
    }
    std::cout << std::endl;
}

std::unique_ptr<Board> Board::createCopy() {
    // Erstelle einen neuen std::unique_ptr, der auf eine Kopie des aktuellen Objekts zeigt
    std::unique_ptr<Board> boardCopy = std::make_unique<Board>();//LLC wir arbeiten mit pointern also bist du sicher das das eine kopie ist
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            boardCopy->guessField[i][j] = guessField[i][j];
            boardCopy->shipField[i][j] = shipField[i][j];
        }
    }
    return boardCopy;
}

Board::~Board() {
    rule = nullptr;
}
