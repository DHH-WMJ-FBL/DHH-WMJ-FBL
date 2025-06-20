#pragma once
#include "ChessMan.h"
#include <QDebug>

class Cannon : public ChessMan {
    Q_OBJECT

public:
    explicit Cannon(QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr)
        : ChessMan(name, color, x, y, icon, parent) {}

    //判断移动是否合法
    bool canMove(int targetX, int targetY, ChessMan* board[10][9]) override {
        // 必须是直线
        if (m_x != targetX && m_y != targetY) {
            qDebug() << "炮只能直线移动";
            return false;
        }

        int count = countPiecesBetween(targetX, targetY, board);
        ChessMan* targetPiece = board[targetY][targetX];
        
        qDebug() << "炮移动检查 - 中间棋子数:" << count << "目标位置有棋子:" << (targetPiece != nullptr);

        // 不吃子：炮路径上不能有其他棋子
        if (!targetPiece && count == 0) {
            qDebug() << "炮移动 - 路径无棋子";
            return true;
        }

        // 吃子：中间必须隔一个
        if (targetPiece && targetPiece->color() != color() && count == 1) {
            qDebug() << "炮吃子 - 正好隔一个棋子";
            return true;
        }
        
        if (targetPiece && targetPiece->color() == color()) {
            qDebug() << "炮移动失败 - 目标位置是己方棋子";
        } else if (count > 1) {
            qDebug() << "炮移动失败 - 路径上有多个棋子";
        } else if (targetPiece && count == 0) {
            qDebug() << "炮移动失败 - 吃子时没有隔子";
        } else if (!targetPiece && count > 0) {
            qDebug() << "炮移动失败 - 移动时路径有障碍";
        }

        return false;
    }

    // 使用基类的moveTo方法和canMove方法

private:
    //计算直线上两个点之间有多少棋子
    int countPiecesBetween(int targetX, int targetY, ChessMan* board[10][9]) const {
        int count = 0;
        if (targetX == m_x) {
            int step = (targetY > m_y) ? 1 : -1;
            for (int y = m_y + step; y != targetY; y += step) {
                if (board[y][targetX]) {
                    count++;
                    qDebug() << "炮路径上的棋子:" << y << targetX;
                }
            }
        } else if (targetY == m_y) {
            int step = (targetX > m_x) ? 1 : -1;
            for (int x = m_x + step; x != targetX; x += step) {
                if (board[targetY][x]) {
                    count++;
                    qDebug() << "炮路径上的棋子:" << targetY << x;
                }
            }
        }
        return count;
    }
};
