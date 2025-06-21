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
        qDebug() << "炮只能直线移动";
        return false;
    }

    int count = countPiecesBetween(targetX, targetY, board);
    ChessMan *targetPiece = board[targetY][targetX];

    qDebug() << "炮移动检查 - 中间棋子数:" << count
             << "目标位置有棋子:" << (targetPiece != nullptr);

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

int Cannon::countPiecesBetween(
    int targetX, int targetY, ChessMan *board[10][9]) const
{
    // 首先检查自己的坐标是否合法
    if (m_x < 0 || m_x >= 9 || m_y < 0 || m_y >= 10) {
        qDebug() << "炮自身坐标非法:" << m_x << m_y;
        return 0; // 非法坐标，返回0
    }

    // 检查目标坐标是否合法
    if (targetX < 0 || targetX >= 9 || targetY < 0 || targetY >= 10) {
        qDebug() << "炮目标坐标非法:" << targetX << targetY;
        return 0; // 非法坐标，返回0
    }

    // 计算起始点和目标点之间的曼哈顿距离，用于安全检查
    int maxDistance = abs(targetX - m_x) + abs(targetY - m_y);
    if (maxDistance > 20) { // 设置一个合理的最大距离
        qDebug() << "炮移动距离异常大:" << maxDistance << "，起点:(" << m_x << "," << m_y
                 << ")，终点:(" << targetX << "," << targetY << ")";
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
                qDebug() << "炮路径超出棋盘范围Y:" << y << "，结束路径检查";
                break; // 使用break而不是continue，立即终止循环
            }
            if (board[y][targetX]) {
                count++;
                qDebug() << "炮路径上的棋子:" << y << targetX;
            }
        }

        if (safetyCounter >= maxIterations) {
            qDebug() << "炮路径检查Y方向迭代次数过多，可能存在无限循环";
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
                qDebug() << "炮路径超出棋盘范围X:" << x << "，结束路径检查";
                break; // 使用break而不是continue，立即终止循环
            }
            if (board[targetY][x]) {
                count++;
                qDebug() << "炮路径上的棋子:" << targetY << x;
            }
        }

        if (safetyCounter >= maxIterations) {
            qDebug() << "炮路径检查X方向迭代次数过多，可能存在无限循环";
            return 0; // 返回0，认为路径不合法
        }
    }
    return count;
}
