//
// Created by laure on 22.12.2023.
//

#include "Opponent.h"
#include "CppRandom.hpp"
#include "Coordinates.h"

std::unique_ptr<Board> Opponent::addRandomShipOfGivenSize(std::unique_ptr<Board> board, int shipSize) {
    // probiert zufällige Kombinationen aus bis Schiff platzierbar
    Coordinates coordinates = Coordinates(0, 0);
    Direction direction;
    do {
        coordinates = Coordinates(GetRandomNumberBetween(0, 9), GetRandomNumberBetween(0, 9));
        std::vector<Direction> allDirections = Coordinates::getListOfAllDirections();
        direction = allDirections.at(GetRandomNumberBetween(0, allDirections.size()-1));
    } while (!board->addShip(shipSize, coordinates, direction));
    // probiert so lange aus ein Schiff der gegebenen Länge an einer zufälligen Koordinate
    // gerichtet in eine zufällige Richtung zu setzen, bis es klappt
    return std::move(board);
}

std::unique_ptr<Board> Opponent::placeAllShips(std::unique_ptr<Board> board) {
    for (int shipSize = 5; shipSize > 1; shipSize--) {
        for (int shipNumber = 0; shipNumber < 6 - shipSize; shipNumber++) {
            board = addRandomShipOfGivenSize(std::move(board), shipSize);
        }
    }
    // setzt alle zu platzierenden Schiffe zufällig (1 der Göße 5, 2 der Größe 4, 3 der Größe 3, 4 der Größe 2)
    return board;
}

std::unique_ptr<Board> Opponent::makeGuess(std::unique_ptr<Board> board) { // hier hab ich nur xPos yPos durch Coords getauscht aber sonst kein refactoring betrieben, weil wir uns diese Methode glaube ich sowieso nochmal anschauen müssen
    int makeCalculatedGuessNumber = GetRandomNumberBetween(0, 10) + smartness;
    Coordinates guessedRightFieldCoordinates = Coordinates(-1, -1);
    std::vector<Coordinates> unGuessedFields;
    for (int xPos = 0; xPos < 10; xPos++) {
        for (int yPos = 0; yPos < 10; yPos++) {
            if (board->guessField[xPos][yPos] == GuessStatus::notGuessed) {
                unGuessedFields.emplace_back(xPos, yPos);
            }
            if (board->guessField[xPos][yPos] == GuessStatus::guessedRight &&
                // Schiff wurde getroffen, aber noch nicht zerstört
                makeCalculatedGuessNumber >= 10) {
                // speichert die Koordinaten an, denen ein Schiff getroffen wurde in die Variablen ab
                guessedRightFieldCoordinates = Coordinates(xPos, yPos);
            }
        }
    }
    Coordinates coordinatesNextToGuessedRightField = Coordinates(0,0);
    if (guessedRightFieldCoordinates.x != -1 && guessedRightFieldCoordinates.y != -1) {
        // wenn ein Feld gefunden wurde, an dem ein Schiff getroffen aber nicht zerstört wurde, wird geschaut,
        // welches der umliegenden Felder noch nicht beschossen wurde (es muss zwingend 1 geben)
        // → Resultat in Variablen gespeichert
        if (guessedRightFieldCoordinates.x + 1 <= 9 &&
            board->guessField[guessedRightFieldCoordinates.x][guessedRightFieldCoordinates.y - 1] == GuessStatus::notGuessed) {
            coordinatesNextToGuessedRightField.x = guessedRightFieldCoordinates.x + 1;
            coordinatesNextToGuessedRightField.y = coordinatesNextToGuessedRightField.y;
        } else if (guessedRightFieldCoordinates.x - 1 >= 0 &&
                   board->guessField[guessedRightFieldCoordinates.x - 2][guessedRightFieldCoordinates.y - 1] == GuessStatus::notGuessed) {
            coordinatesNextToGuessedRightField.x = guessedRightFieldCoordinates.x - 1;
            coordinatesNextToGuessedRightField.y = coordinatesNextToGuessedRightField.y;
        } else if (coordinatesNextToGuessedRightField.y + 1 <= 9 &&
                   board->guessField[guessedRightFieldCoordinates.x - 1][guessedRightFieldCoordinates.y] == GuessStatus::notGuessed) {
            coordinatesNextToGuessedRightField.x = guessedRightFieldCoordinates.x;
            coordinatesNextToGuessedRightField.y = coordinatesNextToGuessedRightField.y + 1;
        } else if (coordinatesNextToGuessedRightField.y - 1 >= 0 &&
                   board->guessField[guessedRightFieldCoordinates.x - 1][guessedRightFieldCoordinates.y - 2] == GuessStatus::notGuessed) {
            coordinatesNextToGuessedRightField.x = guessedRightFieldCoordinates.x;
            coordinatesNextToGuessedRightField.y = coordinatesNextToGuessedRightField.y - 1;
        }
        if (board->makeGuess(coordinatesNextToGuessedRightField) !=
            GuessStatus::guessImpossible) {
            return std::move(board);
        } else {
            return guessRandom(std::move(board), unGuessedFields);
        }
    } else {
        return guessRandom(std::move(board), unGuessedFields);
    }
}

std::unique_ptr<Board> Opponent::guessRandom(std::unique_ptr<Board> board, std::vector<Coordinates> unGuessedFields) {
    int randomFreeField = GetRandomNumberBetween(0, unGuessedFields.size() - 1);
    board->makeGuess(unGuessedFields.at(randomFreeField));
    return std::move(board);
    // guessed ein zufälliges Feld der gegebenen Liste der noch verdeckten Felder
}

