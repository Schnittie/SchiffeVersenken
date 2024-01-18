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
        // wählt zufällige Koordinaten
        std::vector<Direction> allDirections = Coordinates::getListOfAllDirections();
        direction = allDirections.at(GetRandomNumberBetween(0, allDirections.size()-1));
        // wählt zufällige Richtung
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
        // wiederholt so lange, bis Schiffsgröße so klein, dass keine davon gesetzt werden sollen
        if (board->shipsLeftToSet.find(shipSize)->second <= 0) {
            shipSize--;
            // beginnt beim größten Schiff, wenn alle Schiffe dieser Größe gesetzt geht zum nächstkleineren Schiff
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
    return std::move(board);
}

std::unique_ptr<Board> Opponent::makeGuess(std::unique_ptr<Board> board, int difficulty) { // hier hab ich nur xPos yPos durch Coords getauscht aber sonst kein refactoring betrieben, weil wir uns diese Methode glaube ich sowieso nochmal anschauen müssen
    // "Schwierigkeitsscore" (0-10)
    // → legt fest mit welcher Wahrscheinlichkeit die Entscheidungen des computergesteuerten Gegners taktisch gefällt werden
    int makeCalculatedGuessNumber = GetRandomNumberBetween(0, 10) + difficulty;
    // wenn makeCalculatedGuessNumber < 10, dann wird zufällig geraten
    std::vector<Coordinates> unGuessedFields;
    std::vector<Coordinates> couldBeShip;
    std::vector<Coordinates> betweenShips;
    for (int xPos = 0; xPos < board->size; xPos++) {
        for (int yPos = 0; yPos < board->size; yPos++) {
            // gehe alle Felder des Boards durch
            if (board->guessField.at(xPos).at(yPos) == GuessStatus::notGuessed) {
                unGuessedFields.emplace_back(xPos, yPos);
                // füge alle Felder, auf die noch nicht guessed wurde in den Vektor unGuessedFields hinzu
            }
            if (board->guessField.at(xPos).at(yPos) == GuessStatus::guessedRight) {
                // Schiff wurde getroffen, aber noch nicht zerstört
                Coordinates coordinates = Coordinates(xPos,yPos);
                int alreadyDiscoveredFieldsOfShip = getNumberOfDiscoveredFieldsOfShip(std::move(board->createCopy()), coordinates);
                // erstelle ein Koordinatenobjekt vom Feld und schaue, wie viele Felder vom Schiff, das sich auf dem
                // aktuellen Feld befindet, schon getroffen wurden
                Coordinates appliedDirectionCoordinates = coordinates;
                for (Direction direction: Coordinates::getListOfAllDirections()) {
                    appliedDirectionCoordinates = Coordinates::applyDirectionChange(coordinates, direction);
                    // gehe alle anliegenden Felder durch
                    if (GameRule::insideField(appliedDirectionCoordinates, board->size) &&
                            board->guessFieldValue(appliedDirectionCoordinates) == GuessStatus::notGuessed) {
                        Coordinates coordinatesInOppositeDirection =
                                Coordinates::applyDirectionChange(coordinates, Coordinates::getOppositeDirection(direction));
                        Coordinates coordinatesTwoTimesInDirection =
                                Coordinates::applyDirectionChange(appliedDirectionCoordinates, direction);
                        // wenn das aktuell behandelte Feld (appliedDirectionCoordinates) innerhalb des Boards liegt,
                        // dann hole dir das Feld 1 weiter in die Richtung und das in die entgegengesetzte Richtung
                        // vom Ursprungsfeld aus
                        if (alreadyDiscoveredFieldsOfShip >= 2 &&
                            GameRule::insideField(coordinatesInOppositeDirection, board->size) &&
                                board->guessFieldValue(coordinatesInOppositeDirection) == GuessStatus::guessedRight) {
                            // wenn das Feld in der entgegengesetzten Richtung zum Ursprungsfeld auch ein Treffer war,
                            // dann liegt das aktuell behandelte Feld in einer Linie mit den bereits aufgedeckten Feldern
                            // des Schiffs, zu denen auch das Ursprungsfeld gehört
                            if (alreadyDiscoveredFieldsOfShip <= 3 &&
                                GameRule::insideField(coordinatesTwoTimesInDirection, board->size) &&
                                    board->guessFieldValue(coordinatesTwoTimesInDirection) == GuessStatus::guessedRight) {
                                betweenShips.push_back(appliedDirectionCoordinates);
                                // wenn der bereits getroffene Teil des Schiffs maximal 3 Felder groß ist und das Feld
                                // eins weiter in die ursprüngliche Richtung auch ein Treffer war, dann kann es sein,
                                // dass das Feld dazwischen zwei Schiffe verbindet (3 Felder + behandeltes Feld
                                // + getroffenes Feld 1 weiter = 5 Felder → maximale Größe)
                            } else {
                                couldBeShip.push_back(appliedDirectionCoordinates);
                                // ansonsten könnte das Feld trotzdem zum Schiff gehören
                            }
                        } else if (alreadyDiscoveredFieldsOfShip == 1) {
                            if (GameRule::insideField(coordinatesTwoTimesInDirection, board->size) &&
                                    board->guessFieldValue(coordinatesTwoTimesInDirection) == GuessStatus::guessedRight) {
                                betweenShips.push_back(appliedDirectionCoordinates);
                                // wenn das Feld eins weiter in die Richtung auch ein Treffer war, dann liegt das aktuell
                                // behandelte Feld zwischen zwei Schiffen/Schiffsteilen
                            } else {
                                couldBeShip.push_back(appliedDirectionCoordinates);
                                // wenn das nicht der Fall ist, könnte es sich trotzdem um ein Schiffsfeld handel, da
                                // ja kein anderes Schiff angrenzt
                            }
                        }
                    }
                }
            }
        }
    }
    if (makeCalculatedGuessNumber >= 10) {
        // führt nur aus, wenn der Zufallsgenerator und der Schwierigkeitsgrad es verursachen
        std::unique_ptr<Board> boardAfterGuess = Opponent::tryToHit(betweenShips,
                                                                           std::move(board->createCopy()));
        if (boardAfterGuess != nullptr){
            return boardAfterGuess;
        }
        boardAfterGuess = Opponent::tryToHit(couldBeShip,
                                             std::move(board->createCopy()));
        if (boardAfterGuess != nullptr){
            return boardAfterGuess;
        }
        // versucht zunächst ein zufälliges Feld aus der betweenShips Liste zu nehmen (da es bei dieser am
        // wahrscheinlichsten ist, einen Treffer zu landen) bis diese leer ist, gibt, wenn der guess korrekt war
        // eine Nachricht mit dem Ergebnis aus und beendet die Funktion, wenn dies nicht erfolgt wird genauso mit der
        // couldBeShip Liste weitergemacht und wenn auch diese keinen korrekten Guess ermöglicht, wird die Methode
        // guessRandom ausgeführt, der die Liste mit allen möglichen Guesses übergeben wird (diese wählt komplett zufällig)
    }
    return guessRandom(std::move(board), unGuessedFields);
    // ansonsten wird die Methode guessRandom ausgeführt, der die Liste mit allen möglichen Guesses übergeben wird
    // (diese wählt komplett zufällig)
}

