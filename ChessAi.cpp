#include "ChessAi.h"

std::tuple<ChessMan*, int, int> ChessAI::selectBestMove(
    ChessMan* board[10][9], const QString& aiColor)
{
    auto moves = generateAllMoves(board, aiColor);
    int bestScore = -999999;
    std::tuple<ChessMan*, int, int, int> bestMove;

    for (auto& move : moves) {
        ChessMan* piece = std::get<0>(move);
        int fromX = piece->x(), fromY = piece->y();
        int toX = std::get<1>(move), toY = std::get<2>(move);

        ChessMan* target = board[toX][toY];
        board[toX][toY] = piece;
        board[fromX][fromY] = nullptr;
        piece->setX(toX);
        piece->setY(toY);

        int score = evaluate(board, aiColor);

        // 回溯
        piece->setX(fromX);
        piece->setY(fromY);
        board[fromX][fromY] = piece;
        board[toX][toY] = target;

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return {std::get<0>(bestMove), std::get<1>(bestMove), std::get<2>(bestMove)};
}

std::vector<std::tuple<ChessMan*, int, int, int>> ChessAI::generateAllMoves(
    ChessMan* board[10][9], const QString& color)
{
    std::vector<std::tuple<ChessMan*, int, int, int>> moves;
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 9; ++y) {
            ChessMan* piece = board[x][y];
            if (piece && piece->color() == color) {
                for (int tx = 0; tx < 10; ++tx) {
                    for (int ty = 0; ty < 9; ++ty) {
                        if (piece->canMove(tx, ty, board)) {
                            ChessMan* target = board[tx][ty];
                            int score = target ? getScore(target->name()) : 0;
                            moves.emplace_back(piece, tx, ty, score);
                        }
                    }
                }
            }
        }
    }
    return moves;
}

int ChessAI::evaluate(
    ChessMan* board[10][9], const QString& aiColor)
{
    int score = 0;
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 9; ++y) {
            ChessMan* piece = board[x][y];
            if (piece) {
                int val = getScore(piece->name());
                if (piece->color() == aiColor)
                    score += val;
                else
                    score -= val;
            }
        }
    }
    return score;
}

int ChessAI::getScore(
    const QString& name)
{
    if (name.contains("帅") || name.contains("将"))
        return 10000;
    if (name.contains("车"))
        return 500;
    if (name.contains("马"))
        return 300;
    if (name.contains("炮"))
        return 250;
    if (name.contains("相") || name.contains("象"))
        return 150;
    if (name.contains("士") || name.contains("仕"))
        return 100;
    if (name.contains("兵") || name.contains("卒"))
        return 50;
    return 0;
}
