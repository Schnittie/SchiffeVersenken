//
// Created by laure on 22.12.2023.
//

#include <iostream>
#include <thread>
#include <algorithm>
#include "GameLoop.h"
#include "Opponent.h"
#include "Persistance.h"
#include "HelpFunctions.h"


void GameLoop::printMainMenu() {
    std::cout << std::endl << "MAIN MENU:" << std::endl;
    std::cout << std::endl << "Your options:" << std::endl <<
              "  (0) Instructions and rules       (1) Start new game" << std::endl <<
              "  (2) Load saved game          (3) Delete saved game" << std::endl <<
              "  (4) Exit" << std::endl;
}

void GameLoop::tryLoadGame() {
    GameState gameState = Persistance::loadGame();
    if (gameState.playerBoard != nullptr && gameState.opponentBoard != nullptr) {
        gameState.playerBoard->allShipsAlreadySet();
        gameState.opponentBoard->allShipsAlreadySet();
        GameLoop::startGame(std::move(gameState.playerBoard), std::move(gameState.opponentBoard), gameState.opponentLevel);
        // wenn das Laden des Spiels erfolgreich war und beide Boards im gameState Objekt vorhanden sind,
        // wird das Spiel mit dem geladenen Spielstand gestartet
    } else {
        std::cout << std::endl << "Unable to load saved game" << std::endl;
    }
}

void GameLoop::startNewGame() {
    std::string input;
    std::cout << std::endl << "What size do you want the board to be? (Valid sizes:5-30 Default:10)" << std::endl;
    int sizeInput = 0;
    do {
        if (sizeInput != 0) {
            invalidInput();
            std::cout << "Please try again:" << std::endl;
            // Eingabe hat nicht den erwarteten Werten genügt → Fehlermeldung
        }
        std::cin >> input;
        sizeInput = HelpFunctions::stringToInt(input);
        // wandelt den String in einen int um
    } while (!(sizeInput <= 30 && sizeInput >= 5));
    // wiederhole so lange, bis eine valide Eingabe erfolgt
    std::cout << std::endl << "What difficulty do you want your opponent to be? (Difficulty:1-10 10=hardest)" << std::endl;
    int difficultyInput = 0;
    do {
        if (difficultyInput != 0) {
            invalidInput();
            std::cout << "Please try again:" << std::endl;
            // Eingabe hat nicht den erwarteten Werten genügt → Fehlermeldung
        }
        std::cin >> input;
        difficultyInput = HelpFunctions::stringToInt(input);
        // wandelt den String in einen int um
    } while (!(difficultyInput <= 10 && difficultyInput >= 1));
    // wiederhole so lange, bis eine valide Eingabe erfolgt
    std::unique_ptr<Board> playerBoard = std::make_unique<Board>(sizeInput);
    std::unique_ptr<Board> opponentBoard = std::make_unique<Board>(sizeInput);
    std::cout << std::endl << "THE GAME BEGINS" << std::endl;
    GameLoop::startGame(std::move(playerBoard), std::move(opponentBoard), difficultyInput);
    // erstellt die entsprechenden Boards und startet das Spiel
}

