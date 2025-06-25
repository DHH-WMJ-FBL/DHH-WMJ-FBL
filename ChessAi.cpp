#include "ChessAi.h"
#include <QDebug>
#include <cstdlib>
#include <ctime>

std::tuple<ChessMan*, int, int> ChessAI::selectBestMove(
    ChessMan* board[10][9], QString playerColor)
{
    qDebug() << "\n======AI开始选择落子======";
    
    // 添加参数检查
    if (playerColor != "黑" && playerColor != "红") {
        qDebug() << "错误：AI选择落子的棋子颜色参数无效:" << playerColor;
        return std::make_tuple(nullptr, -1, -1);
    }
    qDebug() << "AI负责的棋子颜色:" << playerColor;

    ChessMan* bestPiece = nullptr;
    int bestX = -1, bestY = -1;
    int bestScore = -99999;
    
    // 统计指定颜色的棋子数量
    int pieceCount = 0;
    QList<ChessMan*> myPieces;

    // 打印当前棋盘状态
    qDebug() << "AI分析的棋盘状态:";
    for (int y = 0; y < 10; ++y) {
        QString line;
        for (int x = 0; x < 9; ++x) {
            if (board[y][x]) {
                line += board[y][x]->name().left(2) + " ";
                if (board[y][x]->color() == playerColor) {
                    pieceCount++;
                    myPieces.append(board[y][x]);
                }
            } else {
                line += ".. ";
            }
        }
        qDebug() << line;
    }
    
    qDebug() << "找到" << playerColor << "方棋子:" << pieceCount << "个";
    
    // 如果没有找到棋子，返回空
    if (pieceCount == 0 || myPieces.isEmpty()) {
        qDebug() << "错误：没有找到" << playerColor << "方的棋子!";
        return std::make_tuple(nullptr, -1, -1);
    }

    // 随机打乱顺序，增加变化性
    std::srand(std::time(nullptr));
    for (int i = myPieces.size() - 1; i > 0; --i) {
        int j = std::rand() % (i + 1);
        myPieces.swapItemsAt(i, j);
    }
    
    int inspectedMoves = 0;
    int validMoves = 0;
    const int debugLimit = 20; // 限制打印的调试信息数量
    
    // 遍历所有指定颜色的棋子
    for (ChessMan* piece : myPieces) {
        if (!piece || piece->x() < 0 || piece->x() >= 9 || piece->y() < 0 || piece->y() >= 10) {
            continue; // 跳过无效的棋子
        }

        qDebug() << "检查棋子:" << piece->name() << "位置:(" << piece->x() << "," << piece->y() << ")";
        
        // 计算所有可能的移动
        // 中国象棋棋盘是9x10
        for (int toY = 0; toY < 10; ++toY) {
            for (int toX = 0; toX < 9; ++toX) {
                inspectedMoves++;
                
                // 如果目标位置有自己的棋子，跳过
                if (board[toY][toX] && board[toY][toX]->color() == playerColor) {
                    continue;
                }
                
                // 检查该棋子是否可以移动到目标位置
                bool canMove = piece->canMove(toX, toY, board);
                
                if (validMoves < debugLimit) {
                    qDebug() << "  尝试移动" << piece->name() << "到(" << toX << "," << toY << "): "
                             << (canMove ? "可行" : "不可行");
                }

                if (canMove) {
                    validMoves++;
                    
                    // 计算移动得分
                    int score = 0;
                    
                    // 如果可以吃子，加上该子的分值
                    if (board[toY][toX]) {
                        int captureScore = getScore(board[toY][toX]->name());
                        score += captureScore;
                        
                        if (validMoves < debugLimit) {
                            qDebug() << "    可以吃子:" << board[toY][toX]->name() << "得分:" << captureScore;
                        }
                    }
                    
                    // 随机添加一些分数，避免AI每次走相同的棋
                    int randomScore = std::rand() % 10;
                    score += randomScore;
                    
                    if (validMoves < debugLimit) {
                        qDebug() << "    总得分:" << score << "(含随机分" << randomScore << ")";
                    }
                    
                    if (score > bestScore) {
                        bestScore = score;
                        bestPiece = piece;
                        bestX = toX;
                        bestY = toY;
                        
                        qDebug() << "  【更新最佳移动】" << piece->name() << "到(" << toX << "," << toY << ") 得分:" << bestScore;
                    }
                }
            }
        }
    }
    
    qDebug() << "AI检查了" << inspectedMoves << "种移动，找到" << validMoves << "种有效移动";

    // 如果找不到有效移动，但有可用棋子，尝试随机移动一个棋子
    if (bestPiece == nullptr && !myPieces.isEmpty()) {
        qDebug() << "未找到有得分的移动，尝试随机移动...";
        
        // 随机选择一个棋子
        for (int attempts = 0; attempts < 30 && bestPiece == nullptr; attempts++) {
            ChessMan* randomPiece = myPieces[std::rand() % myPieces.size()];
            
            // 为该棋子找一个有效的移动
            QList<QPair<int, int>> validPositions;
            
            for (int toY = 0; toY < 10; ++toY) {
                for (int toX = 0; toX < 9; ++toX) {
                    // 如果目标位置有自己的棋子，跳过
                    if (board[toY][toX] && board[toY][toX]->color() == playerColor) {
                        continue;
                    }
                    
                    // 检查该棋子是否可以移动到目标位置
                    if (randomPiece->canMove(toX, toY, board)) {
                        validPositions.append(qMakePair(toX, toY));
                    }
                }
            }
            
            if (!validPositions.isEmpty()) {
                // 随机选择一个有效位置
                auto [toX, toY] = validPositions[std::rand() % validPositions.size()];
                bestPiece = randomPiece;
                bestX = toX;
                bestY = toY;
                qDebug() << "找到随机有效移动:" << randomPiece->name() << "到(" << toX << "," << toY << ")";
            }
        }
    }
    
    if (bestPiece) {
        qDebug() << "AI最终选择:" << bestPiece->name() << "，从(" << bestPiece->x() << "," << bestPiece->y() 
                 << ") 移动到 (" << bestX << "," << bestY << ")，得分:" << bestScore;
        return std::make_tuple(bestPiece, bestX, bestY);
    } else {
        qDebug() << "警告：AI无法找到有效移动!";
        return std::make_tuple(nullptr, -1, -1);
    }
}

int ChessAI::getScore(
    const QString& name)
{
    // 打印输入参数以帮助调试
    qDebug() << "AI评分棋子:" << name;

    // 检查一般的棋子类型（忽略数字）
    if (name.contains("King", Qt::CaseInsensitive)) {
        qDebug() << "  评分: 10000 (将/帅)";
        return 10000;
    }
    if (name.contains("Rook", Qt::CaseInsensitive)) {
        qDebug() << "  评分: 500 (车)";
        return 500;
    }
    if (name.contains("Horse", Qt::CaseInsensitive)) {
        qDebug() << "  评分: 300 (马)";
        return 300;
    }
    if (name.contains("Cannon", Qt::CaseInsensitive)) {
        qDebug() << "  评分: 250 (炮)";
        return 250;
    }
    if (name.contains("Elephant", Qt::CaseInsensitive)) {
        qDebug() << "  评分: 150 (象)";
        return 150;
    }
    if (name.contains("Advisor", Qt::CaseInsensitive)) {
        qDebug() << "  评分: 100 (士)";
        return 100;
    }
    if (name.contains("Soldier", Qt::CaseInsensitive)) {
        qDebug() << "  评分: 50 (卒)";
        return 50;
    }
    
    qDebug() << "  未知棋子类型，评分: 0";
    return 0;
}

