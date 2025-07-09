#pragma once
#include "ChessMan.h"
#include <tuple>
#include <vector>

class ChessAI
{
public:
    ChessAI();
    // 选择最佳移动，返回:棋子指针, 目标X坐标, 目标Y坐标
    std::tuple<ChessMan*, int, int> selectBestMove(ChessMan* board[10][9], QString playerColor);
    
    // 切换AI模式（经典/随机）
    void setUseClassicAI(bool useClassic);
    bool getUseClassicAI() const;

private:
    // 经典AI相关
    int evaluateBoard(ChessMan* board[10][9], QString playerColor);
    std::vector<std::tuple<ChessMan*, int, int>> generateMoves(ChessMan* board[10][9], QString playerColor);
    int minimax(ChessMan* board[10][9], int depth, int alpha, int beta, bool maximizingPlayer, QString playerColor, ChessMan*& bestPiece, int& bestToX, int& bestToY);
    void sortMoves(std::vector<std::tuple<ChessMan*, int, int>>& moves, ChessMan* board[10][9]);
    
    // 王对王检查函数
    bool wouldCauseKingFacing(ChessMan* piece, int toX, int toY, ChessMan* board[10][9]);
    ChessMan* getKing(ChessMan* board[10][9], QString color);
    
    // 将军检查相关函数
    bool checkForCheckAI(ChessMan* board[10][9], QString color);
    bool canMoveResolveCheckAI(ChessMan* piece, int toX, int toY, ChessMan* board[10][9]);
    bool wouldCauseSelfCheck(ChessMan* piece, int toX, int toY, ChessMan* board[10][9]);
    
    bool useClassicAI = true;
};
