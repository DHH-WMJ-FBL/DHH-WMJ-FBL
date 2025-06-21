#pragma once
#include "ChessMan.h"
#include <QDebug>

class Cannon : public ChessMan {
    Q_OBJECT

public:
    explicit Cannon(
        QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr);

    //判断移动是否合法
    bool canMove(int targetX, int targetY, ChessMan* board[10][9]) override;

    // 使用基类的moveTo方法和canMove方法

private:
    //计算直线上两个点之间有多少棋子
    int countPiecesBetween(int targetX, int targetY, ChessMan* board[10][9]) const;
};
