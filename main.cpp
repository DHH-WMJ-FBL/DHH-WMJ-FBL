#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "ChessInitializer.h"

int main(
    int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // 初始化所有棋子
    QList<QObject*> allPieces = ChessInitializer::initializePieces();

    // 向 QML 设置上下文属性
    engine.rootContext()->setContextProperty("allPieces", QVariant::fromValue(allPieces));

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("Chess", "Main");

    return app.exec();
}
