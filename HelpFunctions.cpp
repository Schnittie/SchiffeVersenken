//
// Created by jonif on 17.01.2024.
//

#include <iostream>
#include <algorithm>
#include "HelpFunctions.h"

int HelpFunctions::stringToInt(const std::string& input) {
    int returnInt = 0;
    for (char c: input) {
        returnInt = returnInt*10 + (static_cast<int>(c) - 48);
    }
    return returnInt;
}

std::vector<std::string> HelpFunctions::readCInIntoVector(){
    std::string input;
    std::string singleInput;
    std::vector<std::string> inputs;
    std::getline(std::cin, input);
    for (char c: input) {
        if (c != ' ') {
            singleInput.push_back(c);
        } else {
            inputs.push_back(singleInput);
            singleInput.clear();
        }
    }
    if (!singleInput.empty()) {
        inputs.push_back(singleInput);
    }
    return inputs;
}

bool HelpFunctions::valuesOfShipsLeftToSetAreZero(std::unique_ptr<Board> board) {
    return board->shipsLeftToSet.find(2)->second
        + board->shipsLeftToSet.find(3)->second
        + board->shipsLeftToSet.find(4)->second
        + board->shipsLeftToSet.find(5)->second <= 0;
}

bool HelpFunctions::vectorHasOneValueWithOneChar(std::vector<std::string> vector) {
    return vector.size() == 1 && vector.at(0).size() == 1;
}