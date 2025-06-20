#pragma once
#include <QObject>
#include <QList>
#include <QVariantList>
#include <QVector>
#include <QString>
#include <QDebug>
#include <QTimer>
#include "ChessMan.h"
#include "ChessInitializer.h"

class ChessController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentPlayer READ currentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(int roundNumber READ roundNumber NOTIFY roundNumberChanged)
    Q_PROPERTY(QVariantList capturedPieces READ capturedPieces NOTIFY capturedPiecesChanged)
    Q_PROPERTY(bool gameOver READ gameOver NOTIFY gameOverChanged)
    Q_PROPERTY(QString winner READ winner NOTIFY winnerChanged)
    Q_PROPERTY(bool isCheck READ isCheck NOTIFY isCheckChanged)
    Q_PROPERTY(bool isCheckMate READ isCheckMate NOTIFY isCheckMateChanged)
    Q_PROPERTY(QString checkedPlayer READ checkedPlayer NOTIFY checkedPlayerChanged)
    Q_PROPERTY(bool selfCheckMove READ selfCheckMove NOTIFY selfCheckMoveChanged)

public:
    explicit ChessController(QObject* parent = nullptr);

    void initializeGame();

    QList<QObject*> getRawPieces() const;
    
    QString currentPlayer() const;
    int roundNumber() const;
    bool gameOver() const;
    QString winner() const;
    bool isCheck() const;
    bool isCheckMate() const;
    QString checkedPlayer() const;
    bool selfCheckMove() const;

    Q_INVOKABLE QVariantList getPieces() const;
    
    Q_INVOKABLE QVariantList capturedPieces() const;
    
    // 重置游戏
    Q_INVOKABLE void resetGame();
    
    // 获取指定颜色的将/帅
    ChessMan* getKing(const QString& color) const;

    // 检查指定颜色是否被将军
    bool checkForCheck(const QString& color);
    
    // 检查指定颜色是否被将死
    bool checkForCheckMate(const QString& color);
    
    // 检查当前移动是否能解除将军状态
    bool canMoveResolveCheck(ChessMan* piece, int toX, int toY);

    //照面规则
    bool isKingFacingKing() const;

    // 更新将军和将死状态
    void updateCheckStatus();

    // 直接吃掉指定位置的棋子
    void capturePieceAt(int x, int y, ChessMan* capturingPiece);

    Q_INVOKABLE void handleMove(int fromIndex, int toX, int toY);

signals:
    void chessDataChanged();
    void currentPlayerChanged();
    void roundNumberChanged();
    void capturedPiecesChanged();
    void gameOverChanged();
    void winnerChanged();
    void isCheckChanged();
    void isCheckMateChanged();
    void checkedPlayerChanged();
    void selfCheckMoveChanged();

private:
    ChessMan* m_board[10][9];
    QList<QObject*> m_pieces;
    QString m_currentPlayer;
    int m_roundNumber;
    QVariantList m_capturedPiecesInfo; // 存储被吃掉的棋子信息
    bool m_gameOver;
    QString m_winner;
    bool m_isCheck;       // 是否处于将军状态
    bool m_isCheckMate;   // 是否处于将死状态
    QString m_checkedPlayer;
    bool m_selfCheckMove;   // 是否尝试了导致自己被将军的移动
};

