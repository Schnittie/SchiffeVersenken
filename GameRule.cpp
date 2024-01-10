//
// Created by jonif on 06.01.2024.
//

#include <algorithm>
#include <iostream>
#include "GameRule.h"


bool GameRule::shipAddCorrect(int shipSize, int xPos, int yPos, Direction direction, std::unique_ptr<Board> board) {
    // teste, ob Ursprungspunkt des Schiffs auf dem Feld
    if (shipSize <= 5 && shipSize >= 2 && insideField(xPos, yPos)) { //LLC variable Boardsize? dann wäre es nicht 11, sondern board.shipSize +1 → bereits verändert
        // testen, ob weitere Schiffsfelder innerhalb von Board und nicht neben anderem Schiffsfeld
        int fieldsToSet = shipSize; // shipSize ist am Anfang Größe des zu setzenden bootes
        int recentXPos = xPos; //LLC recent = aktuell
        int recentYPos = yPos;
        while (fieldsToSet > 0) {
            // teste, ob aktuell zu behandelndes Feld unbelegt ist
            if (insideField(recentXPos, recentYPos) && !board->shipField[recentXPos][recentYPos]) {
                // teste, ob Feld links davon unbelegt ist oder außerhalb des Feldes liegt
                bool leftFieldFree = recentXPos - 1 < 0 || !board->shipField[recentXPos -1 ][recentYPos]; //LLC ist 1 jetzt im feld oder außerhalb
                // teste, ob Feld rechts davon unbelegt ist oder außerhalb des Feldes liegt
                bool rightFieldFree = recentXPos + 1 > 9 || !board->shipField[recentXPos + 1][recentYPos];
                // teste, ob Feld darüber unbelegt ist oder außerhalb des Feldes liegt
                bool upperFieldFree = recentYPos - 1 < 0 || !board->shipField[recentXPos][recentYPos - 1];
                // teste, ob Feld darunter unbelegt ist oder außerhalb des Feldes liegt
                bool lowerFieldFree = recentYPos + 1 > 9 || !board->shipField[recentXPos][recentYPos + 1];

                if (leftFieldFree && rightFieldFree && upperFieldFree && lowerFieldFree) {
                    //LLC also ich schaue auf alle umliegenden felder, die if condition wird erfüllt, wenn alle Nachbarn entweder außerhalb des felds oder unbelegt sind
                    // passe Variablen für (je nach Direction) neuem Feld an
                    if (direction == Direction::left) {
                        recentXPos--;
                    } else if (direction == Direction::right) {
                        recentXPos++;
                    } else if (direction == Direction::up) {
                        recentYPos--;
                    } else {
                        recentYPos++;
                    }
                    fieldsToSet--; //LLC mein switch herz weint
                } else {
                    // → Schiff würde an anderes Schiff angrenzen
                    board = nullptr; //LLC schiffe dürfen nicht nebeneinander gesetzt werden? → Korrekt, das ist Teil der Regeln
                    return false;
                }
            } else {
                // → Schiff würde nicht komplett im Feld liegen
                board = nullptr;
                return false;
            }
        }
        board = nullptr;//LLC board = nullptr verstehe ich nicht board ist doch nur innerhalb der methode existent?
        return true;
    } else {
        board = nullptr;
        return false;
    }
}

bool GameRule::shipDestroyed(int xPos, int yPos, std::unique_ptr<Board> board) {
    if (insideField(xPos, yPos) && board->shipField[xPos][yPos]) {//LLC cool wäre doch eine methode die checkt ob eine coordinate im feld ist, würde den code glaube ich leserlicher machen
        // teste, ob alle Felder um Ausgangsfeld entweder außerhalb Board oder ohne Schiffsfeld sind //LLC why
//        if ((xPos - 1 < 1 || !board->shipField[xPos-2][yPos-1])
//        && (xPos + 1 > 10 || !board->shipField[xPos][yPos-1])
//        && (yPos - 1 < 1 || !board->shipField[xPos-1][yPos-2])
//        && (yPos + 1 > 10 || !board->shipField[xPos-1][yPos])) {
//            board = nullptr;
//            return true;
//        } else {
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
                return resultPart; //LLC es wird gechecked ob alle true sind?
            });
