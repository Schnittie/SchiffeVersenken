//
// Created by laure on 22.12.2023.
//

#include "Persistance.h"
#include <iostream>
#include <fstream>
#include <string>

void Persistance::saveString(std::string fileName, std::string gameString) {
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
    saveGameString.append(turnBoardIntoString(std::move(playerBoard->createCopy())));
    saveGameString.append("\n");
    saveGameString.append(turnBoardIntoString(std::move(opponentBoard->createCopy())));
    saveGameString.append("\n");
    saveGameString.append(std::to_string(opponentLevel));
}

std::string Persistance::turnBoardIntoString(std::unique_ptr<Board>) {
    std::string guessString;
    std::string shipString;
}
