//
// Created by laure on 22.12.2023.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__OPPONENT_H
#define SCHIFFEVERSENKEN_PROJEKT__OPPONENT_H


#include <memory>
#include "Board.h"
#include "Coordinates.h"

class Opponent {
public:
    // fügt ein neues Schiff in das Board ein und gibt das Board zurück
    static std::unique_ptr<Board> addRandomShipOfGivenSize(std::unique_ptr<Board>, int);
    static std::unique_ptr<Board> placeAllShips(std::unique_ptr<Board>);
    static std::unique_ptr<Board> makeGuess(std::unique_ptr<Board>, int);
    static std::unique_ptr<Board> guessRandom(std::unique_ptr<Board>, std::vector<Coordinates>);

private:
    static int getNumberOfDiscoveredFieldsOfShip(std::unique_ptr<Board>, Coordinates);
    static int getNumberOfDiscoveredFieldsOfShipInThisDirection(std::unique_ptr<Board>, Coordinates, Direction);

    static void printGuessResult(const GuessStatus &guessResult);

    static std::unique_ptr<Board> tryToHit(std::vector<Coordinates> coordinatesVector, std::unique_ptr<Board> board);
};


#endif //SCHIFFEVERSENKEN_PROJEKT__OPPONENT_H
