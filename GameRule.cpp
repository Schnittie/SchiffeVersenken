//
// Created by jonif on 06.01.2024.
//

#include <algorithm>
#include <iostream>
#include <cmath>
#include "GameRule.h"


bool GameRule::shipAddCorrect(int shipSize, Coordinates coordinates, Direction direction, std::unique_ptr<Board> board) {
    if (board->shipsLeftToSet.find(shipSize) == board->shipsLeftToSet.end() ||
        board->shipsLeftToSet.find(shipSize)->second <= 0 || !insideField(coordinates, board->size)) {
        // es dürfen keine Schiffe dieser Schiffsgröße gesetzt werden (sind entweder alle schon platziert
        // oder diese Schiffsgröße ist generell nicht erlaubt) oder Ursprungspunkt des Schiffs nicht auf dem Feld
        return false;
    }
    // testen, ob alle Schiffsfelder innerhalb von Board und nicht neben anderem Schiffsfeld
    int fieldsToSet = shipSize; // shipSize ist am Anfang Größe des zu setzenden Schiffs
    while (fieldsToSet > 0) {
        if (!insideField(coordinates, board->size) || board->shipFieldValue(coordinates)) {
            // aktuell zu behandelndes Feld ist nicht innerhalb des Boards (→ Schiff würde nicht komplett im Feld liegen)
            // oder befindet sich an einem Platz, wo bereits ein anderes Schiff ist
            return false;
        }
        for (Direction dir: Coordinates::getListOfAllDirections()) {
            Coordinates appliedDirectionCoordinates = Coordinates::applyDirectionChange(coordinates, dir);
            // gehe alle umliegenden Felder durch
            if (insideField(appliedDirectionCoordinates, board->size) &&
                board->shipFieldValue(appliedDirectionCoordinates)) {
                // auf behandeltem Feld befindet sich ein Schiff
                // → zu setzendes Schiff würde an einem anderen Schiff anliegen
                return false;
            }
        }
        coordinates = Coordinates::applyDirectionChange(coordinates, direction);
        fieldsToSet--;
        // gehe in Parametern angegebene Richtung einen Schritt weiter

    }
    return true;
    // alle Felder haben sich auf dem Board befunden und kein Feld hat an ein Schiff angegrenzt
}

// gebe true zurück, wenn ein Schiff durch Guess des Felds mit den angegebenen Koordinaten zerstört wird, ansonsten false
bool GameRule::shipDestroyed(Coordinates coordinates, std::unique_ptr<Board> board) {
    if (insideField(coordinates, board->size) && board->shipFieldValue(coordinates)) {
        // auf angegebenem Feld befindet sich tatsächlich ein Schiff
        Coordinates appliedDirectionCoordinates = coordinates;
        for (Direction dir: Coordinates::getListOfAllDirections()) {
            appliedDirectionCoordinates = Coordinates::applyDirectionChange(coordinates, dir);
            // gehe alle umliegenden Felder durch
            if (insideField(appliedDirectionCoordinates, board->size)
                && board->shipFieldValue(appliedDirectionCoordinates)
                && !shipInThisDirectionUncovered(coordinates, std::move(board->createCopy()), dir)) {
                return false;
                // wenn sich im behandelten Feld zwar ein Schiff befindet, der Rest des Schiffs in die behandelte
                // Richtung aber noch nicht komplett aufgedeckt wurde, wird abgebrochen
            }
        }
    }
    return true;
    // wenn nicht abgebrochen wurde, wird durch den Guess auf das angegebene Feld ein Schiff zerstört
}


bool GameRule::shipInThisDirectionUncovered(Coordinates coordinates, std::unique_ptr<Board> board,
                                            Direction direction) {
    coordinates = Coordinates::applyDirectionChange(coordinates, direction);
    while (insideField(coordinates, board->size) && board->shipFieldValue(coordinates)) {
        // gehe jedes Feld in der angegebenen Richtung durch, bis entweder Feld außerhalb des Feldes gefunden oder es
        // befindet sich kein Schiff darauf → alle zusammenhängenden Schiffsfelder in der Richtung wurden erfolgreich behandelt
        if (board->guessFieldValue(coordinates) != GuessStatus::guessedRight) {
            return false;
            // es befindet sich ein Schiff auf dem behandelten Feld (da erster if-Fall nicht eingetroffen),
            // dieses wurde jedoch noch nicht aufgedeckt → das Schiff ist in dieser Richtung noch nicht komplett entdeckt
        }
        coordinates = Coordinates::applyDirectionChange(coordinates, direction);
    }
    return true;
}

// überprüfe, ob sich die gegebenen Koordinaten innerhalb des Boards befinden
bool GameRule::insideField(Coordinates coordinates, int boardSize) {
    if (coordinates.x < boardSize && coordinates.x >= 0 && coordinates.y < boardSize && coordinates.y >= 0) {
        return true;
    }
    return false;
}

int GameRule::getNumberOfShipsOfSizeWhenBoardSize(int boardSize, int shipSize) {
    switch (shipSize) {
        case 2:
            return round(boardSize * boardSize * 0.04);
        case 3:
            return round(boardSize * boardSize * 0.03);
        case 4:
            return round(boardSize * boardSize * 0.02);
        case 5:
            return round(boardSize * boardSize * 0.01);
        default:
            return 0;
    }
}
