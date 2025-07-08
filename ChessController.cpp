#include "ChessController.h"

ChessController::ChessController(
    QObject* parent)
    : QObject(parent)
    , m_currentPlayer("红")
    , m_roundNumber(1)
    , m_gameOver(false)
    , m_winner("")
    , m_isCheck(false)
    , m_isCheckMate(false)
    , m_checkedPlayer("")
    , m_selfCheckMove(false)
    , m_isAiMode(false)
    , aiColor("黑")
{
    initializeGame();
}

void ChessController::initializeGame()
{
    // 清空棋盘
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            m_board[y][x] = nullptr;
        }
    }

    // 清空棋子列表
    qDeleteAll(m_pieces);
    m_pieces.clear();

    // 清空吃子记录
    m_capturedPiecesInfo.clear();

    // 重置游戏状态
    m_currentPlayer = "红";
    m_roundNumber = 1;
    m_gameOver = false;
    m_winner = "";
    m_isCheck = false;
    m_isCheckMate = false;
    m_checkedPlayer = "";

    // 初始化所有棋子
    m_pieces = ChessInitializer::initializePieces(m_board);

    // 发送信号通知UI更新
    emit chessDataChanged();
    emit currentPlayerChanged();
    emit roundNumberChanged();
    emit capturedPiecesChanged();
    emit gameOverChanged();
    emit winnerChanged();
    emit isCheckChanged();
    emit isCheckMateChanged();
}

QList<QObject*> ChessController::getRawPieces() const
{
    return m_pieces;
}

QString ChessController::currentPlayer() const
{
    return m_currentPlayer;
}

int ChessController::roundNumber() const
{
    return m_roundNumber;
}

bool ChessController::gameOver() const
{
    return m_gameOver;
}

QString ChessController::winner() const
{
    return m_winner;
}

bool ChessController::isCheck() const
{
    return m_isCheck;
}

bool ChessController::isCheckMate() const
{
    return m_isCheckMate;
}

QString ChessController::checkedPlayer() const
{
    return m_checkedPlayer;
}

bool ChessController::selfCheckMove() const
{
    return m_selfCheckMove;
}

QVariantList ChessController::getPieces() const
{
    QVariantList list;
    for (QObject* obj : m_pieces)
        list.append(QVariant::fromValue(obj));
    return list;
}

QVariantList ChessController::capturedPieces() const
{
    QVariantList result;
    for (const auto& info : m_capturedPiecesInfo) {
        QVariantMap item;
        item["name"] = info.name;
        item["color"] = info.color;
        item["icon"] = info.icon;
        item["capturedBy"] = info.capturedBy;
        item["round"] = info.round;
        result.append(item);
    }
    return result;
}

void ChessController::resetGame()
{
    m_gameOver = false;
    m_winner = "";
    m_currentPlayer = "红";
    m_roundNumber = 1;
    m_isCheck = false;
    m_isCheckMate = false;
    m_checkedPlayer = "";
    m_selfCheckMove = false;
    m_capturedPiecesInfo.clear();

    // 清空棋盘
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            m_board[y][x] = nullptr;
        }
    }

    // 重新初始化棋子
    m_pieces = ChessInitializer::initializePieces(m_board);

    // 发送信号通知UI更新
    emit gameOverChanged();
    emit winnerChanged();
    emit currentPlayerChanged();
    emit roundNumberChanged();
    emit isCheckChanged();
    emit isCheckMateChanged();
    emit checkedPlayerChanged();
    emit selfCheckMoveChanged();
    emit capturedPiecesChanged();
    emit chessDataChanged();
}

ChessMan* ChessController::getKing(
    const QString& color) const
{
    for (QObject* obj : m_pieces) {
        ChessMan* piece = qobject_cast<ChessMan*>(obj);
        if (piece && piece->name().contains("King") && piece->color() == color && piece->x() >= 0) {
            return piece;
        }
    }
    return nullptr;
}

