#pragma once
#include "ChessMan.h"

class Rook : public ChessMan {
    Q_OBJECT

public:
    explicit Rook(QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr)
        : ChessMan(name, color, x, y, icon, parent) {}

    bool canMove(int targetX, int targetY, const QList<QObject*>& allPieces) {
        if (m_x != targetX && m_y != targetY)
            return false;

        if (isBlocked(targetX, targetY, allPieces))
            return false;

        return !isSameColorPieceAt(targetX, targetY, allPieces);
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
    bool isBlocked(int targetX, int targetY, const QList<QObject*>& allPieces) const {
        int dx = targetX - m_x;
        int dy = targetY - m_y;

        if (dx == 0) {
            int step = (dy > 0) ? 1 : -1;
            for (int y = m_y + step; y != targetY; y += step)
                if (isPieceAt(m_x, y, allPieces)) return true;
        } else {
            int step = (dx > 0) ? 1 : -1;
            for (int x = m_x + step; x != targetX; x += step)
                if (isPieceAt(x, m_y, allPieces)) return true;
        }
        return false;
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
