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
                    Direction direction) { // xPos und yPos sind die Positionen im Array, also 0-9
    if (GameRule::shipAddCorrect(size, coordinates, direction, createCopy())) {
        // wenn shipAddCorrect true zurückgibt, dann Platzierung des Schiffs erlaubt → Felder setzen
        int fieldsLeftToSet = size;
        // Anzahl der Felder die noch gesetzt werden müssen
        while (fieldsLeftToSet > 0) {
            // solange noch Felder gesetzt werden müssen
            shipField[coordinates.x][coordinates.y] = true;
            // an der Position [coordinates.x][coordinates.y] befindet sich im shipField ein Schiff, da true
            coordinates = Coordinates::applyDirectionChange(coordinates, direction);
            // passe Koordinaten für (je nach Direction) neuem Feld an
            fieldsLeftToSet--;
            // ein Feld wurde gesetzt → eins weniger zu setzen
        }
        return true;
        // alles erfolgreich, Kombination war möglich zu setzen
    } else {
        return false;
        // alles erfolgreich, Kombination war möglich zu setzen
    }
}

GuessStatus Board::makeGuess(Coordinates coordinates) {
    if (!GameRule::insideField(coordinates) || guessField[coordinates.x][coordinates.y] != GuessStatus::notGuessed) {
        // das angegebene Feld ist entweder außerhalb des Boards oder wurde schon guessed
        return GuessStatus::guessImpossible;
    }
    if (!shipField[coordinates.x][coordinates.y]) {
        // am angegebenen Feld befindet sich kein Schiff
        guessField[coordinates.x][coordinates.y] = GuessStatus::guessedWrong;
        return GuessStatus::guessedWrong;
    }
    if (GameRule::shipDestroyed(coordinates, createCopy())) {
        // wird das angegebene Feld aufgedeckt, wird dadurch ein Schiff komplett zerstört
        guessField[coordinates.x][coordinates.y] = GuessStatus::sunkShip;
        Coordinates appliedDirectionCoordinates = coordinates;
        for (Direction dir: Coordinates::getListOfAllDirections()) {
            appliedDirectionCoordinates = Coordinates::applyDirectionChange(coordinates, dir);
            //gehe die in alle 4 Richtungen angrenzenden Felder durch
            if (GameRule::insideField(appliedDirectionCoordinates)
                && shipField[appliedDirectionCoordinates.x][appliedDirectionCoordinates.y]) {
                setShipInThisDirectionSunk(coordinates, dir);
                // wenn sich im aktuell behandelten (angrenzenden) Feld ein Teil des Schiffs befindet,
                // setze alle in diese Richtung folgenden Schiffsfelder und damit den Schiffsteil,
                // der in diese Richtung zeigt auf Status sunk
            }
        }
        return GuessStatus::sunkShip;
    }
    // wird das angegebene Feld aufgedeckt, wird dadurch kein Schiff komplett zerstört, aber eines getroffen
    guessField[coordinates.x][coordinates.y] = GuessStatus::guessedRight;
    return GuessStatus::guessedRight;

}

// setzt alle Felder in angegebener Richtung, auf denen sich ein Schiff befindet,
// auf Status sunk bis es auf das Ende des Boards erreicht oder auf ein Feld ohne Schiff stößt
void Board::setShipInThisDirectionSunk(Coordinates coordinates, Direction direction) {
    coordinates = Coordinates(coordinates.x, coordinates.y);
    while (true) {
        coordinates = Coordinates::applyDirectionChange(coordinates, direction);
        // nächstes Feld in die angegebene Richtung wird gewählt
        if (!GameRule::insideField(coordinates) || guessField[coordinates.x][coordinates.y] != GuessStatus::guessedRight) {
            return;
            // befindet sich das behandelte Feld außerhalb des Boards oder befindet sich darauf kein Schiff, wird abgebrochen
        }
        guessField[coordinates.x][coordinates.y] = GuessStatus::sunkShip;
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

// gibt den Status des GuessFields eine Position weiter in Richtung der Direction von den Koordinaten aus zurück
GuessStatus Board::guessStatusOfFieldInDirection(Coordinates coordinates, Direction direction) {
    return guessField[Coordinates::applyDirectionChange(coordinates, direction).x]
                        [Coordinates::applyDirectionChange(coordinates, direction).y];
}

// gibt den Wert des GuessFields an der Stelle von coordinates zurück
GuessStatus Board::guessFieldStatus(Coordinates coordinates) {
    return guessField[coordinates.x][coordinates.y];
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


