//
// Created by jonif on 06.01.2024.
//

#include <algorithm>
#include <iostream>
#include "GameRule.h"


bool GameRule::shipAddCorrect(int shipSize, int xPos, int yPos, Direction direction, std::unique_ptr<Board> board) {
    // teste, ob Ursprungspunkt des Schiffs auf dem Feld
    if (shipSize <= 5 && shipSize >= 2 && insideField(xPos, yPos)) {
        // testen, ob weitere Schiffsfelder innerhalb von Board und nicht neben anderem Schiffsfeld
        int fieldsToSet = shipSize; // shipSize ist am Anfang Größe des zu setzenden bootes
        int currentXPos = xPos;
        int currentYPos = yPos;
        while (fieldsToSet > 0) {
            // teste, ob aktuell zu behandelndes Feld unbelegt ist
            if (insideField(currentXPos, currentYPos) && !board->shipField[currentXPos][currentYPos]) {
                // teste, ob Feld links davon unbelegt ist oder außerhalb des Feldes liegt
                bool leftFieldFree = currentXPos - 1 < 0 || !board->shipField[currentXPos - 1 ][currentYPos];
                // teste, ob Feld rechts davon unbelegt ist oder außerhalb des Feldes liegt
                bool rightFieldFree = currentXPos + 1 > 9 || !board->shipField[currentXPos + 1][currentYPos];
                // teste, ob Feld darüber unbelegt ist oder außerhalb des Feldes liegt
                bool upperFieldFree = currentYPos - 1 < 0 || !board->shipField[currentXPos][currentYPos - 1];
                // teste, ob Feld darunter unbelegt ist oder außerhalb des Feldes liegt
                bool lowerFieldFree = currentYPos + 1 > 9 || !board->shipField[currentXPos][currentYPos + 1];

                if (leftFieldFree && rightFieldFree && upperFieldFree && lowerFieldFree) {
                    // passe Variablen für (je nach Direction) neuem Feld an
                    if (direction == Direction::left) {
                        currentXPos--;
                    } else if (direction == Direction::right) {
                        currentXPos++;
                    } else if (direction == Direction::up) {
                        currentYPos--;
                    } else {
                        currentYPos++;
                    }
                    fieldsToSet--;
                } else {
                    // → Schiff würde an anderes Schiff angrenzen
                    return false;
                }
            } else {
                // → Schiff würde nicht komplett im Feld liegen
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

bool GameRule::shipDestroyed(int xPos, int yPos, std::unique_ptr<Board> board) {
    if (insideField(xPos, yPos) && board->shipField[xPos][yPos]) {
            // teste mit Untermethode, ob Schiff in alle Richtungen, in die es weiter geht, zerstört ist
            std::vector<bool> resultVector;
            if (insideField(xPos - 1) && board->shipField[xPos-1][yPos]) {
                resultVector.push_back(shipInThisDirectionDestroyed(xPos-1, yPos, std::move(board->createCopy()), Direction::left));
            }
            if (insideField(xPos + 1) && board->shipField[xPos+1][yPos]) {
                resultVector.push_back(shipInThisDirectionDestroyed(xPos+1, yPos, std::move(board->createCopy()), Direction::right));
            }
            if (insideField(yPos - 1) && board->shipField[xPos][yPos-1]) {
                resultVector.push_back(shipInThisDirectionDestroyed(xPos, yPos-1, std::move(board->createCopy()), Direction::up));
            }
            if (insideField(yPos + 1) && board->shipField[xPos][yPos+1]) {
                resultVector.push_back(shipInThisDirectionDestroyed(xPos, yPos+1, std::move(board->createCopy()), Direction::down));
            }
            return std::all_of(resultVector.begin(), resultVector.end(), [](bool resultPart) {
                return resultPart;
            });
    } else {
        return false;
    }
}

bool GameRule::shipInThisDirectionDestroyed(int xPos, int yPos, std::unique_ptr<Board> board, Direction direction) {
    int recentXPos = xPos;
    int recentYPos = yPos;
    while (true) {
        if (direction == Direction::left) {
            if (!insideField(recentXPos - 1) || !board->shipField[recentXPos-1][recentYPos]) {
                // Feld 1 weiter links liegt entweder außerhalb oder es befindet sich kein Schiff darauf
                return true;
            } else if (board->guessField[recentXPos-1][recentYPos] == GuessStatus::guessedRight) {
                // es befindet sich ein Schiff auf dem Feld 1 weiter links (da erster if-Fall nicht eingetroffen und dieses wurde bereits an diesem Feld getroffen
                recentXPos--;
            } else {
                // es befindet sich ein Schiff auf dem Feld 1 weiter links und es wurde z.B. noch nicht beschossen
                return false;
            }
        } else if (direction == Direction::right) {
            if (!insideField(recentXPos + 1) || !board->shipField[recentXPos+1][recentYPos]) {
                return true;
            } else if (board->guessField[recentXPos+1][recentYPos] == GuessStatus::guessedRight) {
                recentXPos++;
            } else {
                return false;
            }
        } else if (direction == Direction::up) {
            if (!insideField(recentYPos - 1) || !board->shipField[recentXPos][recentYPos-1]) {
                return true;
            } else if (board->guessField[recentXPos][recentYPos-1] == GuessStatus::guessedRight) {
                recentYPos--;
            } else {
                return false;
            }
        } else{
            if (!insideField(recentYPos + 1)|| !board->shipField[recentXPos][recentYPos+1]) {
                return true;
            } else if (board->guessField[recentXPos][recentYPos+1] == GuessStatus::guessedRight) {
                recentYPos++;
            } else {
                return false;
            }
        }
    }
}

bool GameRule::insideField(int coordinate) {
    if (coordinate <= 9 && coordinate >= 0) {
        return true;
    }
    return false;
}

bool GameRule::insideField(int coordinate1, int coordinate2) {
    if (coordinate1 <= 9 && coordinate1 >= 0 && coordinate2 <= 9 && coordinate2 >= 0) {
        return true;
    }
    return false;
}
