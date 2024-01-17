//
// Created by jonif on 17.01.2024.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__HELPFUNCTIONS_H
#define SCHIFFEVERSENKEN_PROJEKT__HELPFUNCTIONS_H


#include <string>
#include <vector>
#include <map>
#include "Board.h"

class HelpFunctions {
public:
    static int stringToInt(const std::string&);
    static std::vector<std::string> readCInIntoVector();
    static bool valuesOfShipsLeftToSetAreZero(std::unique_ptr<Board>);
    static bool vectorHasOneValueWithOneChar(std::vector<std::string>);
};

#endif //SCHIFFEVERSENKEN_PROJEKT__HELPFUNCTIONS_H
