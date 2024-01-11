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
}

bool Board::addShip(int size, int xPos, int yPos, Direction direction) { // angenommen xPos und yPos sind die Positionen im Array, also 0-9
    if (GameRule::shipAddCorrect(size, xPos, yPos, direction, createCopy())) {
        // wenn shipAddCorrect true zurückgibt, dann Platzierung des Schiffs erlaubt → Felder setzen
        int fieldsToSet = size;
        int recentXPos = xPos;
        int recentYPos = yPos;
        while (fieldsToSet > 0) {
            shipField[recentXPos][recentYPos] = true;
            // passe Variablen für (je nach Direction) neuem Feld an

            if (direction == Direction::left) {
                recentXPos--;
            } else if (direction == Direction::right) {
                recentXPos++;
            } else if (direction == Direction::up) {
                recentYPos--;
            } else{
                recentYPos++;
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
    if (GameRule::insideField(xPos, yPos) && guessField[xPos][yPos] == GuessStatus::notGuessed) {
        // an angegebenem Feld befindet sich ein Schiff
        if (shipField[xPos][yPos]) {
            if (GameRule::shipDestroyed(xPos, yPos, createCopy())) {
                // wird das angegebene Feld aufgedeckt, wird dadurch ein Schiff zerstört
                guessField[xPos][yPos] = GuessStatus::sunkShip;
                // setze alle weiteren angrenzenden Schiffsfelder auf Status sunk
                if (GameRule::insideField(xPos - 1) && shipField[xPos-1][yPos]) {
                    setShipInThisDirectionSunk(xPos, yPos, Direction::left);
                }
                if (GameRule::insideField(xPos + 1) && shipField[xPos+1][yPos]) {
                    setShipInThisDirectionSunk(xPos, yPos, Direction::right);
                }
                if (GameRule::insideField(yPos - 1) && shipField[xPos][yPos-1]) {
                    setShipInThisDirectionSunk(xPos, yPos, Direction::up);
                }
                if (GameRule::insideField(yPos + 1) && shipField[xPos][yPos+1]) {
                    setShipInThisDirectionSunk(xPos, yPos, Direction::down);
                }
                std::cout << std::endl;
                return GuessStatus::sunkShip;
            } else {
                // wird das angegebene Feld aufgedeckt, wird dadurch kein Schiff zerstört
                guessField[xPos][yPos] = GuessStatus::guessedRight;
                return GuessStatus::guessedRight;
            }
        } else {
            // am angegebenen Feld befindet sich kein Schiff
            guessField[xPos][yPos] = GuessStatus::guessedWrong;
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
        if (direction == Direction::left) {
            if (GameRule::insideField(recentXPos - 1) && shipField[recentXPos-1][recentYPos]) {
                guessField[recentXPos-1][recentYPos] = GuessStatus::sunkShip;
                recentXPos--;
            } else {
                return;
            }
        } else if (direction == Direction::right) {
            if (GameRule::insideField(recentXPos + 1) && shipField[recentXPos+1][recentYPos]) {
                guessField[recentXPos+1][recentYPos] = GuessStatus::sunkShip;
                recentXPos++;
            } else {
                return;
            }
        } else if (direction == Direction::up) {
            if (GameRule::insideField(recentYPos - 1) && shipField[recentXPos][recentYPos-1]) {
                guessField[recentXPos][recentYPos-1] = GuessStatus::sunkShip;
                recentYPos--;
            } else {
                return;
            }
        } else {
            if (GameRule::insideField(recentYPos + 1) && shipField[recentXPos][recentYPos+1]) {
                guessField[recentXPos][recentYPos+1] = GuessStatus::sunkShip;
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
            // Zeile des Guess Fields (c-65 ist Zahl von 0 bis 9, da ASCII-Wert von A = 65 und J = 74)
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
    std::unique_ptr<Board> boardCopy = std::make_unique<Board>();
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            boardCopy->guessField[i][j] = guessField[i][j];
            boardCopy->shipField[i][j] = shipField[i][j];
        }
    }
    return boardCopy;
}

Coordinates::Coordinates(int xCoordinate, int yCoordinate) : x(xCoordinate), y(yCoordinate) {}

Coordinates::Coordinates(Direction dir) {
    switch (dir) {
        case Direction::down:
            x = 0;
            y = 1;
            break;
        case Direction::up:
            x = 0;
            y = -1;
            break;
        case Direction::left:
            x = -1;
            y = 0;
            break;
        case Direction::right:
            x = 1;
            y = 0;
            break;
    }
}

