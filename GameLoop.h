//
// Created by laure on 22.12.2023.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__GAMELOOP_H
#define SCHIFFEVERSENKEN_PROJEKT__GAMELOOP_H


#include <memory>
#include "Board.h"

class GameLoop {
private:
    static void startGame(std::unique_ptr<Board>, std::unique_ptr<Board>, int);
    static void requestNewShipField(std::unique_ptr<Board>);
    static std::unique_ptr<Board> tryToRequestAllShipsSet(std::unique_ptr<Board>);
    static std::unique_ptr<Board> requestShipDirectionAndTrySet(std::unique_ptr<Board> playerBoard, Coordinates coordinates, int sizeOfBiggestShipLeftToSet);
    static void requestNewGuess(std::unique_ptr<Board>);
    static std::unique_ptr<Board> interpretGuess(std::unique_ptr<Board>, const std::vector<std::string>&);
    static std::unique_ptr<Board> letOpponentGuess(std::unique_ptr<Board>, int);
    static int getSizeOfBiggestShipLeftToSet(std::unique_ptr<Board>);
    static Coordinates turnStringVectorIntoCoordinates(std::vector<std::string>);
public:
    static void printMainMenu();
    static void printInstructions();
    static void startNewGame();
    static void tryLoadGame();
    static void invalidInput();
};


#endif //SCHIFFEVERSENKEN_PROJEKT__GAMELOOP_H
