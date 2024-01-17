//
// Created by laure on 22.12.2023.
//

#include <iostream>
#include "Board.h"
#include "Coordinates.h"

Board::Board(int boardSize) {
    size = boardSize;
    // speichere die Größe des Boards
    for (int xPos = 0; xPos < size; xPos++) {
        for (int yPos = 0; yPos < size; yPos++) {
            if (xPos == 0) {
                std::vector<bool> shipFieldCol;
                shipField.push_back(shipFieldCol);
                std::vector<GuessStatus> guessFieldCol;
                guessField.push_back(guessFieldCol);
            }
            shipField.at(xPos).push_back(false);
            guessField.at(xPos).push_back(GuessStatus::notGuessed);
        }
    }
    // initialisiere beide Felder
    int neededShipsOfSize;
    for (int shipSize = 2; shipSize <= 5; shipSize++) {
        // beginne bei Schiffen mit der Größe 2
        neededShipsOfSize = GameRule::getNumberOfShipsOfSizeWhenBoardSize(size, shipSize);
        // finde heraus wie viele Schiffe der aktuellen Größe bei dem Board der gegebenen Größe benötigt werden
        shipsLeftToSet.insert(std::make_pair(shipSize, neededShipsOfSize));
        // inseriere die Anzahl der benötigten Schiffe der jeweiligen Größe in die Map
        totalShipsNotSunk += neededShipsOfSize;
        // zähle die Anzahl der Schiffe zu bisherigen Zahl der Schiffe hinzu
    }
    // wiederhole für alle Schiffe der Größe 2 bis 5
}

Board::Board() : Board(10) { /* da default size = 10 */ };

void Board::reset() {
    // speichere die Größe des Boards
    for (int xPos = 0; xPos < size; xPos++) {
        for (int yPos = 0; yPos < size; yPos++) {
            shipField.at(xPos).at(yPos) = false;
            guessField.at(xPos).at(yPos) = GuessStatus::notGuessed;
        }
    }
    // initialisiere beide Felder
    int shipSize = 2;
    // beginne bei Schiffen mit der Größe 2
    int neededShipsOfSize = GameRule::getNumberOfShipsOfSizeWhenBoardSize(size, shipSize);
    // finde heraus wie viele Schiffe der aktuellen Größe bei dem Board der gegebenen Größe benötigt werden
    totalShipsNotSunk = 0;
    while(neededShipsOfSize != 0) {
        shipsLeftToSet.find(shipSize)->second = neededShipsOfSize;
//        shipsNotSunk.insert(std::make_pair(shipSize, neededShipsOfSize));
        // inseriere die Anzahl der benötigten Schiffe der jeweiligen Größe in die Maps
        totalShipsNotSunk += neededShipsOfSize;
        // zähle die Anzahl der Schiffe zu bisherigen Zahl der Schiffe hinzu
        shipSize++;
        // gehe zu den nächstgrößeren Schiffen
        neededShipsOfSize = GameRule::getNumberOfShipsOfSizeWhenBoardSize(size, shipSize);
        // finde heraus wie viele Schiffe der aktuellen Größe bei dem Board der gegebenen Größe benötigt werden
    }
    // wiederhole so lange, bis die Schiffsgröße so groß ist, dass kein Schiff der Größe benötigt wird
    possibleToSetAllShips = true;
}


