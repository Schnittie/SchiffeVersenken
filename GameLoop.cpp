//
// Created by laure on 22.12.2023.
//

#include <iostream>
#include <thread>
#include "GameLoop.h"
#include "Opponent.h"

void GameLoop::startGame(std::unique_ptr<Board> playerBoard, std::unique_ptr<Board> opponentBoard, int difficulty) {
    if (opponentBoard->shipsLeftToSet.find(2)->second + opponentBoard->shipsLeftToSet.find(3)->second +
        opponentBoard->shipsLeftToSet.find(4)->second + opponentBoard->shipsLeftToSet.find(5)->second > 0) {
        opponentBoard = Opponent::placeAllShips(std::move(opponentBoard));
        std::cout << std::endl << "Der Gegner hat alle seine Schiffe gesetzt.";
    }
    std::cout << std::endl << std::endl << "Ihr Feld:";
    playerBoard->printShipField();
    std::vector<std::string> inputsVector;
    int sizeOfBiggestBoatLeftToSet;
    while (playerBoard->shipsLeftToSet.find(2)->second + playerBoard->shipsLeftToSet.find(3)->second +
        playerBoard->shipsLeftToSet.find(4)->second + playerBoard->shipsLeftToSet.find(5)->second > 0) {
//        playerBoard = requestShipSet(std::move(playerBoard));
        sizeOfBiggestBoatLeftToSet = requestNewShipField(std::move(playerBoard->createCopy()));
        do {
            inputsVector = readCInIntoVector();
        } while (inputsVector.empty());
        if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '0') {
            printInstructions();
        } else if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '1') {
            // TODO
        } else {
            playerBoard = requestShipSet(std::move(playerBoard), inputsVector, sizeOfBiggestBoatLeftToSet);
        }
    }
    std::cout << std::endl << "Nun geht es ans Felder aufdecken!" << std::endl;
    while (playerBoard->totalShipsNotSunk > 0 && opponentBoard->totalShipsNotSunk > 0) {
        int numberOfGuessesBefore;
        do {
            numberOfGuessesBefore = opponentBoard->guessCounter;
            requestNewGuess(std::move(opponentBoard->createCopy()));
            do {
                inputsVector = readCInIntoVector();
            } while (inputsVector.empty());
            if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '0') {
                printInstructions();
            } else if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '1') {
                std::cout << "Das ist die Platzierung Ihrer Schiffe:";
                playerBoard->printShipField();
            } else if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '2') {
                // TODO
            } else if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1 && inputsVector.at(0).at(0) == '3') {
                // TODO
            } else {
                opponentBoard = interpretGuess(std::move(opponentBoard), inputsVector);
            }
//            opponentBoard = requestGuess(std::move(opponentBoard));
        } while (opponentBoard->guessCounter == numberOfGuessesBefore);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        if (opponentBoard->totalShipsNotSunk > 0) {
            playerBoard = letOpponentGuess(std::move(playerBoard), difficulty);
        }
    }
    if (playerBoard->totalShipsNotSunk <= 0) {
        std::cout << std::endl << "Der Gegner hat alle Ihre Schiffe zerstoert und dadurch gewonnen!!!";
    } else if (opponentBoard->totalShipsNotSunk <= 0) {
        std::cout << std::endl << "Sie haben alle Schiffe des Gegners zerstoert und dadurch gewonnen!!! Glueckwunsch!!!";
    }
}

int GameLoop::requestNewShipField(std::unique_ptr<Board> playerBoard) {
    int sizeOfBiggestBoatLeftToSet = 0;
    for (int boatSize = 5; boatSize >= 2; boatSize--) {
        if (playerBoard->shipsLeftToSet.find(boatSize)->second > 0) {
            sizeOfBiggestBoatLeftToSet = boatSize;
            break;
        }
    }
    std::cout << "Sie setzen als naechstes ein Boot der Groesse " << sizeOfBiggestBoatLeftToSet << ".";
    std::cout << std::endl << "Bitte geben Sie die Werte des Ausgangsfelds durch ein Leerzeichen getrennt an!";
    std::cout << std::endl << "( (0) Anweisungen und Regeln    (1) Spiel abbrechen und zurueck zum Hauptmenue )" << std::endl;
    return sizeOfBiggestBoatLeftToSet;
}

