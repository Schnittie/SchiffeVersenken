//
// Created by laure on 22.12.2023.
//

#include "Persistance.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cmath>

std::string errorString = "error";
void
Persistance::saveGame(GameState gameState) {
    //Games are converted into a String wich is put into a file
    std::string saveGameString;
    saveGameString.append("PlayerBoard:");
    saveGameString.append(turnBoardIntoString(std::move(gameState.playerBoard->createCopy())));
    saveGameString.append("\nOpponentBoard:");
    saveGameString.append(turnBoardIntoString(std::move(gameState.opponentBoard->createCopy())));
    saveGameString.append("\nOpponentLevel:\n");
    saveGameString.append(std::to_string(gameState.opponentLevel));
    saveGameString.append("\nSaveValue:\n");
    //validation is performed so that we can make somewhat sure the file wasn't changed
    saveGameString.append(Persistance::generateValidationString(saveGameString));

    std::string saveFilename;
    std::string saveDirectoryName = "BattleshipSaves/";

    // Getting user input for the filename
    std::cout << "Enter a name for the save: ";
    std::cin.sync();

    std::getline(std::cin, saveFilename);
    saveFilename.append(".battleship");
    saveDirectoryName.append(saveFilename);
    Persistance::saveString(saveDirectoryName, saveGameString);
}

void Persistance::saveString(const std::string &fileName, const std::string &gameString) {
    //Saves are put in a new directory in the current directory
    tryCreatingSaveDirectory();
    std::ofstream outFile(fileName);

    if (outFile.is_open()) {
        outFile << gameString;
        outFile.close();
        std::cout << "Game successfully saved to " << fileName << std::endl;
    } else {
        std::cerr << "Error opening file: " << fileName << std::endl;
    }
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

    boardString.append("\nShipsNotSunk:\n");
    boardString.append(std::to_string(board->totalShipsNotSunk));
    return boardString;
}

void Persistance::tryCreatingSaveDirectory() {
    std::string directoryName = "BattleshipSaves";

    // Create the directory if it doesn't already exist
    if (!std::filesystem::exists(directoryName)) {
        if (std::filesystem::create_directory(directoryName)) {
        } else {
            std::cerr << "Failed to create directory." << std::endl;
        }
    }
}

void Persistance::deleteSave(){
    std::string filePath = Persistance::chooseFile();
    if (filePath == errorString){
        return;
    }

    // Attempt to delete the file
    if (std::remove(filePath.c_str()) == 0) {
        std::cout << filePath << " wurde erfolgreich geloescht" << std::endl;
    } else {
        std::cout << filePath << " konnte nicht geloescht werden" << std::endl;
    }
}

std::string Persistance::chooseFile(){
    //if the saves Directory isn't here we creat it
    tryCreatingSaveDirectory();
    std::string directoryPath = "./BattleshipSaves"; // the "." is current directory

    //we make a vector with all the files ending in .battleship
    std::vector<std::string> filenames;
    for (const auto &entry: std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".battleship") {
            filenames.push_back(entry.path().filename().string());
        }
    }
    //we show the user all found save files
    std::cout  << "0. Cancel"  << std::endl;
    int numberOfSave = 1;
    for (std::string &filename: filenames) {
        std::cout << numberOfSave << ". " << filename << std::endl;
        numberOfSave++;
    }

    //there are no save files
    if (numberOfSave == 1) {
        std::cout << "No save file found" << std::endl;
        return errorString;
    }

    int userInput;
    bool validInput = false;
    while (!validInput) {
        userInput = -1;
        std::cin.sync();
        std::cout << "Choose a save by inputting the number: ";

        // Try to get an integer from the user
        if (std::cin >> userInput) {
            if (userInput == 0){
                std::cout << "Cancelled" << std::endl;
                return errorString;
            }
            if (userInput <= numberOfSave - 1 && userInput > 0) {
                validInput = true;
            } else {
                std::cout << "Invalid input. Please enter a number between 1 and " << numberOfSave << std::endl;
            }
        } else { //if the user input isn't an integer
            std::cout << "Invalid input. Please enter an integer." << std::endl;
            std::cin.clear();
        }
    }

    //we add the chosen file to the path
    directoryPath.append("/");
    directoryPath.append(filenames.at(userInput - 1));
    return directoryPath;
}

