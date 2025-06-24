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

    // 调试输出初始棋盘状态
    qDebug() << "初始棋盘状态:";
    for (int y = 0; y < 10; ++y) {
        QString line;
        for (int x = 0; x < 9; ++x) {
            if (m_board[y][x]) {
                line += m_board[y][x]->name().left(2) + " ";
            } else {
                line += ".. ";
            }
        }
        qDebug() << line;
    }

    // 验证棋盘数组正确性
    qDebug() << "验证棋盘数组与棋子列表一致性:";
    bool consistent = true;
    for (QObject* obj : m_pieces) {
        ChessMan* piece = qobject_cast<ChessMan*>(obj);
        if (piece && piece->x() >= 0 && piece->y() >= 0) {
            if (m_board[piece->y()][piece->x()] != piece) {
                qDebug() << "不一致: " << piece->name() << " at (" << piece->x() << ","
                         << piece->y() << ")";
                consistent = false;
            }
        }
    }
    qDebug() << "棋盘一致性检查:" << (consistent ? "通过" : "失败");

    qDebug() << "总棋子数量:" << m_pieces.size();

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
    return m_capturedPiecesInfo;
}

void ChessController::resetGame()
{
    qDebug() << "重置游戏";
    initializeGame();
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

bool ChessController::checkForCheck(
    const QString& color)
{
    // 检查是否正在递归调用，防止无限嵌套
    static int checkDepth = 0;
    checkDepth++;

    // 设置最大递归深度
    if (checkDepth > 3) {
        qDebug() << "checkForCheck 递归深度过大，可能存在无限递归，强制返回false";
        checkDepth--;
        return false;
    }

    ChessMan* king = getKing(color);
    if (!king) {
        qDebug() << "未找到" << color << "方将/帅，假设不处于将军状态";
        checkDepth--;
        return false; // 如果找不到将/帅，则不处于将军状态
    }

    int kingX = king->x();
    int kingY = king->y();

    // 检查将/帅的坐标是否合法
    if (kingX < 0 || kingX >= 9 || kingY < 0 || kingY >= 10) {
        qDebug() << "将/帅坐标非法:" << kingX << kingY << "，假设不处于将军状态";
        checkDepth--;
        return false;
    }

    // 检查是否有任何对方棋子可以吃掉将/帅
    int pieceChecked = 0;
    const int maxChecks = 32;        // 最大检查次数，避免无限循环
    QList<ChessMan*> checkingPieces; // 记录能将军的棋子

    for (QObject* obj : m_pieces) {
        // 安全检查，避免过多循环
        if (pieceChecked++ > maxChecks) {
            qDebug() << "检查将军时棋子数量过多，可能有问题，中断检查";
            break;
        }

        ChessMan* piece = qobject_cast<ChessMan*>(obj);
        if (!piece)
            continue;

        // 确保只检查在棋盘上的敌方棋子
        if (piece->color() != color && piece->x() >= 0 && piece->x() < 9 && piece->y() >= 0
            && piece->y() < 10 && m_board[piece->y()][piece->x()] == piece) {
            // 尝试移动前记录日志
            qDebug() << "检查棋子是否能将军:" << piece->name() << "位置:(" << piece->x() << ","
                     << piece->y() << ")";

            // 检查是否可以移动到将/帅位置
            bool canAttack = false;
            try {
                canAttack = piece->canMove(kingX, kingY, m_board);
            } catch (const std::exception& e) {
                qDebug() << "检查将军时出现C++异常!" << e.what();
                continue; // 如果出现异常，跳过此棋子
            } catch (...) {
                qDebug() << "检查将军时出现未知异常!";
                continue; // 如果出现异常，跳过此棋子
            }

            if (canAttack) {
                qDebug() << "将军! " << piece->name() << "可以吃掉" << king->name();
                checkingPieces.append(piece); // 添加到将军棋子列表
            }
        }
    }

    // 如果有棋子能将军
    if (!checkingPieces.isEmpty()) {
        for (auto* piece : checkingPieces) {
            qDebug() << "将军棋子:" << piece->name() << "位置:(" << piece->x() << "," << piece->y()
                     << ")";
        }
        checkDepth--;
        return true;
    }

    checkDepth--;
    return false;
}

bool ChessController::checkForCheckMate(
    const QString& color)
{
    // 检查是否正在递归调用，防止无限嵌套
    static int mateCheckDepth = 0;
    mateCheckDepth++;

    // 设置最大递归深度
    if (mateCheckDepth > 2) {
        qDebug() << "checkForCheckMate 递归深度过大，可能存在无限递归，强制返回false";
        mateCheckDepth--;
        return false;
    }

    // 如果没有被将军，肯定没有被将死
    if (!checkForCheck(color)) {
        mateCheckDepth--;
        return false;
    }

    // 安全计数器，防止检查过多的棋子
    int piecesChecked = 0;
    const int maxPiecesToCheck = 32; // 设置最大检查棋子数量

    // 尝试该方所有棋子的所有可能移动，看是否能解除将军状态
    for (QObject* obj : m_pieces) {
        // 安全检查，避免检查过多棋子
        if (piecesChecked++ > maxPiecesToCheck) {
            qDebug() << "检查将死时棋子数量过多，可能有问题，中断检查并假设未将死";
            mateCheckDepth--;
            return false;
        }

        ChessMan* piece = qobject_cast<ChessMan*>(obj);
        // 确保只检查在棋盘上的本方棋子
        if (!piece || piece->color() != color || piece->x() < 0 || piece->x() >= 9 || piece->y() < 0
            || piece->y() >= 10)
            continue;

        int fromX = piece->x();
        int fromY = piece->y();

        qDebug() << "检查棋子是否可以解除将军:" << piece->name() << "位置:(" << fromX << ","
                 << fromY << ")";

        // 安全计数器，限制检查的位置数量
        int positionsChecked = 0;
        const int maxPositionsToCheck = 100; // 设置最大检查位置数量

        // 尝试所有可能的目标位置
        for (int toY = 0; toY < 10 && positionsChecked < maxPositionsToCheck; ++toY) {
            for (int toX = 0; toX < 9 && positionsChecked < maxPositionsToCheck; ++toX) {
                positionsChecked++;

                // 跳过目标坐标就是起始坐标的情况
                if (toX == fromX && toY == fromY)
                    continue;

                // 跳过明显不合法的移动（距离过远），减少不必要的计算
                int moveDistance = abs(toX - fromX) + abs(toY - fromY);
                if (moveDistance > 10)
                    continue; // 象棋中正常移动距离不会太大

                try {
                    // 如果这个移动是合法的
                    bool canMove = false;
                    canMove = piece->canMove(toX, toY, m_board);

                    if (canMove) {
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

                        // 如果是吃子，暂时从棋子列表中移除目标棋子
                        int targetPieceIndex = -1;
                        bool isCapture = targetPiece != nullptr;
                        int targetOldX = -99;
                        int targetOldY = -99;

                        if (isCapture) {
                            qDebug()
                                << "临时移动中，暂时移除被吃掉的棋子:" << targetPiece->name()
                                << "位置:(" << targetPiece->x() << "," << targetPiece->y() << ")";
                            // 保存原始坐标
                            targetOldX = targetPiece->x();
                            targetOldY = targetPiece->y();
                            // 临时设置为棋盘外位置
                            targetPiece->setX(-99);
                            targetPiece->setY(-99);

                            // 记录被吃掉棋子在列表中的索引（如果需要的话）
                            targetPieceIndex = m_pieces.indexOf(targetPiece);
                        }

                        // 检查移动后是否仍然被将军
                        bool stillInCheck = false;
                        try {
                            stillInCheck = checkForCheck(color);
                            if (!stillInCheck && isCapture) {
                                qDebug() << "通过吃掉" << targetPiece->name() << "解除了将军状态";
                            }
                        } catch (const std::exception& e) {
                            qDebug() << "检查将军状态时出现C++异常!" << e.what();
                            stillInCheck = true; // 保守处理，假设仍然被将军
                        } catch (...) {
                            qDebug() << "检查将军状态时出现未知异常!";
                            stillInCheck = true; // 保守处理，假设仍然被将军
                        }

                        // 恢复棋盘状态
                        piece->setX(oldX);
                        piece->setY(oldY);
                        m_board[fromY][fromX] = piece;
                        m_board[toY][toX] = targetPiece;

                        // 如果是吃子，恢复被吃掉的棋子状态
                        if (isCapture && targetPiece) {
                            qDebug() << "恢复被临时移除的棋子状态:" << targetPiece->name();
                            targetPiece->setX(targetOldX);
                            targetPiece->setY(targetOldY);
                        }

                        // 如果这个移动可以解除将军，则没有被将死
                        if (!stillInCheck) {
                            qDebug() << "可以通过移动 " << piece->name() << " 到 (" << toX << ","
                                     << toY << ") 解除将军";
                            mateCheckDepth--;
                            return false;
                        }
                    }
                } catch (const std::exception& e) {
                    qDebug() << "检查移动时出现C++异常:" << piece->name() << "从(" << fromX << ","
                             << fromY << ")到(" << toX << "," << toY << ")"
                             << " - " << e.what();
                    continue; // 如果出现异常，跳过这个位置
                } catch (...) {
                    qDebug() << "检查移动时出现未知异常:" << piece->name() << "从(" << fromX << ","
                             << fromY << ")到(" << toX << "," << toY << ")";
                    continue; // 如果出现异常，跳过这个位置
                }
            }
        }

        // 如果该棋子的位置检查过多，记录日志
        if (positionsChecked >= maxPositionsToCheck) {
            qDebug() << "棋子" << piece->name() << "的可能位置检查数量过多，部分位置未检查";
        }
    }

    // 如果尝试了所有可能的移动都无法解除将军，则被将死
    qDebug() << "将死! " << color << "方无法解除将军";
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
        qDebug() << "解除将军检查：暂时移除被吃掉的棋子:" << targetPiece->name();
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
        qDebug() << "解除将军检查：恢复被临时移除的棋子状态:" << targetPiece->name();
        targetPiece->setX(targetOldX);
        targetPiece->setY(targetOldY);
    }

    if (isCapture && !stillInCheck) {
        qDebug() << "通过吃掉" << targetPiece->name() << "可以解除将军状态";
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
            qDebug() << "绝杀! 获胜方:" << m_winner;
            emit gameOverChanged();
            emit winnerChanged();
        }
    } else {
        m_isCheckMate = false;
        m_checkedPlayer = "";
    }

    emit isCheckChanged();
    emit isCheckMateChanged();
    emit checkedPlayerChanged();
}

