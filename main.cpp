#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "ChessInitializer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // 声明并初始化棋盘
    ChessMan* board[10][9] = { nullptr };

    // 初始化所有棋子，同时传入 board 数组
    QList<QObject*> allPieces = ChessInitializer::initializePieces(board);

    // 向 QML 暴露棋子列表
    engine.rootContext()->setContextProperty("allPieces", QVariant::fromValue(allPieces));

    // 如果 QML 加载失败则退出
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // 加载 QML 模块
    engine.loadFromModule("Chess", "Main");

    return app.exec();
}
