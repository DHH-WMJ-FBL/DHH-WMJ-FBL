#pragma once
#include "ChessMan.h"
#include <vector>
#include <tuple>

class ChessAI
{
public:
    std::tuple<ChessMan*, int, int> selectBestMove(ChessMan* board[10][9], const QString& aiColor);

private:
    std::vector<std::tuple<ChessMan*, int, int, int>> generateAllMoves(ChessMan* board[10][9],
                                                                       const QString& color);
    int evaluate(ChessMan* board[10][9], const QString& aiColor);
    int getScore(const QString& name);
};
