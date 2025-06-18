#pragma once
#include "ChessMan.h"

class Advisor : public ChessMan {
    Q_OBJECT

public:
    explicit Advisor(QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr)
        : ChessMan(name, color, x, y, icon, parent) {}

    //使用二维数组判断合法性
    bool canMove(int targetX, int targetY, ChessMan* board[10][9]) override {
        int dx = abs(targetX - m_x);
        int dy = abs(targetY - m_y);

        // 必须斜着走一步
        if (dx != 1 || dy != 1) return false;

        if ((color() == "红" || color() == "red") &&
            targetX >= 3 && targetX <= 5 && targetY >= 7 && targetY <= 9) {
            return !isSameColorPieceAt(targetX, targetY, board);
        } else if ((color() == "黑" || color() == "black") &&
                   targetX >= 3 && targetX <= 5 && targetY >= 0 && targetY <= 2) {
            return !isSameColorPieceAt(targetX, targetY, board);
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
