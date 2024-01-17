//
// Created by laure on 22.12.2023.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__GAMELOOP_H
#define SCHIFFEVERSENKEN_PROJEKT__GAMELOOP_H


#include <memory>
#include "Board.h"

class GameLoop {
private:
    static int requestNewShipField(std::unique_ptr<Board>);
    static void requestNewGuess(std::unique_ptr<Board>);
public:
    static void startGame(std::unique_ptr<Board>, std::unique_ptr<Board>, int);
    static std::unique_ptr<Board> requestShipSet(std::unique_ptr<Board>, std::vector<std::string>, int);
    static std::unique_ptr<Board> interpretGuess(std::unique_ptr<Board>, std::vector<std::string>);
    static std::unique_ptr<Board> letOpponentGuess(std::unique_ptr<Board>, int);
    static Coordinates turnStringVectorIntoCoordinates(std::vector<std::string>);

    static void invalidInput();
    static void printMainMenu();
    static void startNewGame();
    static void tryLoadGame();
    static void printInstructions();
};


#endif //SCHIFFEVERSENKEN_PROJEKT__GAMELOOP_H