void GameLoop::startGame(std::unique_ptr<Board> playerBoard, std::unique_ptr<Board> opponentBoard, int difficulty) {
    bool quitRound = false;
    if (!HelpFunctions::valuesOfShipsLeftToSetAreZero(std::move(opponentBoard->createCopy()))) {
        opponentBoard = Opponent::placeAllShips(std::move(opponentBoard));
        std::cout << std::endl << "The opponent has placed their ships.";
    }
    // der Gegner setzt den Rest seiner Schiffe, falls sie noch nicht gesetzt sind
    std::cout << std::endl << std::endl << "Your field:";
    playerBoard->printShipField();
    // das Feld des Spielers wird zu Orientierung ausgegeben
    std::vector<std::string> inputsVector;
    playerBoard = tryToRequestAllShipsSet(std::move(playerBoard));
    // man versucht alle Schiffe vom Spieler gesetzt zu bekommen
    if (HelpFunctions::valuesOfShipsLeftToSetAreZero(std::move(playerBoard->createCopy()))) {
        std::cout << std::endl << "Now it's time to make guesses!" << std::endl;
    } else {
        quitRound = true;
    }
    // wenn nicht alle Schiffe gesetzt wurden, heißt das, dass der Spieler zu Hauptmenü möchte
    while (playerBoard->totalShipsNotSunk > 0 && opponentBoard->totalShipsNotSunk > 0 && !quitRound) {
        // wiederhole so lange, bis alle Schiffe eines zerstört wurden, oder der Spieler die Runde verlassen möchte
        int numberOfGuessesBefore;
        do {
            numberOfGuessesBefore = opponentBoard->guessCounter;
            // speichere die Nummer der Guesses der Spieler zwischen um nach Rückerhalt des Boards
            // nachvollziehen zu können, ob der Spieler erfolgreich getippt hat und um insgesamt die Anzahl der guesses
            // speichern zu können
            requestNewGuess(std::move(opponentBoard->createCopy()));
            // fordere einen neuen Rate-Versuch des Spielers an
            do {
                inputsVector = HelpFunctions::readCInIntoVector();
            } while (inputsVector.empty());
            // warte, bis der Spieler eine Eingabe gemacht und nicht nur eine neue Zeile angefangen hat
            if (HelpFunctions::vectorHasOneValueWithOneChar(inputsVector) && inputsVector.at(0).at(0) == '0') {
                printInstructions();
            } else if (HelpFunctions::vectorHasOneValueWithOneChar(inputsVector) && inputsVector.at(0).at(0) == '1') {
                std::cout << "This is the placement of your ships:";
                playerBoard->printShipField();
            } else if (HelpFunctions::vectorHasOneValueWithOneChar(inputsVector) && inputsVector.at(0).at(0) == '2') {
                quitRound = true;
            } else if (HelpFunctions::vectorHasOneValueWithOneChar(inputsVector) && inputsVector.at(0).at(0) == '3') {
                Persistance::saveGame({(std::move(playerBoard->createCopy())),
                                       std::move(opponentBoard->createCopy()), 7});
            } else {
                opponentBoard = interpretGuess(std::move(opponentBoard), inputsVector);
            }
            // wenn nur ein Zeichen eingegeben wurde und dieses einer Aktion zugewiesen ist, führe diese Aktion durch,
            // ansonsten versuche die Eingabe als Rate-Versuch zu interpretieren
        } while (opponentBoard->guessCounter == numberOfGuessesBefore && !quitRound);
        // wiederhole so lange, bis ein Rate-Versuch des Spielers gültig war oder der Spieler das Spiel verlassen möchte
        // halte den Thread kurz an, damit der Spieler erkennen kann, ob sein Rate-Versuch erfolgreich war
        if (opponentBoard->totalShipsNotSunk > 0 && !quitRound) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            playerBoard = letOpponentGuess(std::move(playerBoard), difficulty);
            playerBoard->guessCounter++;
        }
        // wenn der Spieler noch nicht alle Schiffe des Gegners zerstört hat und nicht die Runde verlassen möchte,
        // macht der Opponent einen Rate-Versuch
    }
    if (playerBoard->totalShipsNotSunk <= 0) {
        std::cout << std::endl << "The enemy has destroyed all your ships and has thereby won the game!!!" << std::endl <<
            "It took them " << playerBoard->guessCounter << " guesses!" << std::endl << std::endl<<
            "The opponents ship placement would have been:";
            opponentBoard->printShipField();
    } else if (opponentBoard->totalShipsNotSunk <= 0) {
        std::cout << std::endl << "You have destroyed all your opponents ships and thereby won the game!!! Congratulations!!!" << std::endl <<
                               "It took you " << opponentBoard->guessCounter << " guesses!" << std::endl;
    }
    // wenn die Schleife für die Rate-Versuche verlassen wurde und dabei ein Spieler alle Schiffe des Gegners zerstört
    // hat, wird eine Nachricht für das Gewinnen/Verlieren ausgegeben
}

void GameLoop::requestNewShipField(std::unique_ptr<Board> playerBoard) {
    std::cout << "Please place a ship of the size " <<
    getSizeOfBiggestShipLeftToSet(std::move(playerBoard->createCopy())) << ".";
    std::cout << std::endl << "Please separate your input by spaces!";
    std::cout << std::endl << "( (0) Instructions and rules    (1) Cancel game and return to main menu";
    std::cout << std::endl << "  (2) place all other ships randomly )" << std::endl;
}

