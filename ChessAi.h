#pragma once
#include "ChessMan.h"
#include <tuple>
#include <QMap>
#include <QVector>
#include <QElapsedTimer>
#include <QHash>

// 棋子基础价值表
const int PIECE_VALUE_KING = 10000;    // 将/帅
const int PIECE_VALUE_ADVISOR = 120;   // 士/仕
const int PIECE_VALUE_ELEPHANT = 120;  // 象/相
const int PIECE_VALUE_HORSE = 270;     // 马
const int PIECE_VALUE_ROOK = 500;      // 车
const int PIECE_VALUE_CANNON = 285;    // 炮
const int PIECE_VALUE_PAWN = 30;       // 兵/卒

// 最大搜索深度
const int MAX_SEARCH_DEPTH = 4;

// 无穷大值，用于alpha-beta剪枝
const int INFINITY_VALUE = 1000000;

// 置换表的表项类型
enum TranspositionFlag {
    EXACT,  // 精确值
    LOWER,  // 下界
    UPPER   // 上界
};

// 置换表条目
struct TranspositionEntry {
    qint64 zobristKey;    // Zobrist哈希键
    int depth;            // 搜索深度
    TranspositionFlag flag; // 表项类型
    int score;            // 评分
    ChessMan* bestPiece;  // 最佳棋子
    int bestToX;          // 最佳目标X坐标
    int bestToY;          // 最佳目标Y坐标
    
    TranspositionEntry() : zobristKey(0), depth(0), flag(EXACT), score(0), bestPiece(nullptr), bestToX(-1), bestToY(-1) {}
};

// AI配置选项
struct AIConfig {
    bool useIterativeDeepening = true; // 是否使用迭代加深
    bool useTranspositionTable = true; // 是否使用置换表
    bool useHistoryHeuristics = true;  // 是否使用历史启发表
    bool useQuiescenceSearch = true;   // 是否使用静态搜索
    bool useMobilityEvaluation = true; // 是否评估机动性
    bool useCenterControl = true;      // 是否评估中心控制
    
    AIConfig() {}
};

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
    
    // 新增AI优化函数
    int calculateMobility(ChessMan* board[10][9], QString color);
    int evaluateCenterControl(ChessMan* board[10][9], QString color);
    void sortMoves(std::vector<std::tuple<ChessMan*, int, int>>& moves, ChessMan* board[10][9]);
    
    bool useClassicAI = true;
};
