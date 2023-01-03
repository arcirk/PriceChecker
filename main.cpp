#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>
#include "include/wsSettings.hpp"
#include "include/websockets.hpp"

//#include "include/websocket.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");

    qmlRegisterType<arcirk::Settings>("Settings", 1, 0, "Settings");
    qmlRegisterType<WebSocketClient>("WebSocketClient", 1, 0, "WebSocketClient");

    QQmlApplicationEngine engine;
    //const QUrl url(u"qrc:/PraceChecker/main.qml"_qs);
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
