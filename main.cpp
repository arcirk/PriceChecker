#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>
#include "include/wsSettings.hpp"
#include "include/websockets.hpp"
#include "include/barcode_parser.hpp"
#include "include/barcode_info.hpp"
#include <QScreen>
#include "include/qproxymodel.h"
#include "include/qjsontablemodel.h"

#ifdef Q_OS_ANDROID
#include "include/qtandroidservice.h"
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");

    qmlRegisterType<arcirk::Settings>("Settings", 1, 0, "Settings");
    qmlRegisterType<WebSocketClient>("WebSocketClient", 1, 0, "WebSocketClient");
    qmlRegisterType<BarcodeParser>("BarcodeParser", 1, 0, "BarcodeParser");
    qmlRegisterType<BarcodeInfo>("BarcodeInfo", 1, 0, "BarcodeInfo");
    qmlRegisterType<QJsonTableModel>("QJsonTableModel", 1, 0, "QJsonTableModel");
    qmlRegisterType<QProxyModel>("QProxyModel", 1, 0, "QProxyModel");

    QQmlApplicationEngine engine;
#ifdef Q_OS_ANDROID
    QtAndroidService *qtAndroidService = new QtAndroidService(&app);
#endif
    QScreen *screen = app.primaryScreen();
    int width = screen->size().width();
    int height = screen->size().height();

    QQmlContext* context = engine.rootContext();
    context->setContextProperty("screenWidth", width);
    context->setContextProperty("screenHeight", height);
#ifdef Q_OS_ANDROID
    context->setContextProperty(QLatin1String("qtAndroidService"), qtAndroidService);
#endif
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