void ChessController::capturePieceAt(
    int x, int y, ChessMan* capturingPiece)
{
    if (x < 0 || x >= 9 || y < 0 || y >= 10) {
        qDebug() << "尝试吃掉棋子：坐标超出棋盘范围" << x << y;
        return;
    }

    ChessMan* targetPiece = m_board[y][x];
    if (!targetPiece) {
        qDebug() << "目标位置没有棋子，无法吃子" << x << y;
        return;
    }

    if (targetPiece->color() == capturingPiece->color()) {
        qDebug() << "目标位置是己方棋子，无法吃子";
        return;
    }

    qDebug() << "执行直接吃子：" << capturingPiece->name() << "吃" << targetPiece->name();

    // 记录被吃掉的棋子
    QVariantMap capturedInfo;
    capturedInfo["name"] = targetPiece->name();
    capturedInfo["color"] = targetPiece->color();
    capturedInfo["icon"] = targetPiece->icon();
    capturedInfo["round"] = m_roundNumber;
    capturedInfo["capturedBy"] = capturingPiece->name();
    m_capturedPiecesInfo.append(capturedInfo);

    // 将被吃掉的棋子移出棋盘
    qDebug() << "将被吃棋子移出棋盘: 原位置" << targetPiece->x() << "," << targetPiece->y();

    // 检查是否吃掉了将/帅
    bool isKing = targetPiece->name().contains("King");
    if (isKing) {
        m_gameOver = true;
        m_winner = capturingPiece->color();
        qDebug() << "游戏结束! 获胜方:" << m_winner;
        emit gameOverChanged();
        emit winnerChanged();
    }

    // 更新棋盘数组
    m_board[y][x] = nullptr;

    // 明确设置棋子位置到棋盘外
    targetPiece->setX(-99);
    targetPiece->setY(-99);

    emit capturedPiecesChanged();
}

