#pragma once

#include "King.h"
#include "Elephant.h"
#include "Advisor.h"
#include "Rook.h"
#include "Horse.h"
#include "Cannon.h"
#include "Soldier.h"
#include <QList>
#include <QObject>

class ChessInitializer
{
public:
    static QList<QObject*> initializePieces() {
        QList<QObject*> pieces;

        // ---------------- 红方 ----------------
        pieces.append(new King("King1", "红", 4, 9, "king_red.png"));
        pieces.append(new Advisor("Advisor1", "红", 3, 9, "advisor_red.png"));
        pieces.append(new Advisor("Advisor2", "红", 5, 9, "advisor_red.png"));
        pieces.append(new Elephant("Elephant1", "红", 2, 9, "elephant_red.png"));
        pieces.append(new Elephant("Elephant2", "红", 6, 9, "elephant_red.png"));
        pieces.append(new Horse("Horse1", "红", 1, 9, "horse_red.png"));
        pieces.append(new Horse("Horse2", "红", 7, 9, "horse_red.png"));
        pieces.append(new Rook("Rook1", "红", 0, 9, "rook_red.png"));
        pieces.append(new Rook("Rook2", "红", 8, 9, "rook_red.png"));
        pieces.append(new Cannon("Cannon1", "红", 1, 7, "cannon_red.png"));
        pieces.append(new Cannon("Cannon2", "红", 7, 7, "cannon_red.png"));
        pieces.append(new Soldier("Soldier1", "红", 0, 6, "soldier_red.png"));
        pieces.append(new Soldier("Soldier2", "红", 2, 6, "soldier_red.png"));
        pieces.append(new Soldier("Soldier3", "红", 4, 6, "soldier_red.png"));
        pieces.append(new Soldier("Soldier4", "红", 6, 6, "soldier_red.png"));
        pieces.append(new Soldier("Soldier5", "红", 8, 6, "soldier_red.png"));

        // ---------------- 黑方 ----------------
        pieces.append(new King("King2", "黑", 4, 0, "king_black.png"));
        pieces.append(new Advisor("Advisor3", "黑", 3, 0, "advisor_black.png"));
        pieces.append(new Advisor("Advisor4", "黑", 5, 0, "advisor_black.png"));
        pieces.append(new Elephant("Elephant3", "黑", 2, 0, "elephant_black.png"));
        pieces.append(new Elephant("Elephant4", "黑", 6, 0, "elephant_black.png"));
        pieces.append(new Horse("Horse3", "黑", 1, 0, "horse_black.png"));
        pieces.append(new Horse("Horse4", "黑", 7, 0, "horse_black.png"));
        pieces.append(new Rook("Rook3", "黑", 0, 0, "rook_black.png"));
        pieces.append(new Rook("Rook4", "黑", 8, 0, "rook_black.png"));
        pieces.append(new Cannon("Cannon3", "黑", 1, 2, "cannon_black.png"));
        pieces.append(new Cannon("Cannon4", "黑", 7, 2, "cannon_black.png"));
        pieces.append(new Soldier("Soldier6", "黑", 0, 3, "soldier_black.png"));
        pieces.append(new Soldier("Soldier7", "黑", 2, 3, "soldier_black.png"));
        pieces.append(new Soldier("Soldier8", "黑", 4, 3, "soldier_black.png"));
        pieces.append(new Soldier("Soldier9", "黑", 6, 3, "soldier_black.png"));
        pieces.append(new Soldier("Soldier10", "黑", 8, 3, "soldier_black.png"));

        return pieces;
    }
};
