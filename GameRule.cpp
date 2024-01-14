//
// Created by jonif on 06.01.2024.
//

#include <algorithm>
#include <iostream>
#include "GameRule.h"


bool GameRule::shipAddCorrect(int shipSize, Coordinates coordinates, Direction direction, std::unique_ptr<Board> board) {
    if (shipSize > 5 || shipSize < 2 || !insideField(coordinates)) {
        // angegebene Schiffsgröße nicht im akzeptierten Bereich oder Ursprungspunkt des Schiffs nicht auf dem Feld
        return false;
    }
    // testen, ob alle Schiffsfelder innerhalb von Board und nicht neben anderem Schiffsfeld
    int fieldsToSet = shipSize; // shipSize ist am Anfang Größe des zu setzenden Bootes
    while (fieldsToSet > 0) {
        if (!insideField(coordinates) || board->shipField[coordinates.x][coordinates.y]) {
            // aktuell zu behandelndes Feld ist nicht innerhalb des Boards (→ Schiff würde nicht komplett im Feld liegen)
            // oder befindet sich an einem Platz, wo bereits ein anderes Schiff ist
            return false;
        }
        for (Direction dir: Coordinates::getListOfAllDirections()) {
            Coordinates appliedDirectionCoordinates = Coordinates::applyDirectionChange(coordinates, dir);
            // gehe alle umliegenden Felder durch
            if (insideField(appliedDirectionCoordinates) &&
                board->shipField[appliedDirectionCoordinates.x][appliedDirectionCoordinates.y]) {
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
    if (insideField(coordinates) && board->shipField[coordinates.x][coordinates.y]) {
        // auf angegebenem Feld befindet sich tatsächlich ein Schiff
        for (Direction dir: Coordinates::getListOfAllDirections()) {
            coordinates = Coordinates::applyDirectionChange(coordinates, dir);
            // gehe alle umliegenden Felder durch
            if (insideField(coordinates)
                && board->shipField[coordinates.x][coordinates.y]
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
    while (true) {
        coordinates = Coordinates::applyDirectionChange(coordinates, direction);
        // gehe jedes Feld in der angegebenen Richtung durch, bis abgebrochen wird
        if (!insideField(coordinates) || !board->shipField[coordinates.x][coordinates.y]) {
            return true;
            // behandeltes Feld liegt entweder außerhalb des Feldes oder es befindet sich kein Schiff darauf
            // → alle zusammenhängenden Schiffsfelder in der Richtung wurden erfolgreich behandelt
        }
        if (board->guessField[coordinates.x][coordinates.y] != GuessStatus::guessedRight) {
            return false;
            // es befindet sich ein Schiff auf dem behandelten Feld (da erster if-Fall nicht eingetroffen),
            // dieses wurde jedoch noch nicht aufgedeckt → das Schiff ist in dieser Richtung noch nicht komplett entdeckt
        }

    }
}

// überprüfe, ob sich die gegebenen Koordinaten innerhalb des Boards befinden
bool GameRule::insideField(Coordinates coordinates) {
    if (coordinates.x <= 9 && coordinates.x >= 0 && coordinates.y <= 9 && coordinates.y >= 0) {
        return true;
    }
    return false;
}
