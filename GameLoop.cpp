//
// Created by laure on 22.12.2023.
//

#include <iostream>
#include <thread>
#include "GameLoop.h"
#include "Opponent.h"
#include "Persistance.h"
#include "HelpFunctions.h"

void GameLoop::startGame(std::unique_ptr<Board> playerBoard, std::unique_ptr<Board> opponentBoard, int difficulty) {
    if (!HelpFunctions::valuesOfShipsLeftToSetAreZero(std::move(opponentBoard->createCopy()))) {
        opponentBoard = Opponent::placeAllShips(std::move(opponentBoard));
        std::cout << std::endl << "Der Gegner hat alle seine Schiffe gesetzt.";
    }
    std::cout << std::endl << std::endl << "Ihr Feld:";
    playerBoard->printShipField();
    std::vector<std::string> inputsVector;
    bool quitGame = false;
    int sizeOfBiggestShipLeftToSet;
    while (!HelpFunctions::valuesOfShipsLeftToSetAreZero(std::move(playerBoard->createCopy())) && !quitGame) {
//        playerBoard = requestShipSet(std::move(playerBoard));
        sizeOfBiggestShipLeftToSet = requestNewShipField(std::move(playerBoard->createCopy()));
        do {
            inputsVector = HelpFunctions::readCInIntoVector();
        } while (inputsVector.empty());
        if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '0') {
            printInstructions();
        } else if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '1') {
            quitGame = true;
        } else if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '2') {
            playerBoard = Opponent::placeAllShips(std::move(playerBoard));
        } else {
            playerBoard = requestShipSet(std::move(playerBoard), inputsVector, sizeOfBiggestShipLeftToSet);
        }
    }
    if (!quitGame) {
        std::cout << std::endl << "Nun geht es ans Felder aufdecken!" << std::endl;
    }
    while (playerBoard->totalShipsNotSunk > 0 && opponentBoard->totalShipsNotSunk > 0 && !quitGame) {
        int numberOfGuessesBefore;
        do {
            numberOfGuessesBefore = opponentBoard->guessCounter;
            requestNewGuess(std::move(opponentBoard->createCopy()));
            do {
                inputsVector = HelpFunctions::readCInIntoVector();
            } while (inputsVector.empty());
            if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '0') {
                printInstructions();
            } else if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '1') {
                std::cout << "Das ist die Platzierung Ihrer Schiffe:";
                playerBoard->printShipField();
            } else if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '2') {
                quitGame = true;
            } else if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '3') {
                Persistance::saveGame({(std::move(playerBoard->createCopy())),
                                       std::move(opponentBoard->createCopy()), 7});
            } else {
                opponentBoard = interpretGuess(std::move(opponentBoard), inputsVector);
            }
        } while (opponentBoard->guessCounter == numberOfGuessesBefore && !quitGame);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        if (opponentBoard->totalShipsNotSunk > 0 && !quitGame) {
            playerBoard = letOpponentGuess(std::move(playerBoard), difficulty);
        }
    }
    if (playerBoard->totalShipsNotSunk <= 0) {
        std::cout << std::endl << "Der Gegner hat alle Ihre Schiffe zerstoert und dadurch gewonnen!!!" << std::endl <<
            "Er hat dafuer " << playerBoard->guessCounter << " mal tippen muessen!" << std::endl << std::endl<<
            "Das gegnerische Schlachtfeld waere gewesen:";
            opponentBoard->printShipField();
    } else if (opponentBoard->totalShipsNotSunk <= 0) {
        std::cout << std::endl << "Sie haben alle Schiffe des Gegners zerstoert und dadurch gewonnen!!! Glueckwunsch!!!" << std::endl <<
                               "Sie haben dafuer " << opponentBoard->guessCounter << " mal tippen müssen!" << std::endl;
    }
}

int GameLoop::requestNewShipField(std::unique_ptr<Board> playerBoard) {
    int sizeOfBiggestShipLeftToSet = 0;
    for (int shipSize = 5; shipSize >= 2; shipSize--) {
        if (playerBoard->shipsLeftToSet.find(shipSize)->second > 0) {
            sizeOfBiggestShipLeftToSet = shipSize;
            break;
        }
    }
    std::cout << "Sie setzen als naechstes ein Schiff der Groesse " << sizeOfBiggestShipLeftToSet << ".";
    std::cout << std::endl << "Bitte geben Sie die Werte des Ausgangsfelds durch ein Leerzeichen getrennt an!";
    std::cout << std::endl << "( (0) Anweisungen und Regeln    (1) Spiel abbrechen und zurueck zum Hauptmenue";
    std::cout << std::endl << "  (2) restliche Schiffe zufaellig setzen )" << std::endl;
    return sizeOfBiggestShipLeftToSet;
}

