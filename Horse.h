#pragma once
#include "ChessMan.h"
//马类
class Horse : public ChessMan {
    Q_OBJECT

public:
    // 构造函数，传入名字、颜色、初始坐标
    explicit Horse(QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr)
        : ChessMan(name, color, x, y, icon, parent) {}

    bool canMove(int targetX, int targetY, const QList<QObject*>& allPieces) {
        int dx = targetX - m_x;
        int dy = targetY - m_y;

        if ((abs(dx) == 1 && abs(dy) == 2 && !isPieceAt(m_x, m_y + dy / 2, allPieces)) ||
            (abs(dx) == 2 && abs(dy) == 1 && !isPieceAt(m_x + dx / 2, m_y, allPieces))) {
            return !isSameColorPieceAt(targetX, targetY, allPieces);
        }
        return false;
    }

    bool moveTo(int targetX, int targetY, const QList<QObject*>& allPieces) {
        if (canMove(targetX, targetY, allPieces)) {
            setX(targetX);
            setY(targetY);
            return true;
        }
        return false;
    }

private:
    bool isPieceAt(int x, int y, const QList<QObject*>& allPieces) const {
        for (QObject* obj : allPieces) {
            ChessMan* piece = qobject_cast<ChessMan*>(obj);
            if (piece && piece->x() == x && piece->y() == y)
                return true;
        }
        return false;
    }

    bool isSameColorPieceAt(int x, int y, const QList<QObject*>& allPieces) const {
        for (QObject* obj : allPieces) {
            ChessMan* piece = qobject_cast<ChessMan*>(obj);
            if (piece && piece->x() == x && piece->y() == y && piece != this)
                return piece->color() == this->color();
        }
        return false;
    }
};
