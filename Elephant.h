#pragma once
#include "ChessMan.h"

class Elephant : public ChessMan {
    Q_OBJECT

public:
    using ChessMan::ChessMan; // 继承构造函数

    bool canMove(int targetX, int targetY, ChessMan* board[10][9]) override {
        int dx = targetX - x();
        int dy = targetY - y();

        // 必须走田字格
        if (abs(dx) == 2 && abs(dy) == 2) {
            // 红方不能过河
            if ((color() == "红" || color() == "red") && targetY <= 4)
                return false;

            // 黑方不能过河
            if ((color() == "黑" || color() == "black") && targetY >= 5)
                return false;

            // 象眼是否被堵
            int eyeX = x() + dx / 2;
            int eyeY = y() + dy / 2;
            if (board[eyeY][eyeX] != nullptr)
                return false;

            // 是否目标位置是己方棋子
            if (isSameColorPieceAt(targetX, targetY, board))
                return false;

            return true;
        }

        return false;
    }

    bool moveTo(int targetX, int targetY, ChessMan* board[10][9]) override {
        if (canMove(targetX, targetY, board)) {
            setX(targetX);
            setY(targetY);
            return true;
        }
        return false;
    }

    //为 QML 保留的接口
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