bool ChessController::checkForCheck(const QString& color)
{
    static int checkDepth = 0;
    if (++checkDepth > 3) {
        checkDepth--;
        return false;
    }

    ChessMan* king = getKing(color);
    if (!king || king->x() < 0 || king->x() >= 9 || king->y() < 0 || king->y() >= 10) {
        checkDepth--;
        return false;
    }

    int kingX = king->x(), kingY = king->y();
    int pieceChecked = 0;
    const int maxChecks = 32;

    for (QObject* obj : m_pieces) {
        if (pieceChecked++ > maxChecks) break;
        
        ChessMan* piece = qobject_cast<ChessMan*>(obj);
        if (piece && piece->color() != color && piece->x() >= 0 && piece->y() >= 0 && piece->canMove(kingX, kingY, m_board)) {
            checkDepth--;
            return true;
        }
    }

    checkDepth--;
    return false;
}

bool ChessController::checkForCheckMate(const QString& color)
{
    static int mateCheckDepth = 0;
    if (++mateCheckDepth > 2) {
        mateCheckDepth--;
        return false;
    }

    if (!checkForCheck(color)) {
        mateCheckDepth--;
        return false;
    }

    int piecesChecked = 0;
    const int maxPiecesToCheck = 32;

    for (QObject* obj : m_pieces) {
        if (piecesChecked++ > maxPiecesToCheck) {
            mateCheckDepth--;
            return false;
        }

        ChessMan* piece = qobject_cast<ChessMan*>(obj);
        if (!piece || piece->color() != color || 
            piece->x() < 0 || piece->x() >= 9 || piece->y() < 0 || piece->y() >= 10) continue;

        int fromX = piece->x(), fromY = piece->y();
        int positionsChecked = 0;
        const int maxPositionsToCheck = 100;

        for (int toY = 0; toY < 10 && positionsChecked < maxPositionsToCheck; ++toY) {
            for (int toX = 0; toX < 9 && positionsChecked < maxPositionsToCheck; ++toX) {
                positionsChecked++;
                if (toX == fromX && toY == fromY) continue;
                if (abs(toX - fromX) + abs(toY - fromY) > 10) continue;

                try {
                    if (piece->canMove(toX, toY, m_board)) {
                        ChessMan* targetPiece = m_board[toY][toX];
                        
                        // 临时移动
                        m_board[fromY][fromX] = nullptr;
                        m_board[toY][toX] = piece;
                        int oldX = piece->x(), oldY = piece->y();
                        piece->setX(toX);
                        piece->setY(toY);

                        bool isCapture = targetPiece != nullptr;
                        int targetOldX = -99, targetOldY = -99;
                        if (isCapture) {
                            targetOldX = targetPiece->x();
                            targetOldY = targetPiece->y();
                            targetPiece->setX(-99);
                            targetPiece->setY(-99);
                        }

                        bool stillInCheck = checkForCheck(color);

                        // 恢复状态
                        piece->setX(oldX);
                        piece->setY(oldY);
                        m_board[fromY][fromX] = piece;
                        m_board[toY][toX] = targetPiece;
                        if (isCapture) {
                            targetPiece->setX(targetOldX);
                            targetPiece->setY(targetOldY);
                        }

                        if (!stillInCheck) {
                            mateCheckDepth--;
                            return false;
                        }
                    }
                } catch (...) {
                    continue;
                }
            }
        }
    }

    mateCheckDepth--;
    return true;
}

bool ChessController::canMoveResolveCheck(
    ChessMan* piece, int toX, int toY)
{
    if (!m_isCheck)
        return true; // 如果没有被将军，任何移动都可以

    int fromX = piece->x();
    int fromY = piece->y();

    // 暂存目标位置的棋子（如果有）
    ChessMan* targetPiece = m_board[toY][toX];

    m_board[toY][toX] = nullptr;

    // 暂时执行这个移动
    m_board[fromY][fromX] = nullptr;
    m_board[toY][toX] = piece;
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
    bool stillInCheck = checkForCheck(piece->color());

    // 恢复棋盘状态
    piece->setX(oldX);
    piece->setY(oldY);
    m_board[fromY][fromX] = piece;
    m_board[toY][toX] = targetPiece;

    // 如果是吃子，恢复被吃掉的棋子状态
    if (isCapture && targetPiece) {
        targetPiece->setX(targetOldX);
        targetPiece->setY(targetOldY);
    }

    if (isCapture && !stillInCheck) {
        return true; // 通过吃掉可以解除将军状态
    }

    // 如果这个移动可以解除将军，则返回true
    return !stillInCheck;
}