bool Board::addShip(int shipSize, Coordinates coordinates,
                    Direction direction) { // xPos und yPos sind die Positionen im Array, also 0-9
    if (GameRule::shipAddCorrect(shipSize, coordinates, direction, std::move(createCopy()))) {
        // wenn shipAddCorrect true zurückgibt, dann Platzierung des Schiffs erlaubt
        // → Anzahl zu setzende Schiffe der Größe um eins reduzieren und Felder setzen
        shipsLeftToSet.find(shipSize)->second = shipsLeftToSet.find(shipSize)->second - 1;
        int fieldsLeftToSet = shipSize;
        // Anzahl der Felder die noch gesetzt werden müssen
        while (fieldsLeftToSet > 0) {
            // solange noch Felder gesetzt werden müssen
            shipField.at(coordinates.x).at(coordinates.y) = true;
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
    if (!GameRule::insideField(coordinates, size) || guessField.at(coordinates.x).at(coordinates.y) != GuessStatus::notGuessed) {
        // das angegebene Feld ist entweder außerhalb des Boards oder wurde schon guessed
        return GuessStatus::guessImpossible;
    }
    if (!shipField.at(coordinates.x).at(coordinates.y)) {
        // am angegebenen Feld befindet sich kein Schiff
        guessField.at(coordinates.x).at(coordinates.y) = GuessStatus::guessedWrong;
        return GuessStatus::guessedWrong;
    }
    if (GameRule::shipDestroyed(coordinates, std::move(createCopy()))) {
        // wird das angegebene Feld aufgedeckt, wird dadurch ein Schiff komplett zerstört
        guessField.at(coordinates.x).at(coordinates.y) = GuessStatus::sunkShip;
        Coordinates appliedDirectionCoordinates = coordinates;
        for (Direction dir: Coordinates::getListOfAllDirections()) {
            appliedDirectionCoordinates = Coordinates::applyDirectionChange(coordinates, dir);
            //gehe die in alle 4 Richtungen angrenzenden Felder durch
            if (GameRule::insideField(appliedDirectionCoordinates, size)
                && shipField.at(appliedDirectionCoordinates.x).at(appliedDirectionCoordinates.y)) {
                setShipInThisDirectionSunk(coordinates, dir);
                // wenn sich im aktuell behandelten (angrenzenden) Feld ein Teil des Schiffs befindet,
                // setze alle in diese Richtung folgenden Schiffsfelder und damit den Schiffsteil,
                // der in diese Richtung zeigt auf Status sunk
            }
        }
        totalShipsNotSunk--;
        if(totalShipsNotSunk <= 0) {
        }
        return GuessStatus::sunkShip;
    }
    // wird das angegebene Feld aufgedeckt, wird dadurch kein Schiff komplett zerstört, aber eines getroffen
    guessField.at(coordinates.x).at(coordinates.y) = GuessStatus::guessedRight;
    return GuessStatus::guessedRight;

}

// setzt alle Felder in angegebener Richtung, auf denen sich ein Schiff befindet,
// auf Status sunk bis es auf das Ende des Boards erreicht oder auf ein Feld ohne Schiff stößt
int Board::setShipInThisDirectionSunk(Coordinates coordinates, Direction direction) {
    coordinates = Coordinates(coordinates.x, coordinates.y);
    int shipFieldsFound = 0;
    while (true) {
        coordinates = Coordinates::applyDirectionChange(coordinates, direction);
        // nächstes Feld in die angegebene Richtung wird gewählt
        if (!GameRule::insideField(coordinates, size) || guessField.at(coordinates.x).at(coordinates.y) != GuessStatus::guessedRight) {
            return shipFieldsFound;
            // befindet sich das behandelte Feld außerhalb des Boards oder befindet sich darauf kein Schiff, wird abgebrochen
        }
        guessField.at(coordinates.x).at(coordinates.y) = GuessStatus::sunkShip;
        shipFieldsFound++;
    }
}

// gibt den Wert des GuessFields an der Stelle von coordinates zurück
GuessStatus Board::guessFieldStatus(Coordinates coordinates) {
    return guessField.at(coordinates.x).at(coordinates.y);
}

void Board::printGuessField() {
    std::cout << std::endl << "    ";
    for (int x = 1; x <= size; x++) {
        std::cout << " " << x;
        if (x < 10) {
            std::cout << " ";
        }
    }
    std::cout << "   " << std::endl;
    // oberer Rand
    std::string output;
    for (int y = 0; y < size; y++) {
        output = " ";
        output += static_cast<char>(65 + y);
        // 65 + y sind die Buchstaben von A bis ..., da ASCII-Wert von A=65, B=66, ...
        output += "   ";
        // seitlicher Rand
        for (auto &column: guessField) {
            if (column.at(y) == GuessStatus::sunkShip) {
                output += "#  ";
            } else if (column.at(y) == GuessStatus::guessedRight) {
                output += "x  ";
            } else if (column.at(y) == GuessStatus::guessedWrong) {
                output += "   ";
            } else if (column.at(y) == GuessStatus::notGuessed) {
                output += "~  ";
            }
            // Zeile des Guess Fields
        }
        std::cout << output << "     " << std::endl;
    }
    std::cout << std::endl;
}

void Board::printShipField() {
    std::cout << std::endl << "    ";
    for (int x = 1; x <= size; x++) {
        std::cout << " " << x;
        if (x < 10) {
            std::cout << " ";
        }
    }
    std::cout << "   " << std::endl;
    // oberer Rand
    std::string output;
    for (int y = 0; y < size; y++) {
        output = " ";
        output += static_cast<char>(65 + y);
        // 65 + y sind die Buchstaben von A bis ..., da ASCII-Wert von A=65, B=66, ...
        output += "   ";
        // seitlicher Rand
        for (auto &column: shipField) {
            if (column.at(y)) {
                output += "@  ";
            } else {
                output += "~  ";
            }
            // Zeile des Guess Fields
        }
        std::cout << output << "     " << std::endl;
    }
    std::cout << std::endl;
}

std::unique_ptr<Board> Board::createCopy() {
    // Erstelle einen neuen std::unique_ptr, der auf eine Kopie des aktuellen Objekts zeigt
    std::unique_ptr<Board> boardCopy = std::make_unique<Board>(size);
    boardCopy->size = this->size;
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            boardCopy->guessField.at(x).at(y) = guessField.at(x).at(y);
            boardCopy->shipField.at(x).at(y) = shipField.at(x).at(y);
        }
    }
    for (auto shipSizePair: this->shipsLeftToSet) {
        if (boardCopy->shipsLeftToSet.find(shipSizePair.first) != boardCopy->shipsLeftToSet.end()) {
            boardCopy->shipsLeftToSet.find(shipSizePair.first)->second = shipSizePair.second;
        } else {
            boardCopy->shipsLeftToSet.insert(shipSizePair);
        }
    }
    boardCopy->totalShipsNotSunk = this->totalShipsNotSunk;
    return boardCopy;
}

void Board::allShipsAlreadySet() {
    for (int shipSize = 2; shipSize <= 5; ++shipSize) {
        if (shipsLeftToSet.find(shipSize) != shipsLeftToSet.end()) {
            shipsLeftToSet.find(shipSize)->second = 0;
        }
    }
}