void ChessController::handleMove(
    int fromIndex, int toX, int toY)
{
    // 如果游戏已结束，不允许移动
    if (m_gameOver) {
        qDebug() << "游戏已结束，不能移动棋子";
        return;
    }

    qDebug() << "\n======开始处理移动======";
    qDebug() << "移动索引:" << fromIndex << "到位置:" << toX << toY;

    // 基本检查
    if (fromIndex < 0 || fromIndex >= m_pieces.size()) {
        qDebug() << "无效的棋子索引:" << fromIndex;
        return;
    }

    // 获取选中的棋子
    ChessMan* piece = qobject_cast<ChessMan*>(m_pieces[fromIndex]);
    if (!piece) {
        qDebug() << "无效的棋子对象";
        return;
    }

    // 检查是否是当前玩家的棋子
    if (piece->color() != m_currentPlayer) {
        qDebug() << "不是当前玩家的棋子，当前玩家:" << m_currentPlayer;
        return;
    }

    // 获取起始位置
    int fromX = piece->x();
    int fromY = piece->y();

    // 检查起始位置是否合法
    if (fromX < 0 || fromX >= 9 || fromY < 0 || fromY >= 10) {
        qDebug() << "起始位置无效:" << fromX << fromY;
        return;
    }

    // 检查目标位置是否合法
    if (toX < 0 || toX >= 9 || toY < 0 || toY >= 10) {
        qDebug() << "目标位置无效:" << toX << toY;
        return;
    }

    qDebug() << "移动棋子:" << piece->name() << "颜色:" << piece->color();
    qDebug() << "从位置:" << fromX << fromY << "到位置:" << toX << toY;

    // 验证棋盘上的棋子与当前选中棋子一致
    if (m_board[fromY][fromX] != piece) {
        qDebug() << "警告: 棋盘上选中位置的棋子与选中棋子不一致!";
        qDebug() << "棋盘上:" << (m_board[fromY][fromX] ? m_board[fromY][fromX]->name() : "空");
        qDebug() << "选中的:" << piece->name();
    }

    // 检查目标位置是否有棋子
    ChessMan* targetPiece = m_board[toY][toX];
    if (targetPiece) {
        qDebug() << "目标位置有棋子:" << targetPiece->name() << "颜色:" << targetPiece->color();
    } else {
        qDebug() << "目标位置没有棋子";
    }

    // 检查是否可以移动
    bool canMove = piece->canMove(toX, toY, m_board);
    qDebug() << "检查是否可以移动:" << (canMove ? "可以" : "不可以");

    if (!canMove) {
        qDebug() << "无法移动到目标位置";
        return;
    }

    // 如果当前处于将军状态，检查这个移动是否能解除将军
    if (m_isCheck && !canMoveResolveCheck(piece, toX, toY)) {
        qDebug() << "当前处于将军状态，这个移动无法解除将军，不允许移动";
        return;
    }

    // 检查这个移动是否会导致自己被将军

    // 暂时执行这个移动
    m_board[fromY][fromX] = nullptr;
    m_board[toY][toX] = piece;
    int oldX = piece->x();
    int oldY = piece->y();
    piece->setX(toX);
    piece->setY(toY);

    // 检查移动后是否会导致自己被将军
    bool willBeInCheck = checkForCheck(piece->color());

    // 恢复棋盘状态
    piece->setX(oldX);
    piece->setY(oldY);
    m_board[fromY][fromX] = piece;
    m_board[toY][toX] = targetPiece;

    if (willBeInCheck) {
        qDebug() << "这个移动会导致自己被将军，不允许移动";
        m_selfCheckMove = true;
        emit selfCheckMoveChanged();

        // 1秒后自动重置提示状态
        QTimer::singleShot(1000, this, [this]() {
            m_selfCheckMove = false;
            emit selfCheckMoveChanged();
        });
        return;
    }
    // 临时执行这个移动，检查将帅是否照面
    m_board[fromY][fromX] = nullptr;
    ChessMan* tempTarget = m_board[toY][toX];
    m_board[toY][toX] = piece;
    piece->setX(toX);
    piece->setY(toY);

    bool kingFacing = isKingFacingKing();

    // 恢复棋盘
    piece->setX(oldX);
    piece->setY(oldY);
    m_board[fromY][fromX] = piece;
    m_board[toY][toX] = tempTarget;

    if (kingFacing) {
        qDebug() << "将帅照面，不允许移动";
        m_selfCheckMove = true;
        emit selfCheckMoveChanged();
        QTimer::singleShot(1000, this, [this]() {
            m_selfCheckMove = false;
            emit selfCheckMoveChanged();
        });
        return;
    }

    // 如果目标位置有对方棋子，则吃掉它
    if (targetPiece && targetPiece->color() != piece->color()) {
        capturePieceAt(toX, toY, piece);
    }

    // 如果游戏已经结束，不需要继续处理
    if (m_gameOver) {
        // 更新棋盘数组
        qDebug() << "清除起始位置:" << fromX << "," << fromY;
        m_board[fromY][fromX] = nullptr;

        // 更新棋子位置
        qDebug() << "更新棋子位置:" << piece->name() << "到" << toX << "," << toY;
        piece->setX(toX);
        piece->setY(toY);

        // 设置目标位置
        qDebug() << "设置目标位置:" << toX << "," << toY << "为棋子" << piece->name();
        m_board[toY][toX] = piece;

        // 通知视图更新
        emit chessDataChanged();
        return;
    }

    // 更新棋盘数组
    qDebug() << "更新棋盘数组:";
    qDebug() << "清除起始位置:" << fromX << "," << fromY;
    m_board[fromY][fromX] = nullptr;

    // 更新棋子位置
    qDebug() << "更新棋子位置:" << piece->name() << "到" << toX << "," << toY;
    piece->setX(toX);
    piece->setY(toY);

    // 设置目标位置
    qDebug() << "设置目标位置:" << toX << "," << toY << "为棋子" << piece->name();
    m_board[toY][toX] = piece;

    // 切换当前玩家
    m_currentPlayer = (m_currentPlayer == "红") ? "黑" : "红";
    qDebug() << "切换当前玩家到:" << m_currentPlayer;
    emit currentPlayerChanged();

    // 更新回合数
    if (m_currentPlayer == "红") {
        m_roundNumber++;
        qDebug() << "回合数增加到:" << m_roundNumber;
        emit roundNumberChanged();
    }

    // 检查是否将军或将死
    updateCheckStatus();

    // 通知视图更新
    qDebug() << "发送chessDataChanged信号";
    emit chessDataChanged();

    // 输出当前棋盘状态
    qDebug() << "移动后棋盘状态:";
    for (int y = 0; y < 10; ++y) {
        QString line;
        for (int x = 0; x < 9; ++x) {
            if (m_board[y][x]) {
                line += m_board[y][x]->name().left(2) + " ";
            } else {
                line += ".. ";
            }
        }
        qDebug() << line;
    }

    // 再次验证棋盘数组正确性
    qDebug() << "验证棋盘数组与棋子列表一致性:";
    bool consistent = true;
    for (QObject* obj : m_pieces) {
        ChessMan* p = qobject_cast<ChessMan*>(obj);
        if (p && p->x() >= 0 && p->y() >= 0) {
            if (m_board[p->y()][p->x()] != p) {
                qDebug() << "不一致: " << p->name() << " at (" << p->x() << "," << p->y() << ")";
                consistent = false;
            }
        }
    }
    qDebug() << "棋盘一致性检查:" << (consistent ? "通过" : "失败");

    // 检查是否有吃子记录
    qDebug() << "当前吃子记录数量:" << m_capturedPiecesInfo.size();

    qDebug() << "======移动处理完成======\n";
}

