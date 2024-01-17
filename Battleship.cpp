#include <iostream>
#include "Board.h"
#include "Opponent.h"
#include "Persistance.h"
#include "GameLoop.h"
#include <memory>


int stringToInt(const std::string& input) {
    int returnInt = 0;
    for (char c: input) {
        returnInt = returnInt*10 + (static_cast<int>(c) - 48);
    }
    return returnInt;
}

int main() {
    std::cout << "    ____  ___  ______________    ___________ __  __________ \n"
                 "   / __ )/   |/_  __/_  __/ /   / ____/ ___// / / /  _/ __ \\\n"
                 "  / __  / /| | / /   / / / /   / __/  \\__ \\/ /_/ // // /_/ /\n"
                 " / /_/ / ___ |/ /   / / / /___/ /___ ___/ / __  // // ____/ \n"
                 "/_____/_/  |_/_/   /_/ /_____/_____//____/_/ /_/___/_/      \n"
                 "                                                            \n";
    bool quit = false;
    while (!quit) {
        std::cout << std::endl << "HAPUTMENUE:" << std::endl;
        std::cout << std::endl << "Ihre Optionen:" << std::endl <<
            "  (0) Anweisungen und Regeln       (1) neues Spiel starten" << std::endl <<
            "  (2) gespeichertes Spiel laden    (3) beenden" << std::endl;
        std::string input;
        do {
            input.clear();
            std::cin >> input;
        } while (input.empty());
        if(input == "0") {
            GameLoop::printInstructions();
        } else if(input == "1") {
            std::cout << std::endl << "Welche Groesse soll das Spielfeld haben? (Erlaubte Groessen:5-30 Normal:10)" << std::endl;
            int sizeInput = 0;
            do {
                if (sizeInput != 0) {
                    std::cout << "Ungueltige Eingabe" << std::endl;
                }
                std::cin >> input;
                sizeInput = stringToInt(input);
            } while (!(sizeInput <= 30 && sizeInput >= 5));
                std::cout << std::endl << "Welchen Schwierigkeitsgrad moechten Sie waehlen? (Schwierigkeitsgrade:1-10 10=schwerstes)" << std::endl;
                int difficultyInput = 0;
                do {
                    if (difficultyInput != 0) {
                        std::cout << "Ungueltige Eingabe" << std::endl;
                    }
                    std::cin >> input;
                    difficultyInput = stringToInt(input);
                } while (!(difficultyInput <= 10 && difficultyInput >= 1));
                std::unique_ptr<Board> playerBoard = std::make_unique<Board>(sizeInput);
                std::unique_ptr<Board> opponentBoard = std::make_unique<Board>(sizeInput);
                std::cout << std::endl << "DAS SPIEL BEGINNT" << std::endl;
                GameLoop::startGame(std::move(playerBoard), std::move(opponentBoard), difficultyInput);
        } else if(input == "2") {
            GameState gameState = Persistance::loadGame();
            if (gameState.playerBoard != nullptr && gameState.opponentBoard != nullptr) {
                GameLoop::startGame(std::move(gameState.playerBoard), std::move(gameState.opponentBoard), gameState.opponentLevel);
            } else {
                std::cout << std::endl << "Spielstand konnte nicht geladen werden" << std::endl;
            }
        } else if(input == "3") {
            quit = true;
        } else {
            std::cout << "Ungueltige Eingabe" << std::endl;
        }
    }
//    std::unique_ptr<Board> playerBoard = std::make_unique<Board>(10);
//    std::unique_ptr<Board> opponentBoard = std::make_unique<Board>(10);
//    GameLoop::startGame(std::move(playerBoard), std::move(opponentBoard), 8);
//    board = gameLoop->requestShipSet(std::move(board));
//    std::unique_ptr<Opponent> opponent = std::make_unique<Opponent>();
//    board = opponent->placeAllShips(std::move(board));
//    // wichtig! logisch: nicht mehr Guesses als size^2 möglich
//    for (int i = 0; i < 390; i++) {
//        board = opponent->makeGuess(std::move(board));
//    }
//    Persistance::saveGame({(std::move(board->createCopy())), std::move(board->createCopy()), 7});
//    board->printShipField();
//    board->printGuessField();
//    GameState gameState = Persistance::loadGame();
//    gameState.playerBoard->printShipField();
//    gameState.playerBoard->printGuessField();
//    return 0;
}
