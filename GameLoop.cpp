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
    bool quitGame = false;
    if (!HelpFunctions::valuesOfShipsLeftToSetAreZero(std::move(opponentBoard->createCopy()))) {
        opponentBoard = Opponent::placeAllShips(std::move(opponentBoard));
        std::cout << std::endl << "The opponent has placed their ships.";
    }
    std::cout << std::endl << std::endl << "Your field:";
    playerBoard->printShipField();
    std::vector<std::string> inputsVector;
    playerBoard = tryToRequestAllShipsSet(std::move(playerBoard));
    if (HelpFunctions::valuesOfShipsLeftToSetAreZero(std::move(playerBoard->createCopy()))) {
        std::cout << std::endl << "Now it's time to make guesses!" << std::endl;
    } else {
        quitGame = true;
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
                std::cout << "This is the placement of your ships:";
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
        std::cout << std::endl << "The enemy has destroyed all your ships and has thereby won the game!!!" << std::endl <<
            "It took them " << playerBoard->guessCounter << " guesses!" << std::endl << std::endl<<
            "The opponents ship placement would have been:";
            opponentBoard->printShipField();
    } else if (opponentBoard->totalShipsNotSunk <= 0) {
        std::cout << std::endl << "You have destroyed all your opponents ships and thereby won the game!!! Congratulations!!!" << std::endl <<
                               "It took you " << opponentBoard->guessCounter << " guesses!" << std::endl;
    }
}

void GameLoop::requestNewShipField(std::unique_ptr<Board> playerBoard) {
    std::cout << "Please place a ship of the size " <<
    getSizeOfBiggestShipLeftToSet(std::move(playerBoard->createCopy())) << ".";
    std::cout << std::endl << "Please separate your input by spaces!";
    std::cout << std::endl << "( (0) Tutorial and rules    (1) Cancel game and return to main menu";
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
}

std::unique_ptr<Board> GameLoop::tryToRequestAllShipsSet(std::unique_ptr<Board> board) {
    bool quitGame = false;
    std::vector<std::string> inputsVector;
    while (!HelpFunctions::valuesOfShipsLeftToSetAreZero(std::move(board->createCopy())) && !quitGame) {
        // wiederhole so lange, bis entweder von allen Schiffsgrößen in ShipsLeftToSet im Board keine mehr gesetzt werden
        // müssen oder quitGame auf true gesetzt wurde
        requestNewShipField(std::move(board->createCopy()));
        // fordert den Spieler an, "Koordinaten" einzugeben, für das Feld, von dem ausgehend er das Schiff setzen möchte
        do {
            inputsVector = HelpFunctions::readCInIntoVector();
        } while (inputsVector.empty());
        // warte, bis der Spieler eine Eingabe gemacht und nicht nur eine neue Zeile angefangen hat
        if (inputsVector.size() == 1 && inputsVector.at(0).size() == 1) {
            if (inputsVector.at(0).at(0) == '0') {
                printInstructions();
            } else if (inputsVector.at(0).at(0) == '1') {
                quitGame = true;
            } else if (inputsVector.at(0).at(0) == '2') {
                board = Opponent::placeAllShips(std::move(board));
            } else {
                board = requestShipSet(std::move(board), inputsVector,
                                       getSizeOfBiggestShipLeftToSet(std::move(board->createCopy())));
            }
            // wenn nur ein Zeichen eingegeben wurde und dieses entweder 1, 2 oder 3 ist, dann führe die entsprechende Funktion aus,
            // wenn nicht, dann führe requestShipSet aus, versuche also für die gegebenen Eingabe ein Schiff zu setzen
        } else {
            board = requestShipSet(std::move(board), inputsVector,
                                   getSizeOfBiggestShipLeftToSet(std::move(board->createCopy())));
        }
    }
    return std::move(board);
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
    std::cout << "Please input the direction the ship should be facing! ('o'^|'u'v|'l'<|'r'>)" << std::endl;
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
    std::cout << "Your results up to this point:";
    opponentBoard->printGuessField();
    std::cout << "You may guess a field.";
    std::cout << std::endl << "Please separate your input by spaces!";
    std::cout << std::endl << "( (0) Tutorial and rules   (1) My shipfield";
    std::cout << std::endl << "  (2) Cancel game and return to main menu   (3) Save game )" << std::endl;
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
    opponentBoard->guessCounter++;
    std::cout << std::endl << "Your progress so far:";
    opponentBoard->printGuessField();
    std::cout << "In order to win you still have to sink " << opponentBoard->totalShipsNotSunk << " ships!" << std::endl;
    return std::move(opponentBoard);
}

std::unique_ptr<Board> GameLoop::letOpponentGuess(std::unique_ptr<Board> playerBoard, int smartness) {
    std::cout << std::endl << "It's the opponent's turn:";
    playerBoard = Opponent::makeGuess(std::move(playerBoard), smartness);
    playerBoard->printGuessField();
    std::cout << "The enemy still needs to sink " << playerBoard->totalShipsNotSunk << " ships in order to win." << std::endl;
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
    std::cout << std::endl << "This is an invalid input" << std::endl;
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
    std::cout << std::endl << "MAIN MENU:" << std::endl;
    std::cout << std::endl << "Your options:" << std::endl <<
              "  (0) Tutorial and rules       (1) Start new games" << std::endl <<
              "  (2) Load saved game    (3) Delete saved game" << std::endl <<
              "  (4) Exit" << std::endl;
}

void GameLoop::startNewGame() {
    std::string input;
    std::cout << std::endl << "What size do you want the board to be? (Valid sizes:5-30 Default:10)" << std::endl;
    int sizeInput = 0;
    do {
        if (sizeInput != 0) {
            std::cout << "Invalid input" << std::endl;
        }
        std::cin >> input;
        sizeInput = HelpFunctions::stringToInt(input);
    } while (!(sizeInput <= 30 && sizeInput >= 5));
    std::cout << std::endl << "What difficulty do you want your opponent to be? (Difficulty:1-10 10=hardest)" << std::endl;
    int difficultyInput = 0;
    do {
        if (difficultyInput != 0) {
            std::cout << "Invalid input" << std::endl;
        }
        std::cin >> input;
        difficultyInput = HelpFunctions::stringToInt(input);
    } while (!(difficultyInput <= 10 && difficultyInput >= 1));
    std::unique_ptr<Board> playerBoard = std::make_unique<Board>(sizeInput);
    std::unique_ptr<Board> opponentBoard = std::make_unique<Board>(sizeInput);
    std::cout << std::endl << "THE GAME BEGINS" << std::endl;
    GameLoop::startGame(std::move(playerBoard), std::move(opponentBoard), difficultyInput);
}

void GameLoop::tryLoadGame() {
    GameState gameState = Persistance::loadGame();
    if (gameState.playerBoard != nullptr && gameState.opponentBoard != nullptr) {
        gameState.playerBoard->allShipsAlreadySet();
        gameState.opponentBoard->allShipsAlreadySet();
        GameLoop::startGame(std::move(gameState.playerBoard), std::move(gameState.opponentBoard), gameState.opponentLevel);
    } else {
        std::cout << std::endl << "Unable to load saved game" << std::endl;
    }
}

