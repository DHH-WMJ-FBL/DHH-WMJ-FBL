#pragma once
#include <QObject>
#include <QList>
#include <QVariantList>
#include <QVector>
#include <QString>
// QDebug removed - no longer needed
#include <QTimer>
#include "ChessMan.h"
#include "ChessInitializer.h"
#include "ChessAi.h"

struct CapturePieceInfo {
    QString name;
    QString color;
    QString icon;
    QString capturedBy;
    int round;
};

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
    Q_PROPERTY(bool isAiMode READ isAiMode NOTIFY aiModeChanged)

public:
    explicit ChessController(QObject* parent = nullptr);

    void initializeGame();
    QList<QObject*> getRawPieces() const;
    
    // Property getters
    QString currentPlayer() const;
    int roundNumber() const;
    bool gameOver() const;
    QString winner() const;
    bool isCheck() const;
    bool isCheckMate() const;
    QString checkedPlayer() const;
    bool selfCheckMove() const;
    bool isAiMode() const;

    // QML invokable methods
    Q_INVOKABLE QVariantList getPieces() const;
    Q_INVOKABLE QVariantList capturedPieces() const;
    Q_INVOKABLE void resetGame();
    Q_INVOKABLE void handleMove(int fromIndex, int toX, int toY);
    Q_INVOKABLE void toggleAIMode();
    Q_INVOKABLE void switchTurn();
    // AI depth and time limit functions removed - not needed for current implementation

    // Game logic methods
    ChessMan* getKing(const QString& color) const;
    bool checkForCheck(const QString& color);
    bool checkForCheckMate(const QString& color);
    bool canMoveResolveCheck(ChessMan* piece, int toX, int toY);
    bool isKingFacingKing() const;
    void updateCheckStatus();
    void capturePieceAt(int x, int y, ChessMan* capturingPiece);

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
    void aiModeChanged();

private:
    ChessMan* m_board[10][9];
    QList<QObject*> m_pieces;
    QString m_currentPlayer;
    int m_roundNumber;
    QList<CapturePieceInfo> m_capturedPiecesInfo;
    bool m_gameOver;
    QString m_winner;
    bool m_isCheck;
    bool m_isCheckMate;
    QString m_checkedPlayer;
    bool m_selfCheckMove;
    bool m_isAiMode = false;
    QString aiColor = "黑";
    ChessAI ai;
};

