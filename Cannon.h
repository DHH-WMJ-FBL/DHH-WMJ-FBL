#pragma once
#include "ChessMan.h"

class Cannon : public ChessMan {
    Q_OBJECT

public:
    explicit Cannon(QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr)
        : ChessMan(name, color, x, y, icon, parent) {}

    //判断移动是否合法
    bool canMove(int targetX, int targetY, ChessMan* board[10][9]) override {
        // 必须是直线
        if (m_x != targetX && m_y != targetY)
            return false;

        int count = countPiecesBetween(targetX, targetY, board);
        ChessMan* targetPiece = board[targetY][targetX];

        // 不吃子：炮路径上不能有其他棋子
        if (!targetPiece && count == 0)
            return true;

        // 吃子：中间必须隔一个
        if (targetPiece && count == 1)
            return !isSameColorPieceAt(targetX, targetY, board);

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
            if (piece)
                board[piece->y()][piece->x()] = piece;
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
    //计算直线上两个点之间有多少棋子
    int countPiecesBetween(int targetX, int targetY, ChessMan* board[10][9]) const {
        int count = 0;
        if (targetX == m_x) {
            int step = (targetY > m_y) ? 1 : -1;
            for (int y = m_y + step; y != targetY; y += step) {
                if (board[y][targetX])
                    count++;
            }
        } else if (targetY == m_y) {
            int step = (targetX > m_x) ? 1 : -1;
            for (int x = m_x + step; x != targetX; x += step) {
                if (board[targetY][x])
                    count++;
            }
        }
        return count;
    }
};
