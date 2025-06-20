#pragma once
#include <QObject>
#include <QList>
#include <QVariantList>
#include <QVector>
#include <QString>
#include <QDebug>
#include "ChessMan.h"
#include "ChessInitializer.h"

class ChessController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentPlayer READ currentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(int roundNumber READ roundNumber NOTIFY roundNumberChanged)
    Q_PROPERTY(QVariantList capturedPieces READ capturedPieces NOTIFY capturedPiecesChanged)

public:
    explicit ChessController(
        QObject* parent = nullptr)
        : QObject(parent),
        m_currentPlayer("红"),
        m_roundNumber(1)
    {
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
                    qDebug() << "不一致: " << piece->name() 
                             << " at (" << piece->x() << "," << piece->y() << ")";
                    consistent = false;
                }
            }
        }
        qDebug() << "棋盘一致性检查:" << (consistent ? "通过" : "失败");
        
        qDebug() << "总棋子数量:" << m_pieces.size();
    }

    QList<QObject*> getRawPieces() const { return m_pieces; }
    
    QString currentPlayer() const { return m_currentPlayer; }
    int roundNumber() const { return m_roundNumber; }

    Q_INVOKABLE QVariantList getPieces() const
    {
        QVariantList list;
        for (QObject* obj : m_pieces)
            list.append(QVariant::fromValue(obj));
        return list;
    }
    
    Q_INVOKABLE QVariantList capturedPieces() const
    {
        return m_capturedPiecesInfo;
    }
    
    // 直接吃掉指定位置的棋子
    void capturePieceAt(int x, int y, ChessMan* capturingPiece) {
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
        
        // 更新棋盘数组
        m_board[y][x] = nullptr;
        
        // 明确设置棋子位置到棋盘外
        targetPiece->setX(-99);
        targetPiece->setY(-99);
        
        emit capturedPiecesChanged();
    }

    Q_INVOKABLE void handleMove(int fromIndex, int toX, int toY)
    {
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
        
        // 如果目标位置有对方棋子，则吃掉它
        if (targetPiece && targetPiece->color() != piece->color()) {
            capturePieceAt(toX, toY, piece);
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
                    qDebug() << "不一致: " << p->name() 
                             << " at (" << p->x() << "," << p->y() << ")";
                    consistent = false;
                }
            }
        }
        qDebug() << "棋盘一致性检查:" << (consistent ? "通过" : "失败");
        
        // 检查是否有吃子记录
        qDebug() << "当前吃子记录数量:" << m_capturedPiecesInfo.size();
        
        qDebug() << "======移动处理完成======\n";
    }

signals:
    void chessDataChanged();
    void currentPlayerChanged();
    void roundNumberChanged();
    void capturedPiecesChanged();

private:
    ChessMan* m_board[10][9];
    QList<QObject*> m_pieces;
    QString m_currentPlayer;
    int m_roundNumber;
    QVariantList m_capturedPiecesInfo; // 存储被吃掉的棋子信息
};

