#pragma once
#include "ChessMan.h"

class Soldier : public ChessMan {
    Q_OBJECT

public:
    explicit Soldier(QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr)
        : ChessMan(name, color, x, y, icon, parent) {}

    bool canMove(int targetX, int targetY, const QList<QObject*>& allPieces) {
        int dx = targetX - m_x;
        int dy = targetY - m_y;

        if (color() == "红" || color() == "red") {
            if (m_y >= 5) {
                return dx == 0 && dy == -1 && !isSameColorPieceAt(targetX, targetY, allPieces);
            } else {
                return ((dx == 0 && dy == -1) || (abs(dx) == 1 && dy == 0)) &&
                       !isSameColorPieceAt(targetX, targetY, allPieces);
            }
        } else {
            if (m_y <= 4) {
                return dx == 0 && dy == 1 && !isSameColorPieceAt(targetX, targetY, allPieces);
            } else {
                return ((dx == 0 && dy == 1) || (abs(dx) == 1 && dy == 0)) &&
                       !isSameColorPieceAt(targetX, targetY, allPieces);
            }
        }
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
