#pragma once
#include "ChessMan.h"

class Cannon : public ChessMan {
    Q_OBJECT

public:
    explicit Cannon(QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr)
        : ChessMan(name, color, x, y, icon, parent) {}

    bool canMove(int targetX, int targetY, const QList<QObject*>& allPieces) {
        if (m_x != targetX && m_y != targetY)
            return false;

        int count = countPiecesBetween(targetX, targetY, allPieces);
        bool hasTarget = isPieceAt(targetX, targetY, allPieces);

        if (!hasTarget && count == 0)
            return true;

        if (hasTarget && count == 1) {
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
    int countPiecesBetween(int x, int y, const QList<QObject*>& allPieces) const {
        int count = 0;
        if (x == m_x) {
            int step = (y > m_y) ? 1 : -1;
            for (int i = m_y + step; i != y; i += step)
                if (isPieceAt(x, i, allPieces)) count++;
        } else if (y == m_y) {
            int step = (x > m_x) ? 1 : -1;
            for (int i = m_x + step; i != x; i += step)
                if (isPieceAt(i, y, allPieces)) count++;
        }
        return count;
    }

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