std::unique_ptr<Board> Opponent::tryToHit(std::vector<Coordinates> coordinatesVector, std::unique_ptr<Board> board) {
    int randomFreeField;
    Coordinates coordinates = Coordinates(0,0);
    while (!coordinatesVector.empty()) {
        randomFreeField = GetRandomNumberBetween(0, coordinatesVector.size() - 1);
        coordinates = coordinatesVector.at(randomFreeField);
        coordinatesVector.erase(coordinatesVector.begin() + randomFreeField);
        GuessStatus guessResult = board->makeGuess(coordinates);
        if (guessResult != GuessStatus::guessImpossible) {
            printGuessResult(guessResult);
            return std::move(board);
        }
    }
    return nullptr;
}

std::unique_ptr<Board> Opponent::guessRandom(std::unique_ptr<Board> board, std::vector<Coordinates> unGuessedFields) {
    int randomFreeField = GetRandomNumberBetween(0, unGuessedFields.size() - 1);
    GuessStatus guessResult = board->makeGuess(unGuessedFields.at(randomFreeField));
    Opponent::printGuessResult(guessResult);
    return std::move(board);
    // guessed ein zufälliges Feld der gegebenen Liste der noch verdeckten Felder, biss die Liste komplett leer ist
}

void Opponent::printGuessResult(const GuessStatus &guessResult) {
    switch (guessResult) {
        case GuessStatus::sunkShip:
            std::cout << std::endl << "The enemy has sunk one of your ships!" << std::endl;
            break;
        case GuessStatus::guessedRight:
            std::cout << std::endl << "The enemy has hit one of your ships!" << std::endl;
            break;
        case GuessStatus::guessedWrong:
            std::cout << std::endl << "The enemy missed!" << std::endl;
            break;
    }
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
    // wenn das Ausgangsfeld ein aufgedecktes Schiffsfeld ist, wird geschaut, welches der umliegenden Felder ebenfalls
    // ein aufgedecktes Schiffsfeld ist und wenn ja in diese Richtung die Methode getNumberOfDiscoveredFieldsOfShipInThisDirection
    // ausgeführt und ihr Rückgabewert den anderen Ausführungen und dem Ausgangsfeld zusammengezählt → Größe des Schiffs
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
    // geht so lange in die angegebene Richtung und zählt die Anzahl der bisher durchquerten Felder, bis sie auf das
    // Ende des Boards oder ein nicht aufgedecktes Feld stößt
}



