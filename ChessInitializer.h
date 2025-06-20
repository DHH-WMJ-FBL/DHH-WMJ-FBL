#pragma once
#include <QList>
#include <QObject>
#include "King.h"
#include "Advisor.h"
#include "Elephant.h"
#include "Horse.h"
#include "Rook.h"
#include "Cannon.h"
#include "Soldier.h"

class ChessInitializer
{
public:
    static inline QList<QObject*> initializePieces(ChessMan* board[10][9]);
};

QList<QObject*> ChessInitializer::initializePieces(
    ChessMan* board[10][9])
{
    QList<QObject*> pieces;

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
    for (int i = 0; i < 5; ++i)
        pieces.append(
            new Soldier(QString("Soldier%1").arg(i + 1), "红", i * 2, 6, "soldier_red.png"));

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
    for (int i = 0; i < 5; ++i)
        pieces.append(
            new Soldier(QString("Soldier%1").arg(i + 6), "黑", i * 2, 3, "soldier_black.png"));

    for (int y = 0; y < 10; ++y)
        for (int x = 0; x < 9; ++x)
            board[y][x] = nullptr;

    for (QObject* obj : pieces) {
        ChessMan* piece = qobject_cast<ChessMan*>(obj);
        if (piece)
            board[piece->y()][piece->x()] = piece;
    }

    return pieces;
}