bool ChessController::isKingFacingKing() const
{
    ChessMan* redKing = getKing("红");
    ChessMan* blackKing = getKing("黑");

    if (!redKing || !blackKing)
        return false;

    // 必须在同一列
    if (redKing->x() != blackKing->x())
        return false;

    int col = redKing->x();
    int fromY = qMin(redKing->y(), blackKing->y()) + 1;
    int toY = qMax(redKing->y(), blackKing->y());

    // 检查中间是否有棋子
    for (int y = fromY; y < toY; ++y) {
        if (m_board[y][col] != nullptr) {
            return false; // 有阻挡
        }
    }

    return true; // 照面！
}

void ChessController::updateCheckStatus()
{
    // 首先检查是否有"将/帅"被吃掉（游戏应该已经结束）
    ChessMan* redKing = getKing("红");
    ChessMan* blackKing = getKing("黑");
    
    if (!redKing) {
        m_gameOver = true;
        m_winner = "黑";
        m_isCheck = false;
        m_isCheckMate = false;
        m_checkedPlayer = "";
        emit gameOverChanged();
        emit winnerChanged();
        emit isCheckChanged();
        emit isCheckMateChanged();
        emit checkedPlayerChanged();
        return;
    }
    
    if (!blackKing) {
        m_gameOver = true;
        m_winner = "红";
        m_isCheck = false;
        m_isCheckMate = false;
        m_checkedPlayer = "";
        emit gameOverChanged();
        emit winnerChanged();
        emit isCheckChanged();
        emit isCheckMateChanged();
        emit checkedPlayerChanged();
        return;
    }

    // 检查当前玩家是否被将军
    bool redInCheck = checkForCheck("红");
    bool blackInCheck = checkForCheck("黑");

    // 设置将军状态
    m_isCheck = redInCheck || blackInCheck;
    m_checkedPlayer = redInCheck ? "红" : (blackInCheck ? "黑" : "");

    if (m_isCheck) {
        QString checkedColor = m_checkedPlayer;
        m_isCheckMate = checkForCheckMate(checkedColor);
        if (m_isCheckMate) {
            m_gameOver = true;
            m_winner = (checkedColor == "红") ? "黑" : "红";
        }
    } else {
        m_isCheckMate = false;
        m_checkedPlayer = "";
    }

    emit isCheckChanged();
    emit isCheckMateChanged();
    emit checkedPlayerChanged();
}

void ChessController::capturePieceAt(int x, int y, ChessMan* capturingPiece)
{
    if (x < 0 || x >= 9 || y < 0 || y >= 10) return;

    ChessMan* targetPiece = m_board[y][x];
    if (!targetPiece || targetPiece->color() == capturingPiece->color()) return;

    // 记录被吃掉的棋子
    CapturePieceInfo capturedInfo;
    capturedInfo.name = targetPiece->name();
    capturedInfo.color = targetPiece->color();
    capturedInfo.icon = targetPiece->icon();
    capturedInfo.round = m_roundNumber;
    capturedInfo.capturedBy = capturingPiece->name();
    m_capturedPiecesInfo.append(capturedInfo);

    // 检查是否吃掉了将/帅
    if (targetPiece->name().contains("King")) {
        m_gameOver = true;
        m_winner = capturingPiece->color();
        emit gameOverChanged();
        emit winnerChanged();
    }

    // 更新棋盘数组
    m_board[y][x] = nullptr;
    targetPiece->setX(-99);
    targetPiece->setY(-99);

    emit capturedPiecesChanged();
}

