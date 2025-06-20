#pragma once
#include "ChessMan.h"
#include <QDebug>

class Rook : public ChessMan {
    Q_OBJECT

public:
    using ChessMan::ChessMan;  // 继承构造函数

    bool canMove(int targetX, int targetY, ChessMan* board[10][9]) override {
        // 车只能直线移动
        if (x() != targetX && y() != targetY) {
            qDebug() << "车只能直线移动";
            return false;
        }

        // 检查路径上是否有障碍物
        if (isBlocked(targetX, targetY, board)) {
            qDebug() << "车移动路径被阻挡";
            return false;
        }

        // 目标位置不能有己方棋子
        if (isSameColorPieceAt(targetX, targetY, board)) {
            qDebug() << "目标位置有己方棋子";
            return false;
        }

        return true;
    }

    // 使用基类的QList版本canMove和moveTo方法

private:
    bool isBlocked(int targetX, int targetY, ChessMan* board[10][9]) const {
        if (x() == targetX) {
            int step = (targetY > y()) ? 1 : -1;
            for (int row = y() + step; row != targetY; row += step) {
                if (board[row][targetX]) {
                    qDebug() << "垂直方向被阻挡，位置:" << targetX << row;
                    return true;
                }
            }
        } else if (y() == targetY) {
            int step = (targetX > x()) ? 1 : -1;
            for (int col = x() + step; col != targetX; col += step) {
                if (board[targetY][col]) {
                    qDebug() << "水平方向被阻挡，位置:" << col << targetY;
                    return true;
                }
            }
        }
        return false;
    }
};
