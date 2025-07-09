#include "ChessAi.h"
#include <QDebug>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <vector>
#include <tuple>
#include <limits>

// 中国象棋AI实现
// 参考GitHub项目: colingogogo/gobang_AI 的极大极小算法
// 和 huoxin4415/min-max 的α-β剪枝优化
// 
// 算法核心:
// 1. 极大极小算法(Minimax) - 博弈树搜索
// 2. Alpha-Beta剪枝 - 减少搜索空间
// 3. 位置价值表 - 提升评估精度
// 4. 移动排序 - 提高剪枝效率

ChessAI::ChessAI() {
    std::srand(std::time(nullptr));
    useClassicAI = true;
}

// 棋子位置价值表 - 参考经典象棋引擎
// 不同棋子在不同位置的价值权重
// 中心位置和进攻位置价值更高
namespace PiecePositionValue {
    // 兵/卒位置价值 (红方视角)
    const int PAWN_VALUE[10][9] = {
        {0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0,  0,  0,  0,  0,  0,  0,  0,  0},
        {0,  0,  0,  0,  0,  0,  0,  0,  0},
        {10, 10, 10, 10, 10, 10, 10, 10, 10},
        {20, 20, 20, 30, 30, 30, 20, 20, 20},
        {30, 30, 30, 40, 40, 40, 30, 30, 30},
        {40, 40, 40, 50, 50, 50, 40, 40, 40},
        {50, 50, 50, 60, 60, 60, 50, 50, 50}
    };
    
    // 车位置价值
    const int ROOK_VALUE[10][9] = {
        {20, 20, 20, 20, 20, 20, 20, 20, 20},
        {20, 20, 20, 20, 20, 20, 20, 20, 20},
        {20, 20, 20, 20, 20, 20, 20, 20, 20},
        {20, 20, 20, 20, 20, 20, 20, 20, 20},
        {20, 20, 20, 20, 20, 20, 20, 20, 20},
        {20, 20, 20, 20, 20, 20, 20, 20, 20},
        {20, 20, 20, 20, 20, 20, 20, 20, 20},
        {20, 20, 20, 20, 20, 20, 20, 20, 20},
        {20, 20, 20, 20, 20, 20, 20, 20, 20},
        {20, 20, 20, 20, 20, 20, 20, 20, 20}
    };
}

// 增强的评估函数 - 参考多个GitHub项目的评估策略
// 综合考虑:
// 1. 棋子基础价值
// 2. 位置价值
// 3. 机动性(可走位置数)
// 4. 中心控制
int ChessAI::evaluateBoard(ChessMan* board[10][9], QString playerColor) {
    int score = 0;
    QString opponentColor = (playerColor == "红") ? "黑" : "红";
    
    // 1. 基础材料价值 + 位置价值
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            ChessMan* piece = board[y][x];
            if (!piece) continue;
            
            int pieceValue = 0;
            int positionValue = 0;
            
            // 基础价值
            if (piece->name().contains("King")) pieceValue = 10000;
            else if (piece->name().contains("Rook")) pieceValue = 500;
            else if (piece->name().contains("Horse")) pieceValue = 300;
            else if (piece->name().contains("Cannon")) pieceValue = 300;
            else if (piece->name().contains("Elephant")) pieceValue = 150;
            else if (piece->name().contains("Advisor")) pieceValue = 150;
            else if (piece->name().contains("Soldier")) {
                pieceValue = 100;
                // 兵的位置价值
                if (piece->color() == "红") {
                    positionValue = PiecePositionValue::PAWN_VALUE[y][x];
                } else {
                    positionValue = PiecePositionValue::PAWN_VALUE[9-y][x];
                }
            }
            
            // 车的位置价值
            if (piece->name().contains("Rook")) {
                positionValue = PiecePositionValue::ROOK_VALUE[y][x];
            }
            
            int totalValue = pieceValue + positionValue;
            
            if (piece->color() == playerColor) {
                score += totalValue;
            } else {
                score -= totalValue;
            }
        }
    }
    
    // 2. 机动性评估 - 可走位置越多越好
    int mobility = calculateMobility(board, playerColor) - calculateMobility(board, opponentColor);
    score += mobility * 2;
    
    // 3. 中心控制评估
    int centerControl = evaluateCenterControl(board, playerColor) - evaluateCenterControl(board, opponentColor);
    score += centerControl * 3;
    
    return score;
}