void ChessController::handleMove(int fromIndex, int toX, int toY)
{
    if (m_gameOver) return;

    if (fromIndex < 0 || fromIndex >= m_pieces.size()) return;

    QObject* obj = m_pieces[fromIndex];
    ChessMan* piece = qobject_cast<ChessMan*>(obj);
    if (!piece) return;

    if (piece->color() != m_currentPlayer) return;

    int fromX = piece->x();
    int fromY = piece->y();

    if (fromX < 0 || fromX >= 9 || fromY < 0 || fromY >= 10) return;
    if (toX < 0 || toX >= 9 || toY < 0 || toY >= 10) return;

    if (m_board[fromY][fromX] != piece) return;

    ChessMan* targetPiece = m_board[toY][toX];
    bool canMove = piece->canMove(toX, toY, m_board);
    if (!canMove) return;

    // 检查是否能解除将军
    if (m_isCheck && m_checkedPlayer == piece->color()) {
        if (!canMoveResolveCheck(piece, toX, toY)) {
            m_selfCheckMove = true;
            emit selfCheckMoveChanged();
            QTimer::singleShot(1000, this, [this]() {
                m_selfCheckMove = false;
                emit selfCheckMoveChanged();
            });
            return;
        }
    }

    // 检查移动后是否会自己被将军
    m_board[fromY][fromX] = nullptr;
    m_board[toY][toX] = piece;
    int oldX = piece->x(), oldY = piece->y();
    piece->setX(toX);
    piece->setY(toY);

    bool isCapture = targetPiece != nullptr;
    int targetOldX = -99, targetOldY = -99;
    if (isCapture) {
        targetOldX = targetPiece->x();
        targetOldY = targetPiece->y();
        targetPiece->setX(-99);
        targetPiece->setY(-99);
    }

    bool wouldBeInCheck = checkForCheck(piece->color());

    // 恢复状态
    piece->setX(oldX);
    piece->setY(oldY);
    m_board[fromY][fromX] = piece;
    m_board[toY][toX] = targetPiece;
    if (isCapture) {
        targetPiece->setX(targetOldX);
        targetPiece->setY(targetOldY);
    }

    if (wouldBeInCheck) {
        m_selfCheckMove = true;
        emit selfCheckMoveChanged();
        QTimer::singleShot(1000, this, [this]() {
            m_selfCheckMove = false;
            emit selfCheckMoveChanged();
        });
        return;
    }

    // 检查将帅照面
    if (piece->name().contains("King")) {
        ChessMan* enemyKing = getKing(piece->color() == "红" ? "黑" : "红");
        if (enemyKing && piece->x() == enemyKing->x()) {
            bool blocked = false;
            int minY = qMin(piece->y(), enemyKing->y());
            int maxY = qMax(piece->y(), enemyKing->y());
            for (int y = minY + 1; y < maxY; ++y) {
                if (m_board[y][piece->x()]) {
                    blocked = true;
                    break;
                }
            }
            if (!blocked) return;
        }
    }

    // 执行移动
    if (targetPiece) {
        CapturePieceInfo captureInfo;
        captureInfo.name = targetPiece->name();
        captureInfo.color = targetPiece->color();
        captureInfo.icon = targetPiece->icon();
        captureInfo.capturedBy = piece->name();
        captureInfo.round = m_roundNumber;
        m_capturedPiecesInfo.append(captureInfo);
        emit capturedPiecesChanged();

        targetPiece->setX(-99);
        targetPiece->setY(-99);

        // 检查游戏结束
        if (targetPiece->name().contains("King")) {
            m_gameOver = true;
            m_winner = piece->color();
            emit gameOverChanged();
            emit winnerChanged();
        }
    }

    // 更新棋盘
    m_board[fromY][fromX] = nullptr;
    piece->setX(toX);
    piece->setY(toY);
    m_board[toY][toX] = piece;

    // 切换玩家
    m_currentPlayer = (m_currentPlayer == "红") ? "黑" : "红";
    emit currentPlayerChanged();

    m_roundNumber++;
    emit roundNumberChanged();

    // 检查将军状态
    QString opponentColor = (piece->color() == "红") ? "黑" : "红";
    bool opponentInCheck = checkForCheck(opponentColor);
    
    if (opponentInCheck) {
        m_isCheck = true;
        m_checkedPlayer = opponentColor;
        bool opponentInCheckMate = checkForCheckMate(opponentColor);
        if (opponentInCheckMate) {
            m_isCheckMate = true;
            m_gameOver = true;
            m_winner = piece->color();
            emit gameOverChanged();
            emit winnerChanged();
        }
    } else {
        m_isCheck = false;
        m_isCheckMate = false;
        m_checkedPlayer = "";
    }

    emit isCheckChanged();
    emit isCheckMateChanged();
    emit checkedPlayerChanged();
    emit chessDataChanged();

    // AI回合
    if (m_isAiMode && m_currentPlayer == aiColor && !m_gameOver) {
        QTimer::singleShot(500, this, [this]() {
            if (m_currentPlayer != aiColor) return;

            ChessAI ai;
            auto [selectedPiece, targetX, targetY] = ai.selectBestMove(m_board, aiColor);

            if (selectedPiece && targetX >= 0 && targetX < 9 && targetY >= 0 && targetY < 10 &&
                selectedPiece->color() == aiColor && selectedPiece->canMove(targetX, targetY, m_board)) {
                
                int pieceIndex = -1;
                for (int i = 0; i < m_pieces.size(); ++i) {
                    if (m_pieces[i] == selectedPiece) {
                        pieceIndex = i;
                        break;
                    }
                }

                if (pieceIndex >= 0) {
                    handleMove(pieceIndex, targetX, targetY);
                }
            } else {
                // AI认输
                m_gameOver = true;
                m_winner = (aiColor == "红") ? "黑" : "红";
                emit gameOverChanged();
                emit winnerChanged();
            }
        });
    }
}

