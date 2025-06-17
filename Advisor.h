#pragma once
#include "ChessMan.h"

class Advisor : public ChessMan {
    Q_OBJECT

public:
    explicit Advisor(QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr)
        : ChessMan(name, color, x, y, icon, parent) {}

    bool canMove(int targetX, int targetY, const QList<QObject*>& allPieces) {
        int dx = abs(targetX - m_x);
        int dy = abs(targetY - m_y);

        if (dx == 1 && dy == 1) {
            if ((color() == "红" || color() == "red") &&
                targetX >= 3 && targetX <= 5 && targetY >= 7 && targetY <= 9) {
                return !isSameColorPieceAt(targetX, targetY, allPieces);
            } else if ((color() == "黑" || color() == "black") &&
                       targetX >= 3 && targetX <= 5 && targetY >= 0 && targetY <= 2) {
                return !isSameColorPieceAt(targetX, targetY, allPieces);
            }
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
    bool isSameColorPieceAt(int x, int y, const QList<QObject*>& allPieces) const {
        for (QObject* obj : allPieces) {
            ChessMan* piece = qobject_cast<ChessMan*>(obj);
            if (piece && piece->x() == x && piece->y() == y && piece != this)
                return piece->color() == this->color();
        }
        return false;
    }
};