// 计算机动性 - 可走位置数量
int ChessAI::calculateMobility(ChessMan* board[10][9], QString color) {
    int mobility = 0;
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            ChessMan* piece = board[y][x];
            if (piece && piece->color() == color) {
                // 计算该棋子可走的位置数
                for (int ty = 0; ty < 10; ++ty) {
                    for (int tx = 0; tx < 9; ++tx) {
                        if ((x != tx || y != ty) && piece->canMove(tx, ty, board)) {
                            mobility++;
                        }
                    }
                }
            }
        }
    }
    return mobility;
}

// 评估中心控制 - 控制中心区域的价值
int ChessAI::evaluateCenterControl(ChessMan* board[10][9], QString color) {
    int centerValue = 0;
    // 中心区域坐标
    int centerX[] = {3, 4, 5};
    int centerY[] = {3, 4, 5, 6};
    
    for (int cy : centerY) {
        for (int cx : centerX) {
            ChessMan* piece = board[cy][cx];
            if (piece && piece->color() == color) {
                centerValue += 10; // 占据中心位置奖励
            }
        }
    }
    return centerValue;
}

// 移动排序 - 提高Alpha-Beta剪枝效率
// 参考GitHub项目的移动排序策略:
// 1. 吃子移动优先
// 2. 将军移动优先
// 3. 中心移动优先
void ChessAI::sortMoves(std::vector<std::tuple<ChessMan*, int, int>>& moves, ChessMan* board[10][9]) {
    std::sort(moves.begin(), moves.end(), [this, board](const auto& a, const auto& b) {
        ChessMan* pieceA = std::get<0>(a);
        int toXA = std::get<1>(a), toYA = std::get<2>(a);
        ChessMan* pieceB = std::get<0>(b);
        int toXB = std::get<1>(b), toYB = std::get<2>(b);
        
        // 吃子移动优先
        bool captureA = (board[toYA][toXA] != nullptr);
        bool captureB = (board[toYB][toXB] != nullptr);
        if (captureA != captureB) return captureA;
        
        // 中心移动优先
        int centerScoreA = (abs(toXA - 4) + abs(toYA - 5));
        int centerScoreB = (abs(toXB - 4) + abs(toYB - 5));
        return centerScoreA < centerScoreB;
    });
}

// 增强的极大极小算法 - 参考经典实现
// 核心改进:
// 1. 增加搜索深度到4层
// 2. 更精确的Alpha-Beta剪枝
// 3. 移动排序优化
// 4. 更好的评估函数
int ChessAI::minimax(ChessMan* board[10][9], int depth, int alpha, int beta, bool maximizingPlayer, QString playerColor, ChessMan*& bestPiece, int& bestToX, int& bestToY) {
    // 到达搜索深度或游戏结束
    if (depth == 0) {
        return evaluateBoard(board, playerColor);
    }
    
    QString currentColor = maximizingPlayer ? playerColor : (playerColor == "红" ? "黑" : "红");
    auto moves = generateMoves(board, currentColor);
    
    if (moves.empty()) {
        return maximizingPlayer ? -999999 : 999999;
    }
    
    // 移动排序 - 提高剪枝效率
    sortMoves(moves, board);
    
    if (maximizingPlayer) {
        int maxEval = -999999;
        for (auto& move : moves) {
            ChessMan* piece = std::get<0>(move);
            int toX = std::get<1>(move);
            int toY = std::get<2>(move);
            int fromX = piece->x();
            int fromY = piece->y();
            
            // 执行移动
            ChessMan* captured = board[toY][toX];
            board[fromY][fromX] = nullptr;
            board[toY][toX] = piece;
            piece->setX(toX);
            piece->setY(toY);
            
            // 立即胜利检测
            if (captured && captured->name().contains("King")) {
                // 撤销移动
                piece->setX(fromX);
                piece->setY(fromY);
                board[fromY][fromX] = piece;
                board[toY][toX] = captured;
                
                if (depth == 4) {
                    bestPiece = piece;
                    bestToX = toX;
                    bestToY = toY;
                }
                return 999999;
            }
            
            // 递归搜索
            int eval = minimax(board, depth - 1, alpha, beta, false, playerColor, bestPiece, bestToX, bestToY);
            
            // 撤销移动
            piece->setX(fromX);
            piece->setY(fromY);
            board[fromY][fromX] = piece;
            board[toY][toX] = captured;
            
            if (eval > maxEval) {
                maxEval = eval;
                if (depth == 4) {
                    bestPiece = piece;
                    bestToX = toX;
                    bestToY = toY;
                }
            }
            
            // Alpha-Beta剪枝
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                break; // Beta剪枝
            }
        }
        return maxEval;
    } else {
        int minEval = 999999;
        for (auto& move : moves) {
            ChessMan* piece = std::get<0>(move);
            int toX = std::get<1>(move);
            int toY = std::get<2>(move);
            int fromX = piece->x();
            int fromY = piece->y();
            
            // 执行移动
            ChessMan* captured = board[toY][toX];
            board[fromY][fromX] = nullptr;
            board[toY][toX] = piece;
            piece->setX(toX);
            piece->setY(toY);
            
            // 立即胜利检测
            if (captured && captured->name().contains("King")) {
                // 撤销移动
                piece->setX(fromX);
                piece->setY(fromY);
                board[fromY][fromX] = piece;
                board[toY][toX] = captured;
                return -999999;
            }
            
            // 递归搜索
            int eval = minimax(board, depth - 1, alpha, beta, true, playerColor, bestPiece, bestToX, bestToY);
            
            // 撤销移动
            piece->setX(fromX);
            piece->setY(fromY);
            board[fromY][fromX] = piece;
            board[toY][toX] = captured;
            
            minEval = std::min(minEval, eval);
            
            // Alpha-Beta剪枝
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break; // Alpha剪枝
            }
        }
        return minEval;
    }
}