std::unique_ptr<Board> GameLoop::requestShipSet(std::unique_ptr<Board> playerBoard, std::vector<std::string> inputsVector,
                                                int sizeOfBiggestBoatLeftToSet) {
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
    if (!playerBoard->addShip(sizeOfBiggestBoatLeftToSet, coordinates, Coordinates::charIntoDirection(input.at(0)))) {
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


std::vector<char> GameLoop::readCInIntoCharVector(){
    std::string input;
    std::vector<char> inputs;
    std::getline(std::cin, input);
    for (char c: input) {
        inputs.push_back(c);
    }
    return inputs;
}

std::vector<std::string> GameLoop::readCInIntoVector(){
    std::string input;
    std::string singleInput;
    std::vector<std::string> inputs;
    std::getline(std::cin, input);
    for (char c: input) {
        if (c != ' ') {
            singleInput.push_back(c);
        } else {
            inputs.push_back(singleInput);
            singleInput.clear();
        }
    }
    if (!singleInput.empty()) {
        inputs.push_back(singleInput);
    }
    return inputs;
}

void GameLoop::printInstructions() {
    std::cout << std::endl << "Spielanweisungen: ";
    std::cout << std::endl << "Die Position von Feldern muss immer durch Eingabe des sie bezeichnenden Buchstaben "
                              "und der Zahl durch ein Leerzeichen getrennt angegeben werden.";
    std::cout << std::endl << "Beim Setzen von Schiffen wird das Ausgangsfeld des Schiffs und die Richtung, "
                              "in die es gerichtet ist nacheinander erwartet. Das Ausgangsfeld wird wie im vorherigen "
                              "Absatz beschrieben angegeben. Die Richtung wird als Anfangsbuchstaben der im "
                              "Folgenden Auswahl an Richtungen erwartet: ";
    std::cout << std::endl << "1.links(-> 'l') 2.rechts(-> 'r') 3.oben(-> 'o') 4.unten(-> 'u')";
    std::cout << std::endl << "Beispiel: Wenn ein Schiff der Groesse 3 angegeben werden soll, als erste Eingabe '7 D' und"
                              "als zweite Eingabe 'u' erfolgt, bedeutet das, dass sich das Schiff "
                              "ueber die Felder 7-D, 7-E und 7-F erstreckt";
    std::cout << std::endl << "Beim Tippen eines Feldes reicht lediglich die zuvor beschriebene Angabe eines Feldes.";
    std::cout << std::endl << "Spielregeln:";
    std::cout << std::endl << "Das Feld besteht anfangs nur aus Wasser (Markierung: ~)";
    std::cout << std::endl << "Zunaechst muessen alle Schiffe gesetzt werden (das Programm fordert die jeweils zu setzende "
                              "Anzahl an Schiffen in der jeweiligen Groesse an), diese duerfen nicht um die Ecke gehen, "
                              "muessen komplett innerhalb des Spielfelds liegen und duerfen ein nebenliegendes Schiff nicht "
                              "beruehren. (Die auf dem eigenen Feld gesetzten Schiffe werden mit dem Zeichen @ dargestellt.)"
                              "Ist das erledigt, kann ab sofort nach jedem Spielzug gespeichert werden.";
    std::cout << std::endl << "Dann darf der Spieler anfangen auf eine Position des Feldes, auf dem der Gegner seine"
                              "Boote platziert hat, zu tippen. Dieses wird daraufhin aufgedeckt und markiert (Markierung: x), "
                              "falls sich ein Teil eines Schiffes darunter befindet. Auf jedes Feld kann nur einmal "
                              "getippt werden. Wenn alle Positionen, an denen sich das Schiff befindet dadurch markiert sind, "
                              "ist dieses versenkt (Markierung: #). Wer zuerst alle Schiffe des Gegners versenkt hat, "
                              "gewinnt das Spiel!" << std::endl << std::endl;
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