void ChessController::toggleAIMode()
{
    isAiMode = !isAiMode;
    m_currentPlayer = "红"; // 重新设置红方为先手
    emit currentPlayerChanged();

    qDebug() << "切换为" << (isAiMode ? "人机模式" : "双人模式");

    // 如果是AI模式且是AI回合，触发AI走棋
    if (isAiMode && m_currentPlayer == "黑") {
        switchTurn(); // AI 回合，直接让 AI 落子
    }
}

void ChessController::switchTurn()
{
    // 切换当前玩家
    m_currentPlayer = (m_currentPlayer == "红") ? "黑" : "红";
    qDebug() << "切换当前玩家到:" << m_currentPlayer;
    emit currentPlayerChanged();

    // 如果是 AI 模式并且当前是 AI 的回合，让 AI 自动落子
    if (isAiMode && m_currentPlayer == "黑") {
        QTimer::singleShot(500, this, [=]() {
            auto [piece, toX, toY] = ai.selectBestMove(m_board, "黑"); // 假设 AI 执黑

            if (piece) {
                qDebug() << "AI选择了:" << piece->name() << " 从 (" << piece->x() << ","
                         << piece->y() << ") 移动到 (" << toX << "," << toY << ")";

                if (piece->moveTo(toX, toY, m_board)) {
                    switchTurn(); // AI 完成移动后切换回合
                } else {
                    qDebug() << "AI移动失败（非法）";
                }
            } else {
                qDebug() << "AI无法找到可走的棋子";
            }
        });
    }
}
