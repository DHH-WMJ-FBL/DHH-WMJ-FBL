#include "EndgameInitializer.h"
#include <QRegularExpression>
#include <QDebug>

// 主要接口方法实现
QList<QObject*> EndgameInitializer::initializeEndgame(const QString& endgameName, ChessMan* board[10][9]) {
    // 清空棋盘
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            board[y][x] = nullptr;
        }
    }
    
    // 解析残局数据
    EndgameData data = parseEndgameData(endgameName);
    
    // 根据数据创建棋子
    return createPiecesFromData(data, board);
}

EndgameData EndgameInitializer::parseEndgameData(const QString& endgameName) {
    EndgameData data;
    data.name = endgameName;
    
    QList<QString> rawData;
    
    if (endgameName == "七星聚会") {
        data.description = "经典残局：七星聚会，考验攻防转换能力";
        data.firstPlayer = "红";
        data.difficulty = 4;
        rawData = getSevenStarsData();
    } else if (endgameName == "蚯蚓降龙") {
        data.description = "经典残局：蚯蚓降龙，考验精妙计算能力";
        data.firstPlayer = "红";
        data.difficulty = 5;
        rawData = getEarthwormDragonData();
    } else if (endgameName == "火烧连营") {
        data.description = "经典残局：火烧连营，考验连续攻杀技巧";
        data.firstPlayer = "红";
        data.difficulty = 4;
        rawData = getFireBurningCampData();
    }
    
    // 解析每行数据
    for (const QString& line : rawData) {
        if (!line.trimmed().isEmpty()) {
            PiecePosition pos = parsePosition(line);
            if (!pos.pieceName.isEmpty()) {
                data.pieces.append(pos);
            }
        }
    }
    
    return data;
}

QList<QObject*> EndgameInitializer::createPiecesFromData(const EndgameData& data, ChessMan* board[10][9]) {
    QList<QObject*> pieces;
    
    for (const PiecePosition& pos : data.pieces) {
        ChessMan* piece = createPiece(pos);
        if (piece) {
            pieces.append(piece);
            // 将棋子放置到棋盘上
            if (pos.x >= 0 && pos.x < 9 && pos.y >= 0 && pos.y < 10) {
                board[pos.y][pos.x] = piece;
            }
        }
    }
    
    return pieces;
}

QStringList EndgameInitializer::getAvailableEndgames() {
    return QStringList() << "七星聚会" << "蚯蚓降龙" << "火烧连营";
}

QString EndgameInitializer::getEndgameDescription(const QString& endgameName) {
    return parseEndgameData(endgameName).description;
}

int EndgameInitializer::getEndgameDifficulty(const QString& endgameName) {
    return parseEndgameData(endgameName).difficulty;
}

QString EndgameInitializer::getEndgameFirstPlayer(const QString& endgameName) {
    return parseEndgameData(endgameName).firstPlayer;
}

// 内部辅助方法实现
PiecePosition EndgameInitializer::parsePosition(const QString& line) {
    PiecePosition pos;
    
    // 使用正则表达式解析格式: "PieceName x，y"
    QRegularExpression regex(R"((\w+)\s+(\d+)，(\d+))");
    QRegularExpressionMatch match = regex.match(line.trimmed());
    
    if (match.hasMatch()) {
        pos.pieceName = match.captured(1);
        pos.x = match.captured(2).toInt();
        pos.y = match.captured(3).toInt();
        
        // 确定棋子类型和颜色
        pos.pieceType = determinePieceType(pos.pieceName);
        pos.color = determineColor(pos.pieceName);
        pos.icon = getIconName(pos.pieceType, pos.color);
    }
    
    return pos;
}

ChessMan* EndgameInitializer::createPiece(const PiecePosition& pos) {
    ChessMan* piece = nullptr;
    
    if (pos.pieceType == "King") {
        piece = new King(pos.pieceName, pos.color, pos.x, pos.y, pos.icon);
    } else if (pos.pieceType == "Advisor") {
        piece = new Advisor(pos.pieceName, pos.color, pos.x, pos.y, pos.icon);
    } else if (pos.pieceType == "Elephant") {
        piece = new Elephant(pos.pieceName, pos.color, pos.x, pos.y, pos.icon);
    } else if (pos.pieceType == "Horse") {
        piece = new Horse(pos.pieceName, pos.color, pos.x, pos.y, pos.icon);
    } else if (pos.pieceType == "Rook") {
        piece = new Rook(pos.pieceName, pos.color, pos.x, pos.y, pos.icon);
    } else if (pos.pieceType == "Cannon") {
        piece = new Cannon(pos.pieceName, pos.color, pos.x, pos.y, pos.icon);
    } else if (pos.pieceType == "Soldier") {
        piece = new Soldier(pos.pieceName, pos.color, pos.x, pos.y, pos.icon);
    }
    
    return piece;
}

QString EndgameInitializer::determineColor(const QString& pieceName) {
    if (isRedPiece(pieceName)) {
        return "红";
    } else if (isBlackPiece(pieceName)) {
        return "黑";
    }
    return "";
}

