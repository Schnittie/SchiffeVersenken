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
    counter++;
    int makeCalculatedGuessNumber = GetRandomNumberBetween(0, 10) + smartness;
//    Coordinates guessedRightFieldCoordinates = Coordinates(-1, -1);
    std::vector<Coordinates> unGuessedFields;
    std::vector<Coordinates> couldBeShip;
    std::vector<Coordinates> betweenShips;
    for (int xPos = 0; xPos < 10; xPos++) {
        for (int yPos = 0; yPos < 10; yPos++) {
            if (board->guessField[xPos][yPos] == GuessStatus::notGuessed) {
                unGuessedFields.emplace_back(xPos, yPos);
            }
            if (board->guessField[xPos][yPos] == GuessStatus::guessedRight) {
                // Schiff wurde getroffen, aber noch nicht zerstört
                Coordinates coordinates = Coordinates(xPos,yPos);
                int alreadyDiscoveredFieldsOfShip = getNumberOfDiscoveredFieldsOfShip(std::move(board->createCopy()), coordinates);
                Coordinates appliedDirectionCoordinates = coordinates;
                for (Direction direction: Coordinates::getListOfAllDirections()) {
                    appliedDirectionCoordinates = Coordinates::applyDirectionChange(coordinates, direction);
                    if (GameRule::insideField(appliedDirectionCoordinates) &&
                        board->guessFieldStatus(appliedDirectionCoordinates) == GuessStatus::notGuessed) {
                        if (alreadyDiscoveredFieldsOfShip >= 2 &&
                            board->guessStatusOfFieldInDirection(coordinates, Coordinates::getOppositeDirection(direction))
                            == GuessStatus::guessedRight) {
                            if (board->guessStatusOfFieldInDirection(appliedDirectionCoordinates, direction)
                                == GuessStatus::guessedRight && alreadyDiscoveredFieldsOfShip <= 3) {
                                betweenShips.push_back(appliedDirectionCoordinates);
                            } else {
                                couldBeShip.push_back(appliedDirectionCoordinates);
                            }
                        } else if (alreadyDiscoveredFieldsOfShip == 1) {
                            if (board->guessStatusOfFieldInDirection(appliedDirectionCoordinates, direction) == GuessStatus::guessedRight) {
                                betweenShips.push_back(appliedDirectionCoordinates);
                            } else {
                                couldBeShip.push_back(appliedDirectionCoordinates);
                            }
                        }
                    }
                }
                // speichert die Koordinaten an, denen ein Schiff getroffen wurde in die Variablen ab
//                guessedRightFieldCoordinates = Coordinates(xPos, yPos);
            }
        }
    }
    if (makeCalculatedGuessNumber >= 10) {
        int randomFreeField;
        Coordinates coordinates = Coordinates(0,0);
        while (betweenShips.size() > 0) {
            randomFreeField = GetRandomNumberBetween(0, betweenShips.size() - 1);
            coordinates = betweenShips.at(randomFreeField);
            betweenShips.erase(betweenShips.begin() + randomFreeField);
            if (board->makeGuess(coordinates) != GuessStatus::guessImpossible) {
                return std::move(board);
            }
        }
        while (couldBeShip.size() > 0) {
            randomFreeField = GetRandomNumberBetween(0, couldBeShip.size() - 1);
            coordinates = couldBeShip.at(randomFreeField);
            couldBeShip.erase(couldBeShip.begin() + randomFreeField);
            if (board->makeGuess(coordinates) != GuessStatus::guessImpossible) {
                return std::move(board);
            }
        }
        return guessRandom(std::move(board), unGuessedFields);
    }
    return guessRandom(std::move(board), unGuessedFields);