int GameLoop::getSizeOfBiggestShipLeftToSet(std::unique_ptr<Board> board) {
    int sizeOfBiggestShipLeftToSet = 0;
    for (int shipSize = 5; shipSize >= 2; shipSize--) {
        if (board->shipsLeftToSet.find(shipSize)->second > 0) {
            sizeOfBiggestShipLeftToSet = shipSize;
            break;
        }
    }
    return sizeOfBiggestShipLeftToSet;
    // gehe die Werte von shipsLeftToSet anfangend vom größten durch und gebe die Schiffsgröße zurück,
    // bei der zuerst noch ein Schiff zu setze ist
}

std::unique_ptr<Board> GameLoop::tryToRequestAllShipsSet(std::unique_ptr<Board> board) {
    bool quitRound = false;
    std::vector<std::string> inputsVector;
    while (!HelpFunctions::valuesOfShipsLeftToSetAreZero(std::move(board->createCopy())) && !quitRound) {
        // wiederhole so lange, bis entweder von allen Schiffsgrößen in ShipsLeftToSet im Board keine mehr gesetzt werden
        // müssen oder quitRound auf true gesetzt wurde
        requestNewShipField(std::move(board->createCopy()));
        // fordert den Spieler an, "Koordinaten" einzugeben, für das Feld, von dem ausgehend er das Schiff setzen möchte
        do {
            inputsVector = HelpFunctions::readCInIntoVector();
        } while (inputsVector.empty());
        // warte, bis der Spieler eine Eingabe gemacht und nicht nur eine neue Zeile angefangen hat
        if (HelpFunctions::vectorHasOneValueWithOneChar(inputsVector)) {
            if (inputsVector.at(0).at(0) == '0') {
                printInstructions();
                inputsVector.clear();
            } else if (inputsVector.at(0).at(0) == '1') {
                quitRound = true;
            } else if (inputsVector.at(0).at(0) == '2') {
                board = Opponent::placeAllShips(std::move(board));
            } else {
                invalidInput();
            }
            // wenn nur ein Zeichen eingegeben wurde und dieses entweder 1, 2 oder 3 ist, dann führe die entsprechende Funktion aus,
            // wenn dieses Zeichen nichts davon ist, dann ist dies eine invalide Eingabe
        } else {
            Coordinates coordinates = turnStringVectorIntoCoordinates(inputsVector);
            if (inputsVector.size() != 2 || !GameRule::insideField(coordinates, board->size)) {
                inputsVector.clear();
                // wenn die Anzahl der Inputs nicht 2 ist oder die Koordinaten, die auf Basis des Inputs generiert wurden
                // nicht im Feld liegen, dann ist dies eine invalide Eingabe
            } else {
                board = requestShipDirectionAndTrySet(std::move(board), coordinates,
                                                      getSizeOfBiggestShipLeftToSet(std::move(board->createCopy())));
            }
            // wenn nicht, dann führe requestShipDirectionAndTrySet aus, versuche also mit den gegebenen Eingaben nach dem Erfragen
            // der Richtung, in die das Schiff gerichtet sein soll ein Schiff zu setzen
        }
    }
    return std::move(board);
}

