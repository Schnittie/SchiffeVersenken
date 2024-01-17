//
// Created by laure on 22.12.2023.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__GAMELOOP_H
#define SCHIFFEVERSENKEN_PROJEKT__GAMELOOP_H


#include <memory>
#include "Board.h"

class GameLoop {
private:
    static void requestNewShipField(std::unique_ptr<Board>);
    static int getSizeOfBiggestShipLeftToSet(std::unique_ptr<Board>);
    static void requestNewGuess(std::unique_ptr<Board>);
public:
    static void startGame(std::unique_ptr<Board>, std::unique_ptr<Board>, int);
    static std::unique_ptr<Board> requestShipDirectionAndTrySet(std::unique_ptr<Board> playerBoard, Coordinates coordinates, int sizeOfBiggestShipLeftToSet);
    static std::unique_ptr<Board> interpretGuess(std::unique_ptr<Board>, const std::vector<std::string>&);
    static std::unique_ptr<Board> letOpponentGuess(std::unique_ptr<Board>, int);
    static Coordinates turnStringVectorIntoCoordinates(std::vector<std::string>);
    static std::unique_ptr<Board> tryToRequestAllShipsSet(std::unique_ptr<Board>);

    static void invalidInput();
    static void printMainMenu();
    static void startNewGame();
    static void tryLoadGame();
    static void printInstructions();
};


#endif //SCHIFFEVERSENKEN_PROJEKT__GAMELOOP_H