void ChessController::toggleAIMode()
{
    m_isAiMode = !m_isAiMode;
    resetGame();
    emit aiModeChanged();
}

void ChessController::switchTurn()
{
    m_currentPlayer = (m_currentPlayer == "红") ? "黑" : "红";
    emit currentPlayerChanged();

    if (m_isAiMode && m_currentPlayer == aiColor) {
        QTimer::singleShot(500, this, [this]() {
            if (m_currentPlayer != aiColor) return;

            ChessAI ai;
            auto [selectedPiece, targetX, targetY] = ai.selectBestMove(m_board, aiColor);

            if (selectedPiece && targetX >= 0 && targetX < 9 && targetY >= 0 && targetY < 10 &&
                selectedPiece->color() == aiColor && selectedPiece->canMove(targetX, targetY, m_board)) {
                
                int pieceIndex = -1;
                for (int i = 0; i < m_pieces.size(); ++i) {
                    if (m_pieces[i] == selectedPiece) {
                        pieceIndex = i;
                        break;
                    }
                }

                if (pieceIndex >= 0) {
                    handleMove(pieceIndex, targetX, targetY);
                } else {
                    // 尝试通过坐标查找
                    int fromX = selectedPiece->x(), fromY = selectedPiece->y();
                    ChessMan* boardPiece = m_board[fromY][fromX];
                    if (boardPiece == selectedPiece) {
                        for (int i = 0; i < m_pieces.size(); ++i) {
                            ChessMan* p = qobject_cast<ChessMan*>(m_pieces[i]);
                            if (p && p->x() == fromX && p->y() == fromY && p->color() == aiColor) {
                                pieceIndex = i;
                                break;
                            }
                        }
                        if (pieceIndex >= 0) {
                            handleMove(pieceIndex, targetX, targetY);
                        }
                    }
                }
            } else {
                // AI认输
                m_gameOver = true;
                m_winner = (aiColor == "红") ? "黑" : "红";
                emit gameOverChanged();
                emit winnerChanged();
            }
        });
    }
}

bool ChessController::isAiMode() const
{
    return m_isAiMode;
}