std::unique_ptr<Board> GameLoop::requestShipSet(std::unique_ptr<Board> playerBoard, std::vector<std::string> inputsVector,
                                                int sizeOfBiggestShipLeftToSet) {
    if (inputsVector.size() != 2) {
        invalidInput();
        return std::move(playerBoard);
    }
    Coordinates coordinates = turnStringVectorIntoCoordinates(inputsVector);
    if (!GameRule::insideField(coordinates, playerBoard->size)) {
        invalidInput();
        return std::move(playerBoard);
    }
    std::cout << "Bitte geben Sie die Richtung, in die das Schiff gerichtet sein soll, an! ('o'^|'u'v|'l'<|'r'>)" << std::endl;
    std::string input;
    do {
        std::cin >> input;
    } while (input.empty());
    if (input.length() == 0 || !(input.at(0) == 'l' || input.at(0) == 'r' || input.at(0) == 'o' || input.at(0) == 'u')) {
        invalidInput();
        return std::move(playerBoard);
    }
    if (input.length() == 1 && input.at(0) == '0') {
        printInstructions();
        return std::move(playerBoard);
    }
    if (!playerBoard->addShip(sizeOfBiggestShipLeftToSet, coordinates, Coordinates::charIntoDirection(input.at(0)))) {
        invalidInput();
        return std::move(playerBoard);
    }
    playerBoard->printShipField();
    return std::move(playerBoard);
}

void GameLoop::requestNewGuess(std::unique_ptr<Board> opponentBoard) {
    std::cout << "Ihr bisheriges Resultat:";
    opponentBoard->printGuessField();
    std::cout << "Sie duerfen auf ein Feld tippen.";
    std::cout << std::endl << "Bitte geben Sie die Werte des Felds durch ein Leerzeichen getrennt an!";
    std::cout << std::endl << "( (0) Anweisungen und Regeln   (1) Mein Schiffsfeld";
    std::cout << std::endl << "  (2) Spiel abbrechen und zurueck zum Hauptmenue   (3) Spiel speichern )" << std::endl;
}

std::unique_ptr<Board> GameLoop::interpretGuess(std::unique_ptr<Board> opponentBoard, std::vector<std::string> inputsVector) {
    if (inputsVector.size() != 2) {
        invalidInput();
        return std::move(opponentBoard);
    }
    Coordinates coordinates = turnStringVectorIntoCoordinates(inputsVector);
    if (!GameRule::insideField(coordinates, opponentBoard->size)) {
        invalidInput();
        return std::move(opponentBoard);
    }
    GuessStatus guessResult = opponentBoard->makeGuess(coordinates);
    switch (guessResult) {
        case GuessStatus::sunkShip:
            std::cout << std::endl << "Sie haben ein Schiff versenkt!" << std::endl;
            break;
        case GuessStatus::guessedRight:
            std::cout << std::endl << "Sie haben ein Schiff getroffen!" << std::endl;
            break;
        case GuessStatus::guessedWrong:
            std::cout << std::endl << "Sie haben nichts getroffen!" << std::endl;
            break;
        default:
            invalidInput();
            return std::move(opponentBoard);
    }
    opponentBoard->guessCounter++;
    std::cout << std::endl << "Ihr bisheriger Fortschritt:";
    opponentBoard->printGuessField();
    std::cout << "Um zu gewinnen muessen Sie noch " << opponentBoard->totalShipsNotSunk << " Schiffe versenken!" << std::endl;
    return std::move(opponentBoard);
}

std::unique_ptr<Board> GameLoop::letOpponentGuess(std::unique_ptr<Board> playerBoard, int smartness) {
    std::cout << std::endl << "Der Gegner ist dran:";
    playerBoard = Opponent::makeGuess(std::move(playerBoard), smartness);
    playerBoard->printGuessField();
    std::cout << "Der Gegner muss noch " << playerBoard->totalShipsNotSunk << " Schiffe versenken, um zu gewinnen." << std::endl;
    return std::move(playerBoard);
}

