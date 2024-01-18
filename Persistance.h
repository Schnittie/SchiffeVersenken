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

    static void deleteSave();
private:
    static GameState emptyGame();
    static void saveString(const std::string &, const std::string &);

    static std::string turnBoardIntoString(std::unique_ptr<Board>);

    static int getIntFromGuessStatus(GuessStatus guessStatus);
    static void tryCreatingSaveDirectory();
    static GameState getGameStateFromFile(const std::string& filename);

    static GuessStatus getGuessStatusFromChar(char i);

    static std::unique_ptr<Board> getBoardFromStrings(std::string guessFieldString, std::string shipFieldString, int notSunk);

    static std::string generateValidationString(const std::string& string);

    static bool validateString(std::vector<std::string> vector1);

    static std::string chooseFile();
};


#endif //SCHIFFEVERSENKEN_PROJEKT__PERSISTANCE_H
