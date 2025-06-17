#pragma once
#include <QObject>
//默认红下黑上(棋盘)
class ChessMan : public QObject
{
    Q_OBJECT
    Q_PROPERTY(
        QString name READ name CONSTANT)
    Q_PROPERTY(
        QString color READ color CONSTANT)
    Q_PROPERTY(
        int x READ x WRITE setX NOTIFY positionChanged)
    Q_PROPERTY(
        int y READ y WRITE setY NOTIFY positionChanged)
    Q_PROPERTY(
        QString icon READ icon CONSTANT)

public:
    ChessMan(
        QString name, QString color, int x, int y, QString icon, QObject* parent = nullptr)
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

    void setX(
        int x)
    {
        if (m_x != x) {
            m_x = x;
            emit positionChanged();
        }
    }
    void setY(
        int y)
    {
        if (m_y != y) {
            m_y = y;
            emit positionChanged();
        }
    }

    //virtual bool canMove(int targetX, int targetY) = 0;
    //添加：纯虚函数，子类必须实现（带 allPieces 参数）
    virtual bool canMove(int targetX, int targetY, const QList<QObject*>& allPieces) = 0;
    virtual bool moveTo(int targetX, int targetY, const QList<QObject*>& allPieces) = 0;

signals:
    void positionChanged();

protected:
    QString m_name;
    QString m_color;
    int m_x, m_y;
    QString m_icon;
};
