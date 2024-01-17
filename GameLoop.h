//
// Created by laure on 22.12.2023.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__GAMELOOP_H
#define SCHIFFEVERSENKEN_PROJEKT__GAMELOOP_H


#include <memory>
#include "Board.h"

class GameLoop {
private:
    static std::vector<std::string> readCInIntoVector();
    static int requestNewShipField(std::unique_ptr<Board>);
    static void requestNewGuess(std::unique_ptr<Board>);
public:
    static void startGame(std::unique_ptr<Board>, std::unique_ptr<Board>, int);
    static std::unique_ptr<Board> requestShipSet(std::unique_ptr<Board>, std::vector<std::string>, int);
    static std::unique_ptr<Board> interpretGuess(std::unique_ptr<Board>, std::vector<std::string>);
    static std::unique_ptr<Board> letOpponentGuess(std::unique_ptr<Board>, int);
    static void invalidInput();
    static void printInstructions();
    static Coordinates turnStringVectorIntoCoordinates(std::vector<std::string>);
//TODO: hier kommt alles zum Spiel rein, das ganze bekommt ein Spielbrett (egal ob es ein neues ist oder geladen wurde) und ein Opponent
//TODO: basically ist das ja nur das anzeigen vom Board, spieler fragen was gemacht werden soll, das tun, dann macht der Opponent whatever und es geht von vorne los
};


#endif //SCHIFFEVERSENKEN_PROJEKT__GAMELOOP_H
