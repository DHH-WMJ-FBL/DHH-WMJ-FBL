#pragma once
#include "ChessMan.h"

class Rook : public ChessMan {
    Q_OBJECT

public:
    using ChessMan::ChessMan;  // 继承构造函数

    bool canMove(int targetX, int targetY, ChessMan* board[10][9]) override {
        if (x() != targetX && y() != targetY)
            return false;

        if (isBlocked(targetX, targetY, board))
            return false;

        return !isSameColorPieceAt(targetX, targetY, board);
    }

    bool moveTo(int targetX, int targetY, ChessMan* board[10][9]) override {
        if (canMove(targetX, targetY, board)) {
            setX(targetX);
            setY(targetY);
            return true;
        }
        return false;
    }

    // QML 兼容版本
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

private:
    bool isBlocked(int targetX, int targetY, ChessMan* board[10][9]) const {
        if (x() == targetX) {
            int step = (targetY > y()) ? 1 : -1;
            for (int row = y() + step; row != targetY; row += step) {
                if (board[row][x()]) return true;
            }
        } else if (y() == targetY) {
            int step = (targetX > x()) ? 1 : -1;
            for (int col = x() + step; col != targetX; col += step) {
                if (board[y()][col]) return true;
            }
        }
        return false;
    }
};