QString EndgameInitializer::determinePieceType(const QString& pieceName) {
    if (pieceName.contains("King")) return "King";
    if (pieceName.contains("Advisor")) return "Advisor";
    if (pieceName.contains("Elephant")) return "Elephant";
    if (pieceName.contains("Horse")) return "Horse";
    if (pieceName.contains("Rook")) return "Rook";
    if (pieceName.contains("Cannon")) return "Cannon";
    if (pieceName.contains("Soldier")) return "Soldier";
    return "";
}

QString EndgameInitializer::getIconName(const QString& pieceType, const QString& color) {
    QString colorSuffix = (color == "红") ? "_red.png" : "_black.png";
    return pieceType.toLower() + colorSuffix;
}

// 残局数据定义
QList<QString> EndgameInitializer::getSevenStarsData() {
    return QList<QString>() 
        << "Rook3 4，0"      // 黑车3号
        << "King2 5，0"      // 黑王2号
        << "Soldier1 3，1"   // 红兵1号
        << "Elephant3 4，2"  // 黑象3号
        << "Soldier2 5，2"   // 红兵2号
        << "Soldier3 8，5"   // 红兵3号
        << "Soldier6 1，7"   // 黑兵6号
        << "Soldier7 4，7"   // 黑兵7号
        << "Soldier8 3，8"   // 黑兵8号
        << "Soldier9 5，8"   // 黑兵9号
        << "Cannon1 7，7"    // 红炮1号
        << "King1 4，9"      // 红王1号
        << "Rook1 6，9"      // 红车1号
        << "Rook2 7，9";     // 红车2号
}

QList<QString> EndgameInitializer::getEarthwormDragonData() {
    return QList<QString>()
        << "Advisor3 3，0"   // 黑士3号
        << "King2 4，0"      // 黑王2号
        << "Advisor4 4，1"   // 黑士4号
        << "Elephant3 4，2"  // 黑象3号
        << "Soldier6 2，4"   // 黑兵6号
        << "Rook1 5，5"      // 红车1号
        << "Soldier1 8，5"   // 红兵1号
        << "Soldier8 4，8"   // 黑兵8号 (修正重复编号)
        << "Soldier7 6，8"   // 黑兵7号
        << "King1 5，9"      // 红王1号
        << "Rook2 8，9";     // 红车2号
}

QList<QString> EndgameInitializer::getFireBurningCampData() {
    return QList<QString>()
        << "Soldier1 1，4"   // 红兵1号
        << "King2 3，0"      // 黑王2号
        << "Rook1 4，1"      // 红车1号
        << "Elephant3 4，2"  // 黑象3号
        << "Cannon1 8，2"    // 红炮1号
        << "Rook2 6，4"      // 红车2号
        << "Cannon2 7，4"    // 红炮2号
        << "Cannon3 8，4"    // 黑炮3号
        << "Elephant1 6，5"  // 红象1号
        << "Soldier6 3，8"   // 黑兵6号
        << "Soldier7 4，7"   // 黑兵7号
        << "Soldier8 4，8"   // 黑兵8号
        << "King1 5，9"      // 红王1号
        << "Soldier9 7，8";  // 黑兵9号
}

// 棋子名称到编号的映射
int EndgameInitializer::getPieceNumber(const QString& pieceName) {
    QRegularExpression regex(R"(\d+)");
    QRegularExpressionMatch match = regex.match(pieceName);
    return match.hasMatch() ? match.captured(0).toInt() : 0;
}

bool EndgameInitializer::isRedPiece(const QString& pieceName) {
    int number = getPieceNumber(pieceName);
    
    // 红方棋子编号规则
    if (pieceName.contains("King") && number == 1) return true;
    if (pieceName.contains("Advisor") && (number == 1 || number == 2)) return true;
    if (pieceName.contains("Elephant") && (number == 1 || number == 2)) return true;
    if (pieceName.contains("Horse") && (number == 1 || number == 2)) return true;
    if (pieceName.contains("Rook") && (number == 1 || number == 2)) return true;
    if (pieceName.contains("Cannon") && (number == 1 || number == 2)) return true;
    if (pieceName.contains("Soldier") && (number >= 1 && number <= 5)) return true;
    
    return false;
}

bool EndgameInitializer::isBlackPiece(const QString& pieceName) {
    int number = getPieceNumber(pieceName);
    
    // 黑方棋子编号规则
    if (pieceName.contains("King") && number == 2) return true;
    if (pieceName.contains("Advisor") && (number == 3 || number == 4)) return true;
    if (pieceName.contains("Elephant") && (number == 3 || number == 4)) return true;
    if (pieceName.contains("Horse") && (number == 3 || number == 4)) return true;
    if (pieceName.contains("Rook") && (number == 3 || number == 4)) return true;
    if (pieceName.contains("Cannon") && (number == 3 || number == 4)) return true;
    if (pieceName.contains("Soldier") && (number >= 6 && number <= 10)) return true;
    
    return false;
} 