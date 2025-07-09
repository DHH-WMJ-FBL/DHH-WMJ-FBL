#include "ChessAi.h"
#include <QDebug>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <vector>
#include <tuple>


ChessAI::ChessAI() {
    std::srand(std::time(nullptr));
    useClassicAI = true;
}

// 简化的评估函数 - 只考虑基本材料价值
int ChessAI::evaluateBoard(ChessMan* board[10][9], QString playerColor) {
    int score = 0;
    QString opponentColor = (playerColor == "红") ? "黑" : "红";
    
    // 基础材料价值
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            ChessMan* piece = board[y][x];
            if (!piece) continue;
            
            int pieceValue = 0;
            
            // 基础价值
            if (piece->name().contains("King")) pieceValue = 10000;
            else if (piece->name().contains("Rook")) pieceValue = 500;
            else if (piece->name().contains("Horse")) pieceValue = 300;
            else if (piece->name().contains("Cannon")) pieceValue = 300;
            else if (piece->name().contains("Elephant")) pieceValue = 150;
            else if (piece->name().contains("Advisor")) pieceValue = 150;
            else if (piece->name().contains("Soldier")) {
                // 过河兵价值更高
                if ((piece->color() == "红" && y <= 4) || (piece->color() == "黑" && y >= 5)) {
                    pieceValue = 200;  // 过河兵
                } else {
                    pieceValue = 100;  // 未过河兵
                }
            }
            
            if (piece->color() == playerColor) {
                score += pieceValue;
            } else {
                score -= pieceValue;
            }
        }
    }
    
    return score;
}

// 移动排序
void ChessAI::sortMoves(std::vector<std::tuple<ChessMan*, int, int>>& moves, ChessMan* board[10][9]) {
    std::sort(moves.begin(), moves.end(), [board](const auto& a, const auto& b) {
        int toXA = std::get<1>(a), toYA = std::get<2>(a);
        int toXB = std::get<1>(b), toYB = std::get<2>(b);
        
        // 吃子移动优先
        bool captureA = (board[toYA][toXA] != nullptr);
        bool captureB = (board[toYB][toXB] != nullptr);
        return captureA > captureB;
    });
}

