//
// Created by laure on 22.12.2023.
//

#include "Persistance.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>


void Persistance::saveString(const std::string &fileName, const std::string &gameString) {
    tryCreatingSaveDirecotry();
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
Persistance::saveGame(GameState gameState) {
    std::string saveGameString;
    saveGameString.append("PlayerBoard:");
    saveGameString.append(turnBoardIntoString(std::move(gameState.playerBoard->createCopy())));
    saveGameString.append("\nOpponentBoard:");
    saveGameString.append(turnBoardIntoString(std::move(gameState.opponentBoard->createCopy())));
    saveGameString.append("\nOpponentLevel:\n");
    saveGameString.append(std::to_string(gameState.opponentLevel));
    std::string saveFilename;
    std::string saveDirectoryName = "BattleshipSaves/";

    // Prompt the user for input
    std::cout << "Enter a name for the save: ";
    std::cin.sync();

    // Get the entire line of input from the user
    std::getline(std::cin, saveFilename);
    saveFilename.append(".battleship");
    saveDirectoryName.append(saveFilename);
    Persistance::saveString(saveDirectoryName, saveGameString);
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
        for (bool j: i) {
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

void Persistance::tryCreatingSaveDirecotry() {
    // Specify the directory name
    std::string directoryName = "BattleshipSaves";

    // Create the directory if it doesn't exist
    if (!std::filesystem::exists(directoryName)) {
        if (std::filesystem::create_directory(directoryName)) {
        } else {
            std::cerr << "Failed to create directory." << std::endl;
        }
    }
}

GameState Persistance::loadGame() {
    // Specify the directory path
    tryCreatingSaveDirecotry();
    std::string directoryPath = "./BattleshipSaves"; // Current directory

    // Create a vector to store filenames
    std::vector<std::string> filenames;

    // Iterate through the directory
    for (const auto &entry: std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".battleship") {
            // Add the filename to the vector
            filenames.push_back(entry.path().filename().string());
        }
    }
    int numberOfSave = 1;
    for (std::string filename: filenames) {
        std::cout << numberOfSave << ". " << filename << std::endl;
        numberOfSave++;
    }
    if (numberOfSave == 1) {
        std::cout << "No save file found" << std::endl;
    }
    int userInput;
    bool validinput = false;
    while (!validinput) {
        userInput = -1;
        std::cin.sync();
        std::cout << "Choose a save: ";

        // Try to get an integer from the user
        if (std::cin >> userInput) {
            if (userInput <= numberOfSave - 1 && userInput > 0) {
                validinput = true;
            } else {
                std::cout << "Invalid input. Please enter a number between 1 and " << numberOfSave << std::endl;
            }
        } else {
            std::cout << "Invalid input. Please enter an integer." << std::endl;

            // Clear the failbit and discard invalid input
            std::cin.clear();
        }
    }
    directoryPath.append("/");
    directoryPath.append(filenames.at(userInput - 1));
    return getGamestateFromFile(directoryPath);

}

GameState Persistance::getGamestateFromFile(std::string filename) {
    std::cout << filename << std::endl;
    return GameState();
}
