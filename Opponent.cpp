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
    int maxRepetitionsCounter = 0;
    do {
        coordinates = Coordinates(GetRandomNumberBetween(0, board->size - 1),
                                  GetRandomNumberBetween(0, board->size - 1));
        std::vector<Direction> allDirections = Coordinates::getListOfAllDirections();
        direction = allDirections.at(GetRandomNumberBetween(0, allDirections.size()-1));
        maxRepetitionsCounter++;
        if (maxRepetitionsCounter == 10000) {
            board->possibleToSetAllShips = false;
            // es scheint keine Möglichkeit auf dem Board zu geben, das verbleibende Schiff zu setzen
            // → breche Platzierungsvorgang ab und markiere die Setzung des nächsten Schiffs als unmöglich
            // durch Änderung der Variable possibleToSetAllShips im Board auf false
        }
    } while (!board->addShip(shipSize, coordinates, direction) &&
        board->shipsLeftToSet.find(shipSize) != board->shipsLeftToSet.end() &&
        board->shipsLeftToSet.find(shipSize)->second > 0 && maxRepetitionsCounter < 10000);
    // probiert so lange aus ein Schiff der gegebenen Länge an einer zufälligen Koordinate
    // gerichtet in eine zufällige Richtung zu setzen, bis es klappt
    return std::move(board);
}

std::unique_ptr<Board> Opponent::placeAllShips(std::unique_ptr<Board> board) {
    int shipSize = 5;
    while (board->shipsLeftToSet.find(shipSize) != board->shipsLeftToSet.end()) {
        if (board->shipsLeftToSet.find(shipSize)->second <= 0) {
            shipSize--;
        } else {
            board = addRandomShipOfGivenSize(std::move(board), shipSize);
        }
        if (!board->possibleToSetAllShips) {
            board->reset();
            shipSize = 5;
            // der letzte Platzierungsversuch ist gescheitert, da die Variable auf false gesetzt wurde
            // → Werte zurücksetzen und nochmal von vorne beginnen
        }
    }
    // setzt alle zu platzierenden Schiffe zufällig
    return board;
}

