#include "Cannon.h"

Cannon::Cannon(
    QString name, QString color, int x, int y, QString icon, QObject *parent)
    : ChessMan(name, color, x, y, icon, parent)
{}

bool Cannon::canMove(
    int targetX, int targetY, ChessMan *board[10][9])
{
    // 必须是直线
    if (m_x != targetX && m_y != targetY) {
        return false;
    }

    int count = countPiecesBetween(targetX, targetY, board);
    ChessMan *targetPiece = board[targetY][targetX];

    // 不吃子：炮路径上不能有其他棋子
    if (!targetPiece && count == 0) {
        return true;
    }

    // 吃子：中间必须隔一个
    if (targetPiece && targetPiece->color() != color() && count == 1) {
        return true;
    }

    return false;
}

int Cannon::countPiecesBetween(
    int targetX, int targetY, ChessMan *board[10][9]) const
{
    // 首先检查自己的坐标是否合法
    if (m_x < 0 || m_x >= 9 || m_y < 0 || m_y >= 10) {
        return 0; // 非法坐标，返回0
    }

    // 检查目标坐标是否合法
    if (targetX < 0 || targetX >= 9 || targetY < 0 || targetY >= 10) {
        return 0; // 非法坐标，返回0
    }

    // 计算起始点和目标点之间的曼哈顿距离，用于安全检查
    int maxDistance = abs(targetX - m_x) + abs(targetY - m_y);
    if (maxDistance > 20) { // 设置一个合理的最大距离
        return 0;
    }

    int count = 0;
    if (targetX == m_x) {
        int step = (targetY > m_y) ? 1 : -1;
        int safetyCounter = 0;
        int maxIterations = 10; // 最大迭代次数

        for (int y = m_y + step; y != targetY && safetyCounter < maxIterations;
             y += step, safetyCounter++) {
            // 添加边界检查
            if (y < 0 || y >= 10) {
                break; //立即终止循环
            }
            if (board[y][targetX]) {
                count++;
            }
        }

        if (safetyCounter >= maxIterations) {
            return 0; // 返回0，认为路径不合法
        }
    } else if (targetY == m_y) {
        int step = (targetX > m_x) ? 1 : -1;
        int safetyCounter = 0;
        int maxIterations = 10; // 最大迭代次数

        for (int x = m_x + step; x != targetX && safetyCounter < maxIterations;
             x += step, safetyCounter++) {
            // 添加边界检查
            if (x < 0 || x >= 9) {
                break; // 使用break而不是continue，立即终止循环
            }
            if (board[targetY][x]) {
                count++;
            }
        }

        if (safetyCounter >= maxIterations) {
            return 0; // 返回0，认为路径不合法
        }
    }
    return count;
}