//    Coordinates coordinatesNextToGuessedRightField = Coordinates(0,0);
//    if (guessedRightFieldCoordinates.x != -1 && guessedRightFieldCoordinates.y != -1) {
//        // wenn ein Feld gefunden wurde, an dem ein Schiff getroffen aber nicht zerstört wurde, wird geschaut,
//        // welches der umliegenden Felder noch nicht beschossen wurde (es muss zwingend 1 geben)
//        // → Resultat in Variablen gespeichert
//        if (guessedRightFieldCoordinates.x + 1 <= 9 &&
//            board->guessField[guessedRightFieldCoordinates.x][guessedRightFieldCoordinates.y - 1] == GuessStatus::notGuessed) {
//            coordinatesNextToGuessedRightField.x = guessedRightFieldCoordinates.x + 1;
//            coordinatesNextToGuessedRightField.y = coordinatesNextToGuessedRightField.y;
//        } else if (guessedRightFieldCoordinates.x - 1 >= 0 &&
//                   board->guessField[guessedRightFieldCoordinates.x - 2][guessedRightFieldCoordinates.y - 1] == GuessStatus::notGuessed) {
//            coordinatesNextToGuessedRightField.x = guessedRightFieldCoordinates.x - 1;
//            coordinatesNextToGuessedRightField.y = coordinatesNextToGuessedRightField.y;
//        } else if (coordinatesNextToGuessedRightField.y + 1 <= 9 &&
//                   board->guessField[guessedRightFieldCoordinates.x - 1][guessedRightFieldCoordinates.y] == GuessStatus::notGuessed) {
//            coordinatesNextToGuessedRightField.x = guessedRightFieldCoordinates.x;
//            coordinatesNextToGuessedRightField.y = coordinatesNextToGuessedRightField.y + 1;
//        } else if (coordinatesNextToGuessedRightField.y - 1 >= 0 &&
//                   board->guessField[guessedRightFieldCoordinates.x - 1][guessedRightFieldCoordinates.y - 2] == GuessStatus::notGuessed) {
//            coordinatesNextToGuessedRightField.x = guessedRightFieldCoordinates.x;
//            coordinatesNextToGuessedRightField.y = coordinatesNextToGuessedRightField.y - 1;
//        }
//        if () {
//            return std::move(board);
//        } else {
//            return guessRandom(std::move(board), unGuessedFields);
//        }
//    } else {
//        return guessRandom(std::move(board), unGuessedFields);
//    }
}

std::unique_ptr<Board> Opponent::guessRandom(std::unique_ptr<Board> board, std::vector<Coordinates> unGuessedFields) {
    int randomFreeField = GetRandomNumberBetween(0, unGuessedFields.size() - 1);
    board->makeGuess(unGuessedFields.at(randomFreeField));
    return std::move(board);
    // guessed ein zufälliges Feld der gegebenen Liste der noch verdeckten Felder
}

int Opponent::getNumberOfDiscoveredFieldsOfShip(std::unique_ptr<Board> board, Coordinates coordinates) {
    if (GameRule::insideField(coordinates) && (board->guessField[coordinates.x][coordinates.y] == GuessStatus::guessedRight
        || board->guessField[coordinates.x][coordinates.y] == GuessStatus::sunkShip)) {
        Coordinates appliedDirectionCoordinates = coordinates;
        int numberOfFields = 1;
        for (Direction direction: Coordinates::getListOfAllDirections()) {
            appliedDirectionCoordinates = Coordinates::applyDirectionChange(coordinates, direction);
            if (board->guessField[appliedDirectionCoordinates.x][appliedDirectionCoordinates.y] == GuessStatus::guessedRight
                || board->guessField[appliedDirectionCoordinates.x][appliedDirectionCoordinates.y] == GuessStatus::sunkShip) {
                numberOfFields += getNumberOfDiscoveredFieldsOfShipInThisDirection(std::move(board->createCopy()), coordinates, direction);
            }
        }
        return numberOfFields;
    } else {
        return 0;
    }
}

int Opponent::getNumberOfDiscoveredFieldsOfShipInThisDirection(std::unique_ptr<Board> board, Coordinates coordinates, Direction direction) {
    int numberOfFields = 0;
    coordinates = Coordinates::applyDirectionChange(coordinates, direction);
    while(GameRule::insideField(coordinates) && (board->guessField[coordinates.x][coordinates.y] == GuessStatus::guessedRight
                                                 || board->guessField[coordinates.x][coordinates.y] == GuessStatus::sunkShip)) {
        numberOfFields++;
        coordinates = Coordinates::applyDirectionChange(coordinates, direction);
    }
    return numberOfFields;
}