// 生成所有合法移动
std::vector<std::tuple<ChessMan*, int, int>> ChessAI::generateMoves(ChessMan* board[10][9], QString playerColor) {
    std::vector<std::tuple<ChessMan*, int, int>> moves;
    bool isInCheck = checkForCheckAI(board, playerColor);
    
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            ChessMan* piece = board[y][x];
            if (piece && piece->color() == playerColor) {
                for (int ty = 0; ty < 10; ++ty) {
                    for (int tx = 0; tx < 9; ++tx) {
                        if ((x != tx || y != ty) && piece->canMove(tx, ty, board)) {
                            // 检查移动后是否会造成王对王
                            if (!wouldCauseKingFacing(piece, tx, ty, board)) {
                                // 如果被将军，只添加能解除将军的移动
                                if (isInCheck) {
                                    if (canMoveResolveCheckAI(piece, tx, ty, board)) {
                                        moves.emplace_back(piece, tx, ty);
                                    }
                                } else {
                                    // 未被将军时，检查移动是否会导致自将军
                                    if (!wouldCauseSelfCheck(piece, tx, ty, board)) {
                                        moves.emplace_back(piece, tx, ty);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return moves;
}

// 主要AI入口 - 选择最佳移动
std::tuple<ChessMan*, int, int> ChessAI::selectBestMove(ChessMan* board[10][9], QString playerColor) {
    if (useClassicAI) {
        ChessMan* bestPiece = nullptr;
        int bestToX = -1, bestToY = -1;
        
        // 使用深度4的极大极小算法 + Alpha-Beta剪枝
        minimax(board, 4, -999999, 999999, true, playerColor, bestPiece, bestToX, bestToY);
        
        if (bestPiece) {
            return std::make_tuple(bestPiece, bestToX, bestToY);
        }
    }
    
    // 备选随机移动
    std::vector<std::tuple<ChessMan*, int, int>> moves = generateMoves(board, playerColor);
    if (moves.empty()) return std::make_tuple(nullptr, -1, -1);
    
    int idx = std::rand() % moves.size();
    return moves[idx];
}

void ChessAI::setUseClassicAI(bool useClassic) {
    useClassicAI = useClassic;
}

bool ChessAI::getUseClassicAI() const {
    return useClassicAI;
}

// 王对王检查函数 - 防止AI造成王对王
bool ChessAI::wouldCauseKingFacing(ChessMan* piece, int toX, int toY, ChessMan* board[10][9]) {
    // 创建临时棋盘副本
    ChessMan* tempBoard[10][9];
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            tempBoard[y][x] = board[y][x];
        }
    }
    
    // 模拟移动
    int fromX = piece->x();
    int fromY = piece->y();
    tempBoard[fromY][fromX] = nullptr;
    tempBoard[toY][toX] = piece;
    
    // 检查王对王
    ChessMan* redKing = getKing(tempBoard, "红");
    ChessMan* blackKing = getKing(tempBoard, "黑");
    
    if (!redKing || !blackKing) return false;
    
    // 获取移动后的王的位置
    int redKingX = (redKing == piece) ? toX : redKing->x();
    int redKingY = (redKing == piece) ? toY : redKing->y();
    int blackKingX = (blackKing == piece) ? toX : blackKing->x();
    int blackKingY = (blackKing == piece) ? toY : blackKing->y();
    
    // 检查是否在同一列
    if (redKingX == blackKingX) {
        int minY = qMin(redKingY, blackKingY);
        int maxY = qMax(redKingY, blackKingY);
        
        bool blocked = false;
        for (int y = minY + 1; y < maxY; ++y) {
            if (tempBoard[y][redKingX]) {
                blocked = true;
                break;
            }
        }
        return !blocked; // 如果没有阻挡，则是王对王
    }
    
    return false;
}

// AI版本的移动解除将军检查函数
bool ChessAI::canMoveResolveCheckAI(ChessMan* piece, int toX, int toY, ChessMan* board[10][9]) {
    if (!checkForCheckAI(board, piece->color())) {
        return true; // 如果没有被将军，任何移动都可以
    }

    int fromX = piece->x();
    int fromY = piece->y();

    // 暂存目标位置的棋子（如果有）
    ChessMan* targetPiece = board[toY][toX];

    // 暂时执行这个移动
    board[fromY][fromX] = nullptr;
    board[toY][toX] = piece;
    int oldX = piece->x();
    int oldY = piece->y();
    piece->setX(toX);
    piece->setY(toY);

    // 如果是吃子，暂时将目标棋子移出棋盘
    int targetOldX = -99;
    int targetOldY = -99;
    bool isCapture = targetPiece != nullptr;

    if (isCapture) {
        // 保存原始坐标
        targetOldX = targetPiece->x();
        targetOldY = targetPiece->y();
        // 临时设置为棋盘外位置
        targetPiece->setX(-99);
        targetPiece->setY(-99);
    }

    // 检查移动后是否仍然被将军
    bool stillInCheck = checkForCheckAI(board, piece->color());

    // 恢复棋盘状态
    piece->setX(oldX);
    piece->setY(oldY);
    board[fromY][fromX] = piece;
    board[toY][toX] = targetPiece;

    // 如果是吃子，恢复被吃掉的棋子状态
    if (isCapture && targetPiece) {
        targetPiece->setX(targetOldX);
        targetPiece->setY(targetOldY);
    }

    // 如果这个移动可以解除将军，则返回true
    return !stillInCheck;
}

// 在棋盘上找到指定颜色的王
ChessMan* ChessAI::getKing(ChessMan* board[10][9], QString color) {
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            ChessMan* piece = board[y][x];
            if (piece && piece->name().contains("King") && piece->color() == color) {
                return piece;
            }
        }
    }
    return nullptr;
}

// AI版本的将军检查函数
bool ChessAI::checkForCheckAI(ChessMan* board[10][9], QString color) {
    ChessMan* king = getKing(board, color);
    if (!king || king->x() < 0 || king->x() >= 9 || king->y() < 0 || king->y() >= 10) {
        return false;
    }

    int kingX = king->x(), kingY = king->y();

    // 检查是否有敌方棋子能攻击到王
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            ChessMan* piece = board[y][x];
            if (piece && piece->color() != color && piece->x() >= 0 && piece->y() >= 0 && piece->canMove(kingX, kingY, board)) {
                return true;
            }
        }
    }

    return false;
}

// 检查移动是否会导致自己被将军
bool ChessAI::wouldCauseSelfCheck(ChessMan* piece, int toX, int toY, ChessMan* board[10][9]) {
    int fromX = piece->x();
    int fromY = piece->y();

    // 暂存目标位置的棋子（如果有）
    ChessMan* targetPiece = board[toY][toX];

    // 暂时执行这个移动
    board[fromY][fromX] = nullptr;
    board[toY][toX] = piece;
    int oldX = piece->x();
    int oldY = piece->y();
    piece->setX(toX);
    piece->setY(toY);

    // 如果是吃子，暂时将目标棋子移出棋盘
    int targetOldX = -99;
    int targetOldY = -99;
    bool isCapture = targetPiece != nullptr;

    if (isCapture) {
        // 保存原始坐标
        targetOldX = targetPiece->x();
        targetOldY = targetPiece->y();
        // 临时设置为棋盘外位置
        targetPiece->setX(-99);
        targetPiece->setY(-99);
    }

    // 检查移动后是否会被将军
    bool wouldBeInCheck = checkForCheckAI(board, piece->color());

    // 恢复棋盘状态
    piece->setX(oldX);
    piece->setY(oldY);
    board[fromY][fromX] = piece;
    board[toY][toX] = targetPiece;

    // 如果是吃子，恢复被吃掉的棋子状态
    if (isCapture && targetPiece) {
        targetPiece->setX(targetOldX);
        targetPiece->setY(targetOldY);
    }

    // 返回是否会被将军
    return wouldBeInCheck;
}

