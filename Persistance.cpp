//
// Created by laure on 22.12.2023.
//

#include "Persistance.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cmath>


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
                boardString.append("1");
            } else {
                boardString.append("0");
            }
        }
    }
    return boardString;
}

GameState Persistance::emptyGame() {
    return {};
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
        return Persistance::emptyGame();
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
    // Open a file for reading
    std::ifstream inputFile(filename);

    // Check if the file is opened successfully
    if (!inputFile.is_open()) {
        std::cout << "Unable to open the file." << std::endl;
        return Persistance::emptyGame();
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }
    // Close the file
    inputFile.close();
    std::string playerGuessFieldString = lines.at(2);
    std::string playerShipFieldString = lines.at(4);
    std::string opponentGuessFieldString = lines.at(7);
    std::string opponentShipFieldString = lines.at(9);
    int opponentLevel = std::stoi(lines.at(11));
    if (playerGuessFieldString.size() != playerShipFieldString.size() || playerGuessFieldString.size() != opponentGuessFieldString.size() ||
        playerShipFieldString.size() != opponentShipFieldString.size() ||
        (static_cast<int>(std::sqrt(playerShipFieldString.size()))) * (static_cast<int>(std::sqrt(playerShipFieldString.size()))) !=
        playerShipFieldString.size()) {
        std::cout << "Faulty save file" << std::endl;
        return Persistance::emptyGame();
    }
    std::vector<std::vector<GuessStatus>> playerGuessField;
    std::vector<std::vector<bool>> playerShipField;
    std::vector<std::vector<GuessStatus>> opponentGuessField;
    std::vector<std::vector<bool>> opponentShipField;
    int size = static_cast<int>(std::sqrt(playerShipFieldString.size()));
    // speichere die Größe des Boards
    for (int xPos = 0; xPos < size; xPos++) {
        for (int yPos = 0; yPos < size; yPos++) {
            if (xPos == 0) {
                std::vector<bool> shipFieldCol;
                playerShipField.push_back(shipFieldCol);
                std::vector<GuessStatus> guessFieldCol;
                playerGuessField.push_back(guessFieldCol);
                std::vector<bool> opponentShipFieldCol;
                opponentShipField.push_back(opponentShipFieldCol);
                std::vector<GuessStatus> opponentGuessFieldCol;
                opponentGuessField.push_back(opponentGuessFieldCol);
            }
            playerShipField.at(xPos).push_back(playerShipFieldString.at(xPos*size+yPos)=='1');
            playerGuessField.at(xPos).push_back(getGuessStatusFromChar(playerGuessFieldString.at(xPos*size+yPos)));
            opponentShipField.at(xPos).push_back(opponentShipFieldString.at(xPos*size+yPos)=='1');
            opponentGuessField.at(xPos).push_back(getGuessStatusFromChar(opponentGuessFieldString.at(xPos*size+yPos)));
        }
    }
    std::unique_ptr<Board> playerBoard = std::make_unique<Board>((static_cast<int>(std::sqrt(playerShipFieldString.size()))));
    playerBoard->shipField = playerShipField;
    playerBoard->guessField = playerGuessField;
    std::unique_ptr<Board> opponentBoard = std::make_unique<Board>((static_cast<int>(std::sqrt(playerShipFieldString.size()))));
    opponentBoard->shipField = opponentShipField;
    opponentBoard->guessField = opponentGuessField;

    return GameState({std::move(playerBoard->createCopy()),std::move(opponentBoard->createCopy()),opponentLevel});
}

GuessStatus Persistance::getGuessStatusFromChar(char i) {
    switch (i) {
        case '1':
            return GuessStatus::guessedRight;
        case '2':
            return GuessStatus::guessedWrong;
        case '3':
            return GuessStatus::notGuessed;
        case '4':
            return GuessStatus::sunkShip;
        default:
            return GuessStatus::guessImpossible;
    }
}