/**
 * @return returns a loaded GameState or an Empty GameState in case there are no saves or an error occurs
 */
GameState Persistance::loadGame() {
    std::string savePath = Persistance::chooseFile();
    if (savePath == errorString){
        return Persistance::emptyGame();
    }
    return getGameStateFromFile(savePath);
}

GameState Persistance::getGameStateFromFile(std::string filename) {
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cout << "Unable to open the file." << std::endl;
        return Persistance::emptyGame();
    }

    //we read the file and put each line in a vector
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }
    inputFile.close();

    //if the validation fails, the save file was changed
    if (!Persistance::validateString(lines)) {
        std::cout << "Faulty save file" << std::endl;
        return Persistance::emptyGame();
    }

    //the lines.at() correlate to the lines where the information was stored, it's ugly, but I think a fancy solution would be overkill
    return GameState({std::move(
            Persistance::getBoardFromStrings(lines.at(2), lines.at(4), std::stoi(lines.at(6)))->createCopy()),
                      std::move(Persistance::getBoardFromStrings(lines.at(9), lines.at(11),
                                                                 std::stoi(lines.at(13)))->createCopy()),
                      std::stoi(lines.at(15))});
}

std::unique_ptr<Board> Persistance::getBoardFromStrings(std::string guessFieldString, std::string shipFieldString,
                                                        int notSunk) {
    //the size of board is the size of one side. since any board is a square, the root of the amount of entries in any field must be an integer
    if (guessFieldString.size() != shipFieldString.size() ||
        (static_cast<int>(std::sqrt(shipFieldString.size()))) *
        (static_cast<int>(std::sqrt(shipFieldString.size()))) !=
        shipFieldString.size()) {
        std::cout << "Faulty save file" << std::endl;
        //returning nullpointer isn't ideal, since if the other board works fine, only part of the loaded game will be null, I'm hoping the validation will catch this though
        return nullptr;
    }

    //this is mostly similar code from the usual board initialization, but instead of initializing every value at false/not guessed, we input the loaded values
    std::vector<std::vector<GuessStatus>> guessField;
    std::vector<std::vector<bool>> shipField;
    int size = static_cast<int>(std::sqrt(shipFieldString.size()));
    for (int xPos = 0; xPos < size; xPos++) {
        for (int yPos = 0; yPos < size; yPos++) {
            if (xPos == 0) {
                std::vector<bool> shipFieldCol;
                shipField.push_back(shipFieldCol);
                std::vector<GuessStatus> guessFieldCol;
                guessField.push_back(guessFieldCol);

            }
            //since x is the outer loop xPos * size + yPos will return the right position in the field-turned-line string
            shipField.at(xPos).push_back(shipFieldString.at(xPos * size + yPos) == '1');
            guessField.at(xPos).push_back(getGuessStatusFromChar(guessFieldString.at(xPos * size + yPos)));

        }
    }

    //we make a new board, but overwrite the fields and other values
    std::unique_ptr<Board> board = std::make_unique<Board>((static_cast<int>(std::sqrt(shipFieldString.size()))));
    board->shipField = shipField;
    board->guessField = guessField;
    board->size = size;
    board->totalShipsNotSunk = notSunk;

    return std::move(board->createCopy());
}

int Persistance::getIntFromGuessStatus(GuessStatus guessStatus) {
    //instead of the Guess Status we save an Int
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
    return 5;
}

GuessStatus Persistance::getGuessStatusFromChar(char i) {
    //this reverses getIntFromGuessStatus
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

std::string Persistance::generateValidationString(std::string string) {
    //it's like hashing but simpler: we sum up all the chars in the file and add it to the bottom
    long sum = 0;
    for (char c: string) {
        sum += c;
    }
    return std::to_string(sum);
}

bool Persistance::validateString(std::vector<std::string> lines) {
    //in order to validate, we take all the lines, generate a new Validation String and look if it's the same from the file
    if (lines.size() != 18){
        return false;
    }
    std::string allLines = lines.at(0);

    //We put all the lines back into one string
    for (size_t i = 1; i < lines.size() - 1; ++i) {
        allLines.append("\n");
        allLines.append(lines.at(i));
    }
    allLines.append("\n");

    return Persistance::generateValidationString(allLines) == lines.back();
}

GameState Persistance::emptyGame() {
    return {nullptr, nullptr,-1};
}
