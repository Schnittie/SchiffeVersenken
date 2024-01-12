//
// Created by jonif on 06.01.2024.
//

#include <algorithm>
#include <iostream>
#include "GameRule.h"


bool
GameRule::shipAddCorrect(int shipSize, Coordinates coordinates, Direction direction, std::unique_ptr<Board> board) {
    // teste, ob Ursprungspunkt des Schiffs auf dem Feld
    if (shipSize > 5 || shipSize < 2 || !insideField(coordinates)) {
        return false;
    }
    // testen, ob weitere Schiffsfelder innerhalb von Board und nicht neben anderem Schiffsfeld
    int fieldsToSet = shipSize; // shipSize ist am Anfang Größe des zu setzenden bootes
    Coordinates currentCoordinates = Coordinates(coordinates.x, coordinates.y);
    while (fieldsToSet > 0) {
        // teste, ob aktuell zu behandelndes Feld unbelegt ist
        if (!insideField(currentCoordinates) || board->shipField[currentCoordinates.x][currentCoordinates.y]) {
            // → Schiff würde nicht komplett im Feld liegen
            return false;
        }
        // teste, ob Feld links davon unbelegt ist oder außerhalb des Feldes liegt
        for (Direction dir: Coordinates::getListOfAllDirections()) {
            Coordinates appliedDirectionCoordinates = Coordinates::applyDirectionChange(currentCoordinates, dir);
            if (insideField(appliedDirectionCoordinates) &&
                board->shipField[appliedDirectionCoordinates.x][appliedDirectionCoordinates.y]) {
                //es liegt an einem anderen Schiff an
                return false;
            }
        }
        currentCoordinates = Coordinates::applyDirectionChange(currentCoordinates, direction);
        fieldsToSet--;

    }
    return true;
}

bool GameRule::shipDestroyed(Coordinates coordinates, std::unique_ptr<Board> board) {
    if (insideField(coordinates) && board->shipField[coordinates.x][coordinates.y]) {
        for (Direction dir: Coordinates::getListOfAllDirections()) {
            Coordinates appliedDirectionCoordinates = Coordinates::applyDirectionChange(coordinates, dir);
            if (insideField(appliedDirectionCoordinates)
                && board->shipField[appliedDirectionCoordinates.x][appliedDirectionCoordinates.y]
                && !shipInThisDirectionDestroyed(appliedDirectionCoordinates, std::move(board->createCopy()), dir)) {
                return false;
            }
        }
    }
    return true;
}


bool
GameRule::shipInThisDirectionDestroyed(Coordinates coordinates, std::unique_ptr<Board> board,
                                       Direction direction) {
    Coordinates currentCoordinates = Coordinates(coordinates.x, coordinates.y);
    while (true) {
        currentCoordinates = Coordinates::applyDirectionChange(currentCoordinates, direction);
        if (!insideField(currentCoordinates) || !board->shipField[currentCoordinates.x][currentCoordinates.y]) {
            // Feld 1 weiter liegt entweder außerhalb des Feldes oder es befindet sich kein Schiff darauf
            return true;
        }
        if (board->guessField[currentCoordinates.x][currentCoordinates.y] != GuessStatus::guessedRight) {
            // es befindet sich ein Schiff auf dem Feld 1 weiter (da erster if-Fall nicht eingetroffen und dieses wurde bereits an diesem Feld getroffen
            return false;
        }

    }
}

bool GameRule::insideField(int coordinate) {
    if (coordinate <= 9 && coordinate >= 0) {
        return true;
    }
    return false;
}

bool GameRule::insideField(Coordinates coordinates) {
    if (coordinates.x <= 9 && coordinates.x >= 0 && coordinates.y <= 9 && coordinates.y >= 0) {
        return true;
    }
    return false;
}