std::unique_ptr<Board> Opponent::makeGuess(std::unique_ptr<Board> board, int smartness) { // hier hab ich nur xPos yPos durch Coords getauscht aber sonst kein refactoring betrieben, weil wir uns diese Methode glaube ich sowieso nochmal anschauen müssen
    // "Schwierigkeitsscore" (0-10)
    // → legt fest mit welcher Wahrscheinlichkeit die Entscheidungen des computergesteuerten Gegners taktisch gefällt werden
    int makeCalculatedGuessNumber = GetRandomNumberBetween(0, 10) + smartness;
    std::vector<Coordinates> unGuessedFields;
    std::vector<Coordinates> couldBeShip;
    std::vector<Coordinates> betweenShips;
    for (int xPos = 0; xPos < board->size; xPos++) {
        for (int yPos = 0; yPos < board->size; yPos++) {
            if (board->guessField.at(xPos).at(yPos) == GuessStatus::notGuessed) {
                unGuessedFields.emplace_back(xPos, yPos);
            }
            if (board->guessField.at(xPos).at(yPos) == GuessStatus::guessedRight) {
                // Schiff wurde getroffen, aber noch nicht zerstört
                Coordinates coordinates = Coordinates(xPos,yPos);
                int alreadyDiscoveredFieldsOfShip = getNumberOfDiscoveredFieldsOfShip(std::move(board->createCopy()), coordinates);
                Coordinates appliedDirectionCoordinates = coordinates;
                for (Direction direction: Coordinates::getListOfAllDirections()) {
                    appliedDirectionCoordinates = Coordinates::applyDirectionChange(coordinates, direction);
                    if (GameRule::insideField(appliedDirectionCoordinates, board->size) &&
                            board->guessFieldValue(appliedDirectionCoordinates) == GuessStatus::notGuessed) {
                        Coordinates coordinatesInOppositeDirection =
                                Coordinates::applyDirectionChange(coordinates, Coordinates::getOppositeDirection(direction));
                        Coordinates coordinatesTwoTimesInDirection =
                                Coordinates::applyDirectionChange(appliedDirectionCoordinates, direction);
                        if (alreadyDiscoveredFieldsOfShip >= 2 &&
                            GameRule::insideField(coordinatesInOppositeDirection, board->size) &&
                                board->guessFieldValue(coordinatesInOppositeDirection) == GuessStatus::guessedRight) {
                            if (alreadyDiscoveredFieldsOfShip <= 3 &&
                                GameRule::insideField(coordinatesTwoTimesInDirection, board->size) &&
                                    board->guessFieldValue(coordinatesTwoTimesInDirection) == GuessStatus::guessedRight) {
                                betweenShips.push_back(appliedDirectionCoordinates);
                            } else {
                                couldBeShip.push_back(appliedDirectionCoordinates);
                            }
                        } else if (alreadyDiscoveredFieldsOfShip == 1) {
                            if (GameRule::insideField(coordinatesTwoTimesInDirection, board->size) &&
                                    board->guessFieldValue(coordinatesTwoTimesInDirection) == GuessStatus::guessedRight) {
                                betweenShips.push_back(appliedDirectionCoordinates);
                            } else {
                                couldBeShip.push_back(appliedDirectionCoordinates);
                            }
                        }
                    }
                }
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
            GuessStatus guessResult = board->makeGuess(coordinates);
            if (guessResult != GuessStatus::guessImpossible) {
                switch (guessResult) {
                    case GuessStatus::sunkShip:
                        std::cout << std::endl << "Der Gegner hat ein Schiff versenkt!" << std::endl;
                        break;
                    case GuessStatus::guessedRight:
                        std::cout << std::endl << "Der Gegner hat ein Schiff getroffen!" << std::endl;
                        break;
                    case GuessStatus::guessedWrong:
                        std::cout << std::endl << "Der Gegner hat nichts getroffen!" << std::endl;
                        break;
                }
                return std::move(board);
            }
        }
        while (couldBeShip.size() > 0) {
            randomFreeField = GetRandomNumberBetween(0, couldBeShip.size() - 1);
            coordinates = couldBeShip.at(randomFreeField);
            couldBeShip.erase(couldBeShip.begin() + randomFreeField);
            GuessStatus guessResult = board->makeGuess(coordinates);
            if (guessResult != GuessStatus::guessImpossible) {
                switch (guessResult) {
                    case GuessStatus::sunkShip:
                        std::cout << std::endl << "Der Gegner hat ein Schiff versenkt!" << std::endl;
                        break;
                    case GuessStatus::guessedRight:
                        std::cout << std::endl << "Der Gegner hat ein Schiff getroffen!" << std::endl;
                        break;
                    case GuessStatus::guessedWrong:
                        std::cout << std::endl << "Der Gegner hat nichts getroffen!" << std::endl;
                        break;
                }
                return std::move(board);
            }
        }
        return guessRandom(std::move(board), unGuessedFields);
    }
    return guessRandom(std::move(board), unGuessedFields);
}

std::unique_ptr<Board> Opponent::guessRandom(std::unique_ptr<Board> board, std::vector<Coordinates> unGuessedFields) {
    int randomFreeField = GetRandomNumberBetween(0, unGuessedFields.size() - 1);
    GuessStatus guessResult = board->makeGuess(unGuessedFields.at(randomFreeField));
    switch (guessResult) {
        case GuessStatus::sunkShip:
            std::cout << std::endl << "Der Gegner hat ein Schiff versenkt!" << std::endl;
            break;
        case GuessStatus::guessedRight:
            std::cout << std::endl << "Der Gegner hat ein Schiff getroffen!" << std::endl;
            break;
        case GuessStatus::guessedWrong:
            std::cout << std::endl << "Der Gegner hat nichts getroffen!" << std::endl;
            break;
    }
    return std::move(board);
    // guessed ein zufälliges Feld der gegebenen Liste der noch verdeckten Felder
}

int Opponent::getNumberOfDiscoveredFieldsOfShip(std::unique_ptr<Board> board, Coordinates coordinates) {
    if (GameRule::insideField(coordinates, board->size) &&
        (board->guessFieldValue(coordinates) == GuessStatus::guessedRight ||
        board->guessFieldValue(coordinates) == GuessStatus::sunkShip)) {
        Coordinates appliedDirectionCoordinates = coordinates;
        int numberOfFields = 1;
        for (Direction direction: Coordinates::getListOfAllDirections()) {
            appliedDirectionCoordinates = Coordinates::applyDirectionChange(coordinates, direction);
            if (GameRule::insideField(appliedDirectionCoordinates, board->size) &&
                (board->guessFieldValue(appliedDirectionCoordinates) == GuessStatus::guessedRight
                || board->guessFieldValue(appliedDirectionCoordinates) == GuessStatus::sunkShip)) {
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
    while(GameRule::insideField(coordinates, board->size) &&
        (board->guessFieldValue(coordinates) == GuessStatus::guessedRight ||
        board->guessFieldValue(coordinates) == GuessStatus::sunkShip)) {
        numberOfFields++;
        coordinates = Coordinates::applyDirectionChange(coordinates, direction);
    }
    return numberOfFields;
}



