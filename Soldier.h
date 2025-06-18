#pragma once
#include "ChessMan.h"

class Soldier : public ChessMan {
    Q_OBJECT

public:
    using ChessMan::ChessMan;

    bool canMove(int targetX, int targetY, ChessMan* board[10][9]) override {
        int dx = targetX - x();
        int dy = targetY - y();

        if (color() == "红" || color() == "red") {
            if (y() >= 5) {
                return dx == 0 && dy == -1 && !isSameColorPieceAt(targetX, targetY, board);
            } else {
                return ((dx == 0 && dy == -1) || (abs(dx) == 1 && dy == 0)) &&
                       !isSameColorPieceAt(targetX, targetY, board);
            }
        } else {
            if (y() <= 4) {
                return dx == 0 && dy == 1 && !isSameColorPieceAt(targetX, targetY, board);
            } else {
                return ((dx == 0 && dy == 1) || (abs(dx) == 1 && dy == 0)) &&
                       !isSameColorPieceAt(targetX, targetY, board);
            }
        }
    }

    bool moveTo(int targetX, int targetY, ChessMan* board[10][9]) override {
        if (canMove(targetX, targetY, board)) {
            setX(targetX);
            setY(targetY);
            return true;
        }
        return false;
    }

    bool canMove(int targetX, int targetY, const QList<QObject*>& allPieces) override {
        ChessMan* board[10][9] = {};
        for (QObject* obj : allPieces) {
            auto* piece = qobject_cast<ChessMan*>(obj);
            if (piece) {
                board[piece->y()][piece->x()] = piece;
            }
        }
        return canMove(targetX, targetY, board);
    }

    bool moveTo(int targetX, int targetY, const QList<QObject*>& allPieces) override {
        if (canMove(targetX, targetY, allPieces)) {
            setX(targetX);
            setY(targetY);
            return true;
        }
        return false;
    }
};
