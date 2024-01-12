//
// Created by laure on 22.12.2023.
//

#include <iostream>
#include "Board.h"
#include "Coordinates.h"

Board::Board() {
    // initialisiere beide Felder
    for (int xPos = 0; xPos < 10; xPos++) {
        for (int yPos = 0; yPos < 10; yPos++) {
            shipField[xPos][yPos] = false;
            guessField[xPos][yPos] = GuessStatus::notGuessed;
        }
    }
}

bool Board::addShip(int size, Coordinates coordinates,
                    Direction direction) { // angenommen xPos und yPos sind die Positionen im Array, also 0-9
    if (GameRule::shipAddCorrect(size, coordinates, direction, createCopy())) {
        // wenn shipAddCorrect true zurückgibt, dann Platzierung des Schiffs erlaubt → Felder setzen
        int fieldsToSet = size;
        Coordinates currentCoordinates = Coordinates(coordinates.x, coordinates.y);
        while (fieldsToSet > 0) {
            shipField[currentCoordinates.x][currentCoordinates.y] = true;
            // passe Variablen für (je nach Direction) neuem Feld an
            currentCoordinates = Coordinates::applyDirectionChange(currentCoordinates, direction);
            fieldsToSet--;
        }
        return true;
    } else {
        return false;
    }
}

GuessStatus Board::makeGuess(Coordinates coordinates) {
    // das angegebene Feld ist entweder außerhalb des Boards oder wurde schon guessed
    if (!GameRule::insideField(coordinates) || guessField[coordinates.x][coordinates.y] != GuessStatus::notGuessed) {
        return GuessStatus::guessImpossible;
    }
    // am angegebenen Feld befindet sich kein Schiff
    if (!shipField[coordinates.x][coordinates.y]) {
        guessField[coordinates.x][coordinates.y] = GuessStatus::guessedWrong;
        return GuessStatus::guessedWrong;
    }
    if (GameRule::shipDestroyed(coordinates, createCopy())) {
        // wird das angegebene Feld aufgedeckt, wird dadurch ein Schiff zerstört
        guessField[coordinates.x][coordinates.y] = GuessStatus::sunkShip;
        // setze alle weiteren angrenzenden Schiffsfelder auf Status sunk
        for (Direction dir: Coordinates::getListOfAllDirections()) {
            Coordinates appliedDirectionCoordinates = Coordinates::applyDirectionChange(coordinates, dir);
            if (GameRule::insideField(appliedDirectionCoordinates)
                && shipField[appliedDirectionCoordinates.x][appliedDirectionCoordinates.y]) {
                setShipInThisDirectionSunk(coordinates, dir);
            }
        }
        std::cout << std::endl;
        return GuessStatus::sunkShip;
    }
    // wird das angegebene Feld aufgedeckt, wird dadurch kein Schiff zerstört
    guessField[coordinates.x][coordinates.y] = GuessStatus::guessedRight;
    return GuessStatus::guessedRight;

}

// setzt alle Felder in der angegebenem Richtung auf Status sunk bis es auf das Ende des Boards oder ein Feld ohne Schiff stößt
void Board::setShipInThisDirectionSunk(Coordinates coordinates, Direction direction) {
    Coordinates currentCoordinates = Coordinates(coordinates.x, coordinates.y);
    while (true) {
        currentCoordinates = Coordinates::applyDirectionChange(currentCoordinates, direction);
        if (!GameRule::insideField(currentCoordinates) || !shipField[currentCoordinates.x][currentCoordinates.y]) {
            return;
        }
        guessField[currentCoordinates.x][currentCoordinates.y] = GuessStatus::sunkShip;
    }
}

void Board::printGuessField() {
    std::cout << std::endl << "     1  2  3  4  5  6  7  8  9  10    " << std::endl;
    // oberer Rand
    for (char c = 'A'; c <= 'J'; c++) {
        std::string output = " ";
        output += c;
        output += "   ";
        // seitlicher Rand
        for (auto &i: guessField) {
            if (i[c - 65] == GuessStatus::sunkShip) {
                output += "#  ";
            } else if (i[c - 65] == GuessStatus::guessedRight) {
                output += "x  ";
            } else if (i[c - 65] == GuessStatus::guessedWrong) {
                output += "   ";
            } else if (i[c - 65] == GuessStatus::notGuessed) {
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
        output += c;
        output += "   ";
        // seitlicher Rand
        for (auto &i: shipField) {
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

