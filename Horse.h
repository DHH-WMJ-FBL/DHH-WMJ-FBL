#pragma once
#include "ChessMan.h"

class Horse : public ChessMan {
    Q_OBJECT

public:
    using ChessMan::ChessMan;

    bool canMove(int targetX, int targetY, ChessMan* board[10][9]) override {
        int dx = targetX - x();
        int dy = targetY - y();

        // 马走日（判断蹩马腿）
        if (abs(dx) == 1 && abs(dy) == 2) {
            int blockY = y() + dy / 2;
            if (board[blockY][x()] != nullptr) return false;
        } else if (abs(dx) == 2 && abs(dy) == 1) {
            int blockX = x() + dx / 2;
            if (board[y()][blockX] != nullptr) return false;
        } else {
            return false;
        }

        // 是否己方棋子
        if (isSameColorPieceAt(targetX, targetY, board))
            return false;

        return true;
    }

    bool moveTo(int targetX, int targetY, ChessMan* board[10][9]) override {
        if (canMove(targetX, targetY, board)) {
            setX(targetX);
            setY(targetY);
            return true;
        }
        return false;
    }

    // QML用版本
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
