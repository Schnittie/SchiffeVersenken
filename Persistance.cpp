//
// Created by laure on 22.12.2023.
//

#include "Persistance.h"
#include <iostream>
#include <fstream>
#include <string>

void Persistance::saveString(const std::string &fileName, const std::string &gameString) {
    std::ofstream outFile(fileName);
    if (outFile.is_open()) {
        outFile << gameString;
        outFile.close();
        std::cout << "Game successfully saved to " << fileName << std::endl;
    } else {
        std::cerr << "Error opening file: " << fileName << std::endl;
    }
}

void
Persistance::saveGame(std::unique_ptr<Board> playerBoard, std::unique_ptr<Board> opponentBoard, int opponentLevel) {
    std::string saveGameString;
    saveGameString.append("\nPlayerBoard:\n");
    saveGameString.append(turnBoardIntoString(std::move(playerBoard->createCopy())));
    saveGameString.append("\n");
    saveGameString.append("\nOpponentBoard:\n");
    saveGameString.append(turnBoardIntoString(std::move(opponentBoard->createCopy())));
    saveGameString.append("\n");
    saveGameString.append("\nOpponentLevel:\n");
    saveGameString.append(std::to_string(opponentLevel));
    std::string saveFilename;

    // Prompt the user for input
    std::cout << "Enter a name for the save: ";

    // Get the entire line of input from the user
    std::getline(std::cin, saveFilename);
    saveFilename.append(".battleship");
    Persistance::saveString(saveFilename,saveGameString);
}

std::string Persistance::turnBoardIntoString(std::unique_ptr<Board> board) {
    std::string boardString;
    boardString.append("\nGuessField:\n");
    for (auto &i: board->guessField) {
        for (auto &j: i) {
            boardString.append(std::to_string(getIntFromGuessStatus(j)));
        }
    }
    boardString.append("\nShipField:\n");
    for (auto &i: board->shipField) {
        for (bool &j: i) {
            if (j) {
                boardString.append("0");
            } else {
                boardString.append("1");
            }
        }
    }
    return boardString;
}

int Persistance::getIntFromGuessStatus(GuessStatus guessStatus) {
    switch (guessStatus) {
        case GuessStatus::guessImpossible:
            return 0;
        case GuessStatus::guessedRight:
            return 1;
        case GuessStatus::guessedWrong:
            return 2;
        case GuessStatus::notGuessed:
            return 3;
        case GuessStatus::sunkShip:
            return 4;
    }
}