std::unique_ptr<Board> GameLoop::requestShipDirectionAndTrySet(std::unique_ptr<Board> playerBoard, Coordinates coordinates,
                                                               int sizeOfBiggestShipLeftToSet) {
    std::cout << "Please input the direction the ship should be facing! ('u'^|'d'v|'l'<|'r'>)" << std::endl;
    std::string input;
    do {
        std::cin >> input;
    } while (input.empty());
    // warte, bis der Spieler eine Eingabe gemacht und nicht nur eine neue Zeile angefangen hat
    std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    if (input.length() != 1 || !(input.at(0) == 'l' || input.at(0) == 'r' || input.at(0) == 'u'
                            || input.at(0) == 'd' || input.at(0) == '0')) {
        invalidInput();
        return std::move(playerBoard);
    }
    // wenn entweder nicht genau ein Zeichen eingegeben wurde oder die Eingabe nicht eines der erwarteten Zeichen ist,
    // ist das ein invalide Eingabe
    if (input.at(0) == '0') {
        printInstructions();
        return std::move(playerBoard);
    } // wenn die Eingabe '0' ist, dann gebe die Anweisungen und Regeln aus und beende den Vorgang
    if (!playerBoard->addShip(sizeOfBiggestShipLeftToSet, coordinates, Coordinates::charIntoDirection(input.at(0)))) {
        std::cout << std::endl << "You cannot place a ship there" << std::endl;
        return std::move(playerBoard);
    } // wenn das Hinzufügen des Schiffes nicht erfolgreich war, dann war das ein invalide Eingabe
    playerBoard->printShipField();
    return std::move(playerBoard);
    // wenn alles korrekt war, gebe das Board mit dem neuen Schiff aus und beende die Funktion
    // (Das Schiff wurde durch playerBoard->addShip(...) hinzugefügt, da dieses ja nicht fehlgeschlagen ist)
}

void GameLoop::requestNewGuess(std::unique_ptr<Board> opponentBoard) {
    std::cout << "Your results up to this point:";
    opponentBoard->printGuessField();
    std::cout << "You may guess a field.";
    std::cout << std::endl << "Please separate your input by spaces!";
    std::cout << std::endl << "( (0) Instructions and rules   (1) My shipfield";
    std::cout << std::endl << "  (2) Cancel game and return to main menu   (3) Save game )" << std::endl;
}

std::unique_ptr<Board> GameLoop::interpretGuess(std::unique_ptr<Board> opponentBoard, const std::vector<std::string>& inputsVector) {
    // der inputs vector enthält die einzelnen Eingaben des Spielers an den Leerzeichen getrennt
    Coordinates coordinates = turnStringVectorIntoCoordinates(inputsVector);
    if (inputsVector.size() != 2 || !GameRule::insideField(coordinates, opponentBoard->size)) {
        invalidInput();
        return std::move(opponentBoard);
        // wenn entweder nicht genau zwei Zeichenketten eingegeben wurden oder sich die Koordinaten, die auf Basis der
        // Zeichenketten erstellt wurden, nicht im Spielfeld befinden, dann war das ein invalide Eingabe
    }
    GuessStatus guessResult = opponentBoard->makeGuess(coordinates);
    // versuche den Guess auf das Board anzuwenden
    switch (guessResult) {
        case GuessStatus::sunkShip:
            std::cout << std::endl << "You sunk a ship!" << std::endl;
            break;
        case GuessStatus::guessedRight:
            std::cout << std::endl << "You hit a ship!" << std::endl;
            break;
        case GuessStatus::guessedWrong:
            std::cout << std::endl << "You missed!" << std::endl;
            break;
        default:
            invalidInput();
            return std::move(opponentBoard);
    }
    // eine Nachricht wird je nach Treffer/nicht-Treffer ausgegeben, oder, falls der Guess ungültig war
    // (z.B. weil dieses Feld schon guessed wurde), dann war das ein invalide Eingabe
    opponentBoard->guessCounter++;
    // sonst war der Guess erfolgreich → der GuessCounter wird erhöht
    std::cout << std::endl << "Your progress so far:";
    opponentBoard->printGuessField();
    std::cout << "In order to win you still have to sink " << opponentBoard->totalShipsNotSunk << " ships!" << std::endl;
    // dem Spieler werden Informationen über seinen Spielfortschritt ausgegeben
    return std::move(opponentBoard);
}

std::unique_ptr<Board> GameLoop::letOpponentGuess(std::unique_ptr<Board> playerBoard, int difficulty) {
    std::cout << std::endl << "It's the opponent's turn:";
    playerBoard = Opponent::makeGuess(std::move(playerBoard), difficulty);
    playerBoard->printGuessField();
    std::cout << "The enemy still needs to sink " << playerBoard->totalShipsNotSunk << " ships in order to win." << std::endl;
    return std::move(playerBoard);
    // der Gegner macht einen Guess und das Resultat dessen wird dem Spieler angezeigt
}

