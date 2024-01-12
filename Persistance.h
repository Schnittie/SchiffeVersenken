//
// Created by laure on 22.12.2023.
//

#ifndef SCHIFFEVERSENKEN_PROJEKT__PERSISTANCE_H
#define SCHIFFEVERSENKEN_PROJEKT__PERSISTANCE_H


#include <string>
#include <memory>
#include "Board.h"

class Board;
class Persistance {
public:
    static void saveGame(std::unique_ptr<Board>,std::unique_ptr<Board>,int);
private:
    static void saveString(std::string, std::string );
    static std::string turnBoardIntoString(std::unique_ptr<Board>);

//TODO: Hier kommt das Speichern und laden von Spielständen hin, das einzige was wirklich gespeichert werden muss ist wie das Board aussieht, da immer wenn das Speichern möglich ist sowieso der Spieler dran ist
//TODO: evtl. falls wir den Opponent fancy machen müssten wir noch speichern gegen welche Art von Opponent gespielt wird
};


#endif //SCHIFFEVERSENKEN_PROJEKT__PERSISTANCE_H
