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
#include <QtSql>
#include "include/database_struct.hpp"
#include <QException>

#ifdef Q_OS_ANDROID
#include "include/qtandroidservice.h"
#endif

QVector<QString> get_tables(QSqlDatabase& sql){

    QSqlQuery rs("SELECT name FROM sqlite_master WHERE type='table';");

    QVector<QString> result;
    while (rs.next())
    {
        QString tbl = rs.value(0).toString();
        result.push_back(tbl);
    }

    return result;
}

QVector<QString> get_views(QSqlDatabase& sql){

    QSqlQuery rs("SELECT name FROM sqlite_master WHERE type='view';");
    QVector<QString> result;
    while (rs.next())
    {
        QString tbl = rs.value(0).toString();
        result.push_back(tbl);
    }

    return result;
}


void verify_tables(QSqlDatabase& sql, const QVector<QString>& tables_arr, std::map<std::string, std::string>& t_ddl){

    QSqlQuery a_query;

    for (auto itr = t_ddl.begin(); itr != t_ddl.end() ; ++itr) {
        if(std::find(tables_arr.begin(), tables_arr.end(), QString::fromStdString(itr->first)) == tables_arr.end()) {
            try {
                a_query.exec(QString::fromStdString(itr->second));
            } catch (const QException &e) {
                std::cerr << e.what() << std::endl;
                continue;
            }
        }
    }


}

void verifyDatabase(){

    using namespace arcirk::database;

    QSqlDatabase sql = QSqlDatabase::addDatabase("QSQLITE");
    sql.setDatabaseName("pricechecker.sqlite");

    if (!sql.open()) {
        qDebug() << sql.lastError().text();
        return;
    }

    sql.close();

    try {
        auto m_tables = get_tables(sql);
        auto m_views = get_views(sql);

        std::map<std::string, std::string> t_ddl;
        t_ddl.emplace(arcirk::enum_synonym(tables::tbDocuments), documents_table_ddl);
        t_ddl.emplace(arcirk::enum_synonym(tables::tbDocumentsTables), document_table_table_ddl);
        t_ddl.emplace(arcirk::enum_synonym(tables::tbNomenclature), nomenclature_table_ddl);

        //проверка таблиц
        verify_tables(sql, m_tables, t_ddl);
        //проверка представлений
        verify_tables(sql, m_views, t_ddl);
    } catch (const QException &e) {
        std::cerr << e.what() << std::endl;
    }

}
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    verifyDatabase();

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
    QScreen *screen = app.primaryScreen();
    int width = screen->size().width();
    int height = screen->size().height();
#else
    //QScreen *screen = app.primaryScreen();
    int width = 720; //screen->size().width();
    int height = 1280; //screen->size().height();
#endif


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