void GameLoop::printInstructions() {
    std::cout << std::endl << "INSTRUCTIONS: ";
    std::cout << std::endl << "Inputting coordinates is done by writing the corresponding letter and number "
                              "seperated by a space." << std::endl;
    std::cout << std::endl << "When setting up your ships, first input the coordinates, "
                              "then input the direction the ship should go along. " << std::endl <<
                              "The direction is given by inputting the first letter." << std::endl;
    std::cout << std::endl << "1.left(-> 'l') 2.right(-> 'r') 3.up(-> 'u') 4.down(-> 'd')";
    std::cout << std::endl << "Example: When inputting a ship of size 3, first input the coordinates 'D 7' afterwards "
                              "input the direction 'd'." << std::endl <<
                              "This will place a ship which extends over the fields 'D 7' 'E 7' 'F 7'";
    std::cout << std::endl << "When making a guess, inputting coordinates follows the same pattern.";
    std::cout << std::endl << std::endl << "RULES:";
    std::cout << std::endl << "The field starts out containing only water (Represented by: ~).";
    std::cout << std::endl << "First all ships have to be placed (the game will tell you which size ship to place next. "<< std::endl <<
                              "The number of ships is dependant on the size of the board) " << std::endl <<
                              "The ships may not go around corners, "<< std::endl <<
                              "have to be completely inside the field " << std::endl <<
                              "and cannot be placed next to each other. "
                              "(Your own placed ships are represented by an @.)" << std::endl <<
                              "Once you have placed your ships, you can save your game before each turn.";
    std::cout << std::endl << "Each of your turns beginns with you making a guess on your opponents field."<< std::endl <<
                              "That field will then be revealed. If you managed to hit an enemy ship,"
                              "the location will be marked with an x. " << std::endl <<
                              "You can only ever guess each field once."<< std::endl <<
                              "As soon as all coordinates "
                              "of a ship are hit, it will sink and get the marking # " << std::endl <<
                              "The player who first sinks all of their opponents ships wins. " << std::endl <<
                              "Good luck!" << std::endl << std::endl;
}

void GameLoop::invalidInput() {
    std::cout << std::endl << "This is an invalid input" << std::endl;
}

Coordinates GameLoop::turnStringVectorIntoCoordinates(std::vector<std::string> inputsVector) {
    if (inputsVector.size() != 2) {
        invalidInput();
        return {-1, -1};
        // Es wird ein Vektor mit zwei Werten erwartet → alles andere ist eine ungültige Eingabe → Rückgabe ungültiger Koordinaten
    }
    std::transform(inputsVector.at(0).begin(), inputsVector.at(0).end(), inputsVector.at(0).begin(), [](unsigned char c) {
        return std::toupper(c);
    });
    int xPos = 0;
    int yPos;
    if (inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) >= 65) {
        yPos = static_cast<int>(inputsVector.at(0).at(0)) - 65;
        for (char &digit: inputsVector.at(1)) {
            xPos = xPos * 10 + (static_cast<int>(digit) - 48);
        }
        xPos--;
    } else if (inputsVector.at(1).size() == 1 && inputsVector.at(1).at(0) >= 65) {
        yPos = static_cast<int>(inputsVector.at(1).at(0)) - 65;
        for (char digit: inputsVector.at(0)) {
            xPos = xPos * 10 + (static_cast<int>(digit) - 48);
        }
        xPos--;
    } else {
        invalidInput();
        return {-1, -1};
    }
    // einer der Werte muss ein einzelnes Zeichen sein mit einem Ascii-Code >= 65 (es handelt sich um die y-Koordinate,
    // die als Buchstabe erwartet wird), wenn dies nicht so ist, dann ist das eine invalide Eingabe
    // der zuvor benannte Wert wird zu einer Zahl umgewandelt und -65 gerechnet, um einen Wert von 0-30 zu bekommen
    // (A=0, B=1, ...) und wird dann als yPos interpretiert, beim anderen wird der Zwischenwert von xPos so lage *10
    // gerechnet und die nächste Ziffer in eine int gecastet - 48 gerechnet und aufaddiert, bis alle Ziffern durch sind
    // (Ascii-Code von 1=48, 2=49, ...) → der Wert wird vom String in eine Int umgewandelt
    return {xPos, yPos};
}

