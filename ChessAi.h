#pragma once
#include "ChessMan.h"
#include <tuple>

class ChessAI
{
public:
    // 选择最佳移动，返回:棋子指针, 目标X坐标, 目标Y坐标
    std::tuple<ChessMan*, int, int> selectBestMove(ChessMan* board[10][9], QString playerColor);

private:
    // 获取棋子价值分数
    int getScore(const QString& name);
};
