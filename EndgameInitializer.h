#pragma once
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include "ChessMan.h"
#include "King.h"
#include "Advisor.h"
#include "Elephant.h"
#include "Horse.h"
#include "Rook.h"
#include "Cannon.h"
#include "Soldier.h"

// 棋子位置数据结构
struct PiecePosition {
    QString pieceName;      // 棋子名称(如"Rook3")
    QString pieceType;      // 棋子类型(如"Rook") 
    QString color;          // 颜色("红"/"黑")
    int x, y;              // 位置坐标
    QString icon;           // 图标文件名
};

// 残局数据结构
struct EndgameData {
    QString name;           // 残局名称
    QString description;    // 残局描述  
    QList<PiecePosition> pieces;  // 棋子位置列表
    QString firstPlayer;    // 先手方
    int difficulty;         // 难度等级
};

// 残局初始化器类
class EndgameInitializer
{
public:
    // 主要接口方法
    static QList<QObject*> initializeEndgame(const QString& endgameName, ChessMan* board[10][9]);
    static EndgameData parseEndgameData(const QString& endgameName);
    static QList<QObject*> createPiecesFromData(const EndgameData& data, ChessMan* board[10][9]);
    static QStringList getAvailableEndgames();
    
    // 获取残局信息
    static QString getEndgameDescription(const QString& endgameName);
    static int getEndgameDifficulty(const QString& endgameName);
    static QString getEndgameFirstPlayer(const QString& endgameName);
    
private:
    // 内部辅助方法
    static PiecePosition parsePosition(const QString& line);
    static ChessMan* createPiece(const PiecePosition& pos);
    static QString determineColor(const QString& pieceName);
    static QString determinePieceType(const QString& pieceName);
    static QString getIconName(const QString& pieceType, const QString& color);
    
    // 残局数据定义
    static QList<QString> getSevenStarsData();      // 七星聚会数据
    static QList<QString> getEarthwormDragonData(); // 蚯蚓降龙数据
    static QList<QString> getFireBurningCampData(); // 火烧连营数据
    
    // 棋子名称到编号的映射
    static int getPieceNumber(const QString& pieceName);
    static bool isRedPiece(const QString& pieceName);
    static bool isBlackPiece(const QString& pieceName);
}; 