void GameLoop::printInstructions() {
    std::cout << std::endl << "SPIELANWEISUNGEN: ";
    std::cout << std::endl << "Die Position von Feldern muss immer durch Eingabe des sie bezeichnenden Buchstaben "
                              "und der Zahl durch ein Leerzeichen " << std::endl <<
                              "getrennt angegeben werden.";
    std::cout << std::endl << "Beim Setzen von Schiffen wird das Ausgangsfeld des Schiffs und die Richtung, "
                              "in die es gerichtet ist nacheinander " << std::endl <<
                              "erwartet. Das Ausgangsfeld wird wie im vorherigen "
                              "Absatz beschrieben angegeben. Die Richtung wird als Anfangsbuchstaben" << std::endl <<
                              "der folgenden Auswahl an Richtungen erwartet: ";
    std::cout << std::endl << "1.links(-> 'l') 2.rechts(-> 'r') 3.oben(-> 'o') 4.unten(-> 'u')";
    std::cout << std::endl << "Beispiel: Wenn ein Schiff der Groesse 3 angegeben werden soll, als erste Eingabe '7 D' und "
                              "als zweite Eingabe 'u' " << std::endl <<
                              "erfolgt, bedeutet das, dass sich das Schiff ueber die Felder 7-D, 7-E und 7-F erstreckt";
    std::cout << std::endl << "Beim Tippen eines Feldes reicht lediglich die wie zuvor beschriebene Angabe eines Feldes.";
    std::cout << std::endl << std::endl << "SPIELREGELN:";
    std::cout << std::endl << "Das Feld besteht anfangs nur aus Wasser (Markierung: ~).";
    std::cout << std::endl << "Zunaechst muessen alle Schiffe gesetzt werden (das Programm fordert die jeweils zu setzende "
                              "Anzahl an Schiffen in der " << std::endl <<
                              "jeweiligen Groesse an), diese duerfen nicht um die Ecke gehen, "
                              "muessen komplett innerhalb des Spielfelds liegen und " << std::endl <<
                              "duerfen ein nebenliegendes Schiff nicht "
                              "beruehren. (Die auf dem eigenen Feld gesetzten Schiffe werden mit dem Zeichen @" << std::endl <<
                              "dargestellt.) Ist das erledigt, kann ab sofort nach jedem Spielzug gespeichert werden.";
    std::cout << std::endl << "Dann darf der Spieler anfangen auf eine Position des Feldes, auf dem der Gegner seine "
                              "Schiffe platziert hat, zu tippen. " << std::endl <<
                              "Dieses wird daraufhin aufgedeckt und markiert (Markierung: x), "
                              "falls sich ein Teil eines Schiffes darunter befindet. " << std::endl <<
                              "Auf jedes Feld kann nur einmal getippt werden. Wenn alle Positionen, "
                              "an denen sich das Schiff befindet dadurch " << std::endl <<
                              "markiert sind, ist dieses versenkt (Markierung: #). Wer zuerst alle Schiffe des "
                              "Gegners versenkt hat, gewinnt das Spiel!" << std::endl << std::endl;
}

void GameLoop::invalidInput() {
    std::cout << std::endl << "Dies ist eine ungueltige Eingabe!" << std::endl;
}

Coordinates GameLoop::turnStringVectorIntoCoordinates(std::vector<std::string> inputsVector) {
    if (inputsVector.size() != 2) {
        invalidInput();
        return Coordinates(0, 0);
    }
    int xPos = 0;
    int yPos = 0;
    if (inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) >= 65) {
        yPos = static_cast<int>(inputsVector.at(0).at(0)) - 65;
        for (char digit: inputsVector.at(1)) {
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
        return Coordinates(0, 0);
    }
    return Coordinates(xPos, yPos);
}

void GameLoop::printMainMenu() {
    std::cout << std::endl << "HAUPTMENUE:" << std::endl;
    std::cout << std::endl << "Ihre Optionen:" << std::endl <<
              "  (0) Anweisungen und Regeln       (1) neues Spiel starten" << std::endl <<
              "  (2) gespeichertes Spiel laden    (3) Spielstand loeschen" << std::endl <<
              "  (4) beenden" << std::endl;
}

void GameLoop::startNewGame() {
    std::string input;
    std::cout << std::endl << "Welche Groesse soll das Spielfeld haben? (Erlaubte Groessen:5-30 Normal:10)" << std::endl;
    int sizeInput = 0;
    do {
        if (sizeInput != 0) {
            std::cout << "Ungueltige Eingabe" << std::endl;
        }
        std::cin >> input;
        sizeInput = HelpFunctions::stringToInt(input);
    } while (!(sizeInput <= 30 && sizeInput >= 5));
    std::cout << std::endl << "Welchen Schwierigkeitsgrad moechten Sie waehlen? (Schwierigkeitsgrade:1-10 10=schwerstes)" << std::endl;
    int difficultyInput = 0;
    do {
        if (difficultyInput != 0) {
            std::cout << "Ungueltige Eingabe" << std::endl;
        }
        std::cin >> input;
        difficultyInput = HelpFunctions::stringToInt(input);
    } while (!(difficultyInput <= 10 && difficultyInput >= 1));
    std::unique_ptr<Board> playerBoard = std::make_unique<Board>(sizeInput);
    std::unique_ptr<Board> opponentBoard = std::make_unique<Board>(sizeInput);
    std::cout << std::endl << "DAS SPIEL BEGINNT" << std::endl;
    GameLoop::startGame(std::move(playerBoard), std::move(opponentBoard), difficultyInput);
}

void GameLoop::tryLoadGame() {
    GameState gameState = Persistance::loadGame();
    if (gameState.playerBoard != nullptr && gameState.opponentBoard != nullptr) {
        gameState.playerBoard->allShipsAlreadySet();
        gameState.opponentBoard->allShipsAlreadySet();
        GameLoop::startGame(std::move(gameState.playerBoard), std::move(gameState.opponentBoard), gameState.opponentLevel);
    } else {
        std::cout << std::endl << "Spielstand konnte nicht geladen werden" << std::endl;
    }
}