// Minimax算法 + Alpha-Beta剪枝
int ChessAI::minimax(ChessMan* board[10][9], int depth, int alpha, int beta, bool maximizingPlayer, QString playerColor, ChessMan*& bestPiece, int& bestToX, int& bestToY) {
    if (depth == 0) {
        return evaluateBoard(board, playerColor);
    }
    
    QString currentColor = maximizingPlayer ? playerColor : ((playerColor == "红") ? "黑" : "红");
    std::vector<std::tuple<ChessMan*, int, int>> moves = generateMoves(board, currentColor);
    
    if (moves.empty()) {
        return maximizingPlayer ? -999999 : 999999;
    }
    
    //移动排序
    sortMoves(moves, board);
    
    if (maximizingPlayer) {
        int maxEval = -999999;
        for (auto& move : moves) {
            ChessMan* piece = std::get<0>(move);
            int toX = std::get<1>(move);
            int toY = std::get<2>(move);
            
            int fromX = piece->x();
            int fromY = piece->y();
            ChessMan* capturedPiece = board[toY][toX];
            
            // 执行移动
            board[fromY][fromX] = nullptr;
            board[toY][toX] = piece;
            piece->setX(toX);
            piece->setY(toY);
            
            ChessMan* tempBestPiece = nullptr;
            int tempBestToX = -1, tempBestToY = -1;
            int eval = minimax(board, depth - 1, alpha, beta, false, playerColor, tempBestPiece, tempBestToX, tempBestToY);
            
            // 恢复移动
            piece->setX(fromX);
            piece->setY(fromY);
            board[fromY][fromX] = piece;
            board[toY][toX] = capturedPiece;
            
            if (eval > maxEval) {
                maxEval = eval;
                if (depth == 4) {
                    bestPiece = piece;
                    bestToX = toX;
                    bestToY = toY;
                }
            }
            
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
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
            ChessMan* capturedPiece = board[toY][toX];
            
            // 执行移动
            board[fromY][fromX] = nullptr;
            board[toY][toX] = piece;
            piece->setX(toX);
            piece->setY(toY);
            
            ChessMan* tempBestPiece = nullptr;
            int tempBestToX = -1, tempBestToY = -1;
            int eval = minimax(board, depth - 1, alpha, beta, true, playerColor, tempBestPiece, tempBestToX, tempBestToY);
            
            // 恢复移动
            piece->setX(fromX);
            piece->setY(fromY);
            board[fromY][fromX] = piece;
            board[toY][toX] = capturedPiece;
            
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

// 生成所有合法移动 - 包含将军检查
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
                                // 检查移动后是否会让自己被将军
                                if (!wouldCauseSelfCheck(piece, tx, ty, board)) {
                                    // 如果被将军，只添加能解除将军的移动
                                    if (isInCheck) {
                                        if (canMoveResolveCheckAI(piece, tx, ty, board)) {
                                            moves.emplace_back(piece, tx, ty);
                                        }
                                    } else {
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
    
    // 如果被将军但没有解除将军的移动，返回所有不会自杀的移动
    if (isInCheck && moves.empty()) {
        for (int y = 0; y < 10; ++y) {
            for (int x = 0; x < 9; ++x) {
                ChessMan* piece = board[y][x];
                if (piece && piece->color() == playerColor) {
                    for (int ty = 0; ty < 10; ++ty) {
                        for (int tx = 0; tx < 9; ++tx) {
                            if ((x != tx || y != ty) && piece->canMove(tx, ty, board)) {
                                if (!wouldCauseKingFacing(piece, tx, ty, board) && 
                                    !wouldCauseSelfCheck(piece, tx, ty, board)) {
                                    moves.emplace_back(piece, tx, ty);
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

// 选择最佳移动
std::tuple<ChessMan*, int, int> ChessAI::selectBestMove(ChessMan* board[10][9], QString playerColor) {
    if (useClassicAI) {
        ChessMan* bestPiece = nullptr;
        int bestToX = -1, bestToY = -1;
        
        // 使用深度4的极大极小算法
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

// === 将军检查相关函数 - 保留完整功能 ===

// 检查移动后是否会造成王对王
bool ChessAI::wouldCauseKingFacing(ChessMan* piece, int toX, int toY, ChessMan* board[10][9]) {
    int fromX = piece->x();
    int fromY = piece->y();
    
    // 暂存目标位置的棋子
    ChessMan* originalTarget = board[toY][toX];
    
    // 执行移动
    board[fromY][fromX] = nullptr;
    board[toY][toX] = piece;
    piece->setX(toX);
    piece->setY(toY);
    
    // 获取双方的王
    ChessMan* redKing = getKing(board, "红");
    ChessMan* blackKing = getKing(board, "黑");
    
    bool wouldCauseFacing = false;
    
    if (redKing && blackKing) {
        // 检查两个王是否在同一列
        if (redKing->x() == blackKing->x()) {
            // 检查两个王之间是否有其他棋子
            int startY = std::min(redKing->y(), blackKing->y()) + 1;
            int endY = std::max(redKing->y(), blackKing->y()) - 1;
            
            bool hasBlocker = false;
            for (int y = startY; y <= endY; ++y) {
                if (board[y][redKing->x()] != nullptr) {
                    hasBlocker = true;
                    break;
                }
            }
            
            if (!hasBlocker) {
                wouldCauseFacing = true;
            }
        }
    }
    
    // 恢复原状态
    piece->setX(fromX);
    piece->setY(fromY);
    board[fromY][fromX] = piece;
    board[toY][toX] = originalTarget;
    
    return wouldCauseFacing;
}

// 检查移动是否能解除将军
bool ChessAI::canMoveResolveCheckAI(ChessMan* piece, int toX, int toY, ChessMan* board[10][9]) {
    if (!checkForCheckAI(board, piece->color())) {
        return true; // 如果没有被将军，任何移动都可以
    }

    int fromX = piece->x();
    int fromY = piece->y();

    // 暂存目标位置的棋子
    ChessMan* targetPiece = board[toY][toX];

    // 暂时执行移动
    board[fromY][fromX] = nullptr;
    board[toY][toX] = piece;
    int oldX = piece->x();
    int oldY = piece->y();
    piece->setX(toX);
    piece->setY(toY);

    // 如果是吃子，暂时将目标棋子移出棋盘
    if (targetPiece) {
        targetPiece->setX(-99);
        targetPiece->setY(-99);
    }

    // 检查移动后是否仍然被将军
    bool stillInCheck = checkForCheckAI(board, piece->color());

    // 恢复原状态
    piece->setX(oldX);
    piece->setY(oldY);
    board[fromY][fromX] = piece;
    board[toY][toX] = targetPiece;

    if (targetPiece) {
        targetPiece->setX(toX);
        targetPiece->setY(toY);
    }

    return !stillInCheck;
}

// 检查移动后是否会让自己被将军
bool ChessAI::wouldCauseSelfCheck(ChessMan* piece, int toX, int toY, ChessMan* board[10][9]) {
    int fromX = piece->x();
    int fromY = piece->y();

    // 暂存目标位置的棋子
    ChessMan* targetPiece = board[toY][toX];

    // 暂时执行移动
    board[fromY][fromX] = nullptr;
    board[toY][toX] = piece;
    int oldX = piece->x();
    int oldY = piece->y();
    piece->setX(toX);
    piece->setY(toY);

    // 如果是吃子，暂时将目标棋子移出棋盘
    if (targetPiece) {
        targetPiece->setX(-99);
        targetPiece->setY(-99);
    }

    // 检查移动后是否会被将军
    bool wouldBeInCheck = checkForCheckAI(board, piece->color());

    // 恢复原状态
    piece->setX(oldX);
    piece->setY(oldY);
    board[fromY][fromX] = piece;
    board[toY][toX] = targetPiece;

    if (targetPiece) {
        targetPiece->setX(toX);
        targetPiece->setY(toY);
    }

    return wouldBeInCheck;
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
    
    QString opponentColor = (color == "红") ? "黑" : "红";
    int kingX = king->x();
    int kingY = king->y();
    
    // 检查是否有敌方棋子能攻击到王
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            ChessMan* piece = board[y][x];
            if (piece && piece->color() == opponentColor) {
                if (piece->canMove(kingX, kingY, board)) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