//        }
    } else {
        board = nullptr;
        return false;
    }//LLC wegen dieser methode dürfen schiffe nicht direkt nebeneinander sein?
    //LLC ich glaube es wäre besser wenn schiffe eigene objekte wären, muss darüber aber noch nachdenken wenn ich den restlichen code durchgelesen habe
}

bool GameRule::shipInThisDirectionDestroyed(int xPos, int yPos, std::unique_ptr<Board> board, Direction direction) {
    int recentXPos = xPos;
    int recentYPos = yPos;
    while (true) {
        if (direction == Direction::left) {
            if (!insideField(recentXPos - 1) || !board->shipField[recentXPos-1][recentYPos]) {
                // Feld 1 weiter links liegt entweder außerhalb oder es befindet sich kein Schiff darauf
                board = nullptr;
                return true;
            } else if (board->guessField[recentXPos-1][recentYPos] == GuessStatus::guessedRight) {
                // es befindet sich ein Schiff auf dem Feld 1 weiter links (da erster if-Fall nicht eingetroffen und dieses wurde bereits an diesem Feld getroffen
                recentXPos--;
            } else {
                // es befindet sich ein Schiff auf dem Feld 1 weiter links und es wurde z.B. noch nicht beschossen
                board = nullptr;
                return false;
            }
        } else if (direction == Direction::right) {
            if (!insideField(recentXPos + 1) || !board->shipField[recentXPos+1][recentYPos]) {
                board = nullptr;
                return true;
            } else if (board->guessField[recentXPos+1][recentYPos] == GuessStatus::guessedRight) {
                recentXPos++;
            } else {
                board = nullptr;
                return false;
            }
        } else if (direction == Direction::up) {
            if (!insideField(recentYPos - 1) || !board->shipField[recentXPos][recentYPos-1]) {
                board = nullptr;
                return true;
            } else if (board->guessField[recentXPos][recentYPos-1] == GuessStatus::guessedRight) {
                recentYPos--;
            } else {
                board = nullptr;
                return false;
            }
        } else{
            if (!insideField(recentYPos + 1)|| !board->shipField[recentXPos][recentYPos+1]) {
                board = nullptr;
                return true;
            } else if (board->guessField[recentXPos][recentYPos+1] == GuessStatus::guessedRight) {
                recentYPos++;
            } else {
                board = nullptr;
                return false;
            }
        }
//        switch (direction) {
//            case Direction::left: //LLC idk ob das in c++ geht, aber in java würde ich dem Direction Enum noch die information hinzufügen, was es bedeutet z.B. links zu sein
//                if (!insideField(recentXPos - 1) || !board->shipField[recentXPos-1][recentYPos]) {
//                    // Feld 1 weiter links liegt entweder außerhalb oder es befindet sich kein Schiff darauf
//                    board = nullptr;
//                    return true;
//                } else if (board->guessField[recentXPos-1][recentYPos] == GuessStatus::guessedRight) {
//                    // es befindet sich ein Schiff auf dem Feld 1 weiter links (da erster if-Fall nicht eingetroffen und dieses wurde bereits an diesem Feld getroffen
//                    recentXPos--;
//                } else {
//                    // es befindet sich ein Schiff auf dem Feld 1 weiter links und es wurde z.B. noch nicht beschossen
//                    board = nullptr;
//                    return false;
//                }
//            case Direction::right:
//                if (!insideField(recentXPos + 1) || !board->shipField[recentXPos+1][recentYPos]) {
//                    board = nullptr;
//                    return true;
//                } else if (board->guessField[recentXPos+1][recentYPos] == GuessStatus::guessedRight) {
//                    recentXPos++;
//                } else {
//                    board = nullptr;
//                    return false;
//                }
//            case Direction::up:
//                if (!insideField(recentYPos - 1) || !board->shipField[recentXPos][recentYPos-1]) {
//                    board = nullptr;
//                    return true;
//                } else if (board->guessField[recentXPos][recentYPos-1] == GuessStatus::guessedRight) {
//                    recentYPos--;
//                } else {
//                    board = nullptr;
//                    return false;
//                }
//            case Direction::down:
//                if (!insideField(recentYPos + 1)|| !board->shipField[recentXPos][recentYPos+1]) {
//                    board = nullptr;
//                    return true;
//                } else if (board->guessField[recentXPos][recentYPos+1] == GuessStatus::guessedRight) {
//                    recentYPos++;
//                } else {
//                    board = nullptr;
//                    return false;
//                }
//        }
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
