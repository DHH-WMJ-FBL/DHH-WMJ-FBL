#pragma once
#include "ChessMan.h"

class Elephant : public ChessMan
{
    Q_OBJECT

public:
    explicit Elephant(
        QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr)
        : ChessMan(name, color, x, y, icon, parent)
    {}

    // 判断是否能移动
    bool canMove(int targetX, int targetY, const QList<QObject*>& allPieces) override
    {
        int dx = targetX - m_x;
        int dy = targetY - m_y;

        // 必须是走田字格（斜着两格）
        if (abs(dx) == 2 && abs(dy) == 2) {
            // 不允许过河
            if ((m_color == "红" || m_color == "red") && targetY <= 4)
                return false;
            if ((m_color == "黑" || m_color == "black") && targetY >= 5)
                return false;

            // 判断象眼是否被挡
            int eyeX = m_x + dx / 2;
            int eyeY = m_y + dy / 2;
            for (QObject* obj : allPieces) {
                ChessMan* piece = qobject_cast<ChessMan*>(obj);
                if (piece && piece->x() == eyeX && piece->y() == eyeY)
                    return false; // 象眼被挡
            }

            // 不允许走到己方棋子位置
            for (QObject* obj : allPieces) {
                ChessMan* piece = qobject_cast<ChessMan*>(obj);
                if (piece && piece->x() == targetX && piece->y() == targetY && piece != this) {
                    if (piece->color() == this->color())
                        return false;
                }
            }

            return true;
        }

        return false;
    }

    // 移动（已判断合法）
    bool moveTo(int targetX, int targetY, const QList<QObject*>& allPieces) override
    {
        if (canMove(targetX, targetY, allPieces)) {
            setX(targetX);
            setY(targetY);
            return true;
        }
        return false;
    }
};
