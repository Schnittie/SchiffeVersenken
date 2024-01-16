//
// Created by laure on 22.12.2023.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__PERSISTANCE_H
#define SCHIFFEVERSENKEN_PROJEKT__PERSISTANCE_H


#include <string>
#include <memory>
#include "Board.h"

class Board;

struct GameState {
    std::unique_ptr<Board> playerBoard;
    std::unique_ptr<Board> opponentBoard;
    int opponentLevel;
};

class Persistance {
public:
    static void saveGame(GameState gameState);

    static GameState loadGame();

    static GameState emptyGame();

private:
    static void saveString(const std::string &, const std::string &);

    static std::string turnBoardIntoString(std::unique_ptr<Board>);

//TODO: Hier kommt das Speichern und laden von Spielständen hin, das einzige was wirklich gespeichert werden muss ist wie das Board aussieht, da immer wenn das Speichern möglich ist sowieso der Spieler dran ist
//TODO: evtl. falls wir den Opponent fancy machen müssten wir noch speichern gegen welche Art von Opponent gespielt wird

    static int getIntFromGuessStatus(GuessStatus guessStatus);
    static void tryCreatingSaveDirecotry();
    static GameState getGameStateFromFile(std::string filename);
    static GuessStatus getGuessStatusFromChar(char i);

    static std::unique_ptr<Board> getBoardFromStrings(std::string guessFieldString, std::string shipFieldString, int notSunk);

    static std::string generateValidationString(std::string string);

    static bool validateString(std::vector<std::string> vector1);
};


#endif //SCHIFFEVERSENKEN_PROJEKT__PERSISTANCE_H
