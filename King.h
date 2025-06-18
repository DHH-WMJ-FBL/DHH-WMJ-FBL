#pragma once
#include "ChessMan.h"

class King : public ChessMan {
    Q_OBJECT

public:
    using ChessMan::ChessMan; // 继承构造函数

    bool canMove(int targetX, int targetY, ChessMan* board[10][9]) override {
        int dx = abs(targetX - x());
        int dy = abs(targetY - y());

        // 只能横或竖走一格
        if (!((dx == 1 && dy == 0) || (dx == 0 && dy == 1)))
            return false;

        // 判断是否在九宫格内
        if ((color() == "红" || color() == "red")) {
            if (targetX < 3 || targetX > 5 || targetY < 7 || targetY > 9)
                return false;
        } else if ((color() == "黑" || color() == "black")) {
            if (targetX < 3 || targetX > 5 || targetY < 0 || targetY > 2)
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
};
