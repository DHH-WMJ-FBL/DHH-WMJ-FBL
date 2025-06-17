#pragma once
#include "ChessMan.h"

class King : public ChessMan
{
    Q_OBJECT

public:
    explicit King(
        QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr)
        : ChessMan(name, color, x, y, icon, parent)
    {}
    //判断能否移动
    bool canMove(int targetX, int targetY, const QList<QObject*>& allPieces)
    {
        int dx = abs(targetX - m_x);
        int dy = abs(targetY - m_y);

        // 移动必须是横向或纵向一格
        if ((dx == 1 && dy == 0) || (dx == 0 && dy == 1)) {
            // 判断是否在九宫格内
            if ((m_color == "红" || m_color == "red") &&
                (targetX >= 3 && targetX <= 5 && targetY >= 7 && targetY <= 9)) {
                return !isSameColorPieceAt(targetX, targetY, allPieces);
            } else if ((m_color == "黑" || m_color == "black") &&
                       (targetX >= 3 && targetX <= 5 && targetY >= 0 && targetY <= 2)) {
                return !isSameColorPieceAt(targetX, targetY, allPieces);
            }
        }

        return false;
    }
    //移动棋子（会更新allPieces）
    bool moveTo(int targetX, int targetY, const QList<QObject*>& allPieces)
    {
        if (canMove(targetX, targetY, allPieces)) {
            setX(targetX);
            setY(targetY);
            return true;
        }
        return false;
    }

private:
    //防止相同颜色重叠
    bool isSameColorPieceAt(int x, int y, const QList<QObject*>& allPieces) const
    {
        for (QObject* obj : allPieces) {
            ChessMan* piece = qobject_cast<ChessMan*>(obj);
            if (piece && piece->x() == x && piece->y() == y && piece != this) {
                return piece->color() == this->color();
            }
        }
        return false;
    }
};
