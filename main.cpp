#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>
#include "include/wsSettings.hpp"
#include "include/websockets.hpp"
#include "include/barcode_parser.hpp"
#include "include/barcode_info.hpp"
#include <QScreen>
#include "include/qtandroidservice.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");

    qmlRegisterType<arcirk::Settings>("Settings", 1, 0, "Settings");
    qmlRegisterType<WebSocketClient>("WebSocketClient", 1, 0, "WebSocketClient");
    qmlRegisterType<BarcodeParser>("BarcodeParser", 1, 0, "BarcodeParser");
    qmlRegisterType<BarcodeInfo>("BarcodeInfo", 1, 0, "BarcodeInfo");

    QQmlApplicationEngine engine;
    QtAndroidService *qtAndroidService = new QtAndroidService(&app);

    QScreen *screen = app.primaryScreen();
    int width = screen->size().width();
    int height = screen->size().height();

    QQmlContext* context = engine.rootContext();
    context->setContextProperty("screenWidth", width);
    context->setContextProperty("screenHeight", height);
    context->setContextProperty(QLatin1String("qtAndroidService"), qtAndroidService);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
