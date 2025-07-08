#pragma once
#include <QObject>
#include <QString>
#include <QList>
// QDebug removed - no debug output needed

// 默认红下黑上(棋盘)
class ChessMan : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString color READ color CONSTANT)
    Q_PROPERTY(int x READ x WRITE setX NOTIFY positionChanged)
    Q_PROPERTY(int y READ y WRITE setY NOTIFY positionChanged)
    Q_PROPERTY(QString icon READ icon CONSTANT)

public:
    ChessMan(QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr)
        : QObject(parent)
        , m_name(name)
        , m_color(color)
        , m_x(x)
        , m_y(y)
        , m_icon(icon)
    {}

    QString name() const { return m_name; }
    QString color() const { return m_color; }
    int x() const { return m_x; }
    int y() const { return m_y; }
    QString icon() const { return m_icon; }

    void setX(int x) {
        if (m_x != x) {
            m_x = x;
            emit positionChanged();
        }
    }

    void setY(int y) {
        if (m_y != y) {
            m_y = y;
            emit positionChanged();
        }
    }
    
    // 检查目标位置是否有棋子
    ChessMan* pieceAt(int x, int y, ChessMan* board[10][9]) const {
        if (x < 0 || x >= 9 || y < 0 || y >= 10) return nullptr;
        return board[y][x];
    }
    
    // 判断目标位置是否有棋子，不管颜色
    bool hasPieceAt(int x, int y, ChessMan* board[10][9]) const {
        return pieceAt(x, y, board) != nullptr;
    }
    
    //判断目标格是否有相同颜色的棋子
    bool isSameColorPieceAt(int x, int y, ChessMan* board[10][9]) const {
        if (x < 0 || x >= 9 || y < 0 || y >= 10) return false;
        ChessMan* piece = board[y][x];
        return piece != nullptr && piece->color() == this->color();
    }
    
    //判断目标格是否有敌方棋子
    bool isEnemyPieceAt(int x, int y, ChessMan* board[10][9]) const {
        if (x < 0 || x >= 9 || y < 0 || y >= 10) return false;
        ChessMan* piece = board[y][x];
        return piece != nullptr && piece->color() != this->color();
    }
    
    //能移动（使用棋子数组）
    virtual bool canMove(int targetX, int targetY, const QList<QObject*>& allPieces) {
        ChessMan* board[10][9] = {};
        for (QObject* obj : allPieces) {
            auto* piece = qobject_cast<ChessMan*>(obj);
            if (piece && piece->x() >= 0 && piece->y() >= 0) {
                board[piece->y()][piece->x()] = piece;
            }
        }
        return canMove(targetX, targetY, board);
    }
    
    //移动到（使用棋子数组）
    virtual bool moveTo(int targetX, int targetY, const QList<QObject*>& allPieces) {
        ChessMan* board[10][9] = {};
        for (QObject* obj : allPieces) {
            auto* piece = qobject_cast<ChessMan*>(obj);
            if (piece && piece->x() >= 0 && piece->y() >= 0) {
                board[piece->y()][piece->x()] = piece;
            }
        }
        return moveTo(targetX, targetY, board);
    }

    //能移动
    virtual bool canMove(int targetX, int targetY, ChessMan* board[10][9]) = 0;
    
    //移动到
    virtual bool moveTo(int targetX, int targetY, ChessMan* board[10][9]) {
        if (canMove(targetX, targetY, board)) {
            // 可移动，执行更新
            setX(targetX);
            setY(targetY);
            return true;
        }
        return false;
    }

signals:
    void positionChanged();

protected:
    QString m_name;
    QString m_color;
    int m_x, m_y;
    QString m_icon;
};
