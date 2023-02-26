#ifndef WEBSOCKETS_HPP
#define WEBSOCKETS_HPP

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QUuid>
#include "wsSettings.hpp"
#include "shared_struct.hpp"
#include <QQueue>
#include "barcode_info.hpp"
#include <QTimer>
#include "qjsontablemodel.h"
#include <QtSql>
#include <QThread>
//#include "SyncData.h"

typedef std::function<void()> async_await;

#include <include/synch_operations.hpp>
#include <include/synch_data.hpp>

struct SynchObjects
{
    arcirk::synchronize::SynchDocumentsBase * synchDoc;
    arcirk::synchronize::SynchInitialDataEntry * synchDataEntry;

    SynchObjects() {}
};

class WebSocketClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ getUrl WRITE setUrl NOTIFY urlChanged)
//    Q_PROPERTY(QString typePriceRef READ typePriceRef WRITE setTypePriceRef NOTIFY typePriceRefChanged)
//    Q_PROPERTY(QString stockRef READ stockRef WRITE setStockRef NOTIFY stockRefChanged)
//    Q_PROPERTY(QString typePrice READ typePrice WRITE setTypePrice NOTIFY typePriceChanged)
//    Q_PROPERTY(QString stock READ stock WRITE setStock NOTIFY stockChanged)

public:
    explicit WebSocketClient(const QUrl& url, QObject *parent = nullptr);
    explicit WebSocketClient(QObject *parent = nullptr);
    ~WebSocketClient();

    Q_INVOKABLE static QString generateHash(const QString& usr, const QString& pwd);
    Q_INVOKABLE bool isStarted();

    Q_INVOKABLE void open(arcirk::Settings * sett);
    Q_INVOKABLE void close();

    void reconnect();

//    QString typePriceRef() const;
//    QString stockRef() const;

//    void setTypePriceRef(const QString& value);
//    void setStockRef(const QString& value);

//    QString typePrice() const;
//    QString stock() const;

//    void setTypePrice(const QString& value);
//    void setStock(const QString& value);

    Q_INVOKABLE void setUrl(const QUrl& url);
    Q_INVOKABLE QUrl getUrl() const;

    static QString get_hash(const QString& first, const QString& second);

    Q_INVOKABLE void updateHttpServiceConfiguration();

    Q_INVOKABLE void updateDavServiceConfiguration();

    Q_INVOKABLE QString cryptPass(const QString& source, const QString& key);

    Q_INVOKABLE void registerDevice();

    Q_INVOKABLE void get_barcode_information(const QString& barcode, BarcodeInfo* bInfo, bool skip_data = false);

    Q_INVOKABLE void get_image_data(BarcodeInfo* bInfo);

    Q_INVOKABLE void checkConnection();

    Q_INVOKABLE void firstLoadDatabase();

    Q_INVOKABLE void startReconnect(){
        if(!wsSettings)
            return;
        if (wsSettings->autoConnect())
            m_reconnect->start(1000 * 60);
    };

    Q_INVOKABLE void startSynchronize(){
        qDebug() << __FUNCTION__;
        //ToDo: Добавить в настройки периодичность запуска.
        m_tmr_synchronize->start(5 * 1000 * 60);
    };

    Q_INVOKABLE void deleteDocument(const QString& ref, const int ver);
    Q_INVOKABLE void getDocuments();
    Q_INVOKABLE void getDocumentInfo(const QString& ref);
    Q_INVOKABLE void getDocumentContent(const QString& ref);
    Q_INVOKABLE QString documentDate(const int value) const;
    Q_INVOKABLE void addDocument(const QString& number, const int date, const QString& comment);
    Q_INVOKABLE void documentContentUpdate(const QString& barcode, const int quantity, const QString& parent, const QString& ref, QJsonTableModel* model);
    Q_INVOKABLE void documentUpdate(const QString& number, const QString& date, const QString& comment, const QString& source);
    Q_INVOKABLE QString documentGenerateNewNumber(const int id);
    Q_INVOKABLE void removeRecord(const QString& ref, QJsonTableModel* model);

    Q_INVOKABLE void debugViewTime();

    Q_INVOKABLE void synchronizeBase();



private:
    QWebSocket* m_client;
    arcirk::Settings * wsSettings;
    SynchObjects synchObjects;

    QUuid m_currentSession;
    QUuid m_currentUserUuid;

    QUrl m_url;
    bool m_started;

    QTimer * m_reconnect;
    QTimer * m_tmr_synchronize;

    QQueue<async_await> m_async_await;
    QVector<QString> m_vecOperations;

    bool is_offline;

    QSqlDatabase sqlDatabase;
    QMap<QString, arcirk::server::server_response> sqlResult;

    //QThread syncOperatiions;
    //SyncData * syncData;

    void syncDataCreateConnections();

    static QString get_sha1(const QByteArray& p_arg);

    void parse_response(const QString& resp);

    void parse_command_to_client(const std::string& receiver, const std::string& sender, const std::string& param);

    void send_command(arcirk::server::server_commands cmd, const nlohmann::json& param = {});

    void update_server_configuration(const QString& typeConf, const std::string& srv_resp);

    void parse_exec_query_result(arcirk::server::server_response& resp);

    void get_workplace_options();

    void get_workplace_view_options();

    void asyncAwait(){
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
    };


    void synchronizeBaseNext(const arcirk::server::server_response& resp);

private slots:

    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onError(QAbstractSocket::SocketError error);
    void onReconnect();
    void onSynchronize();
//    void onEndSynchronize(bool isValid, const nlohmann::json& objects);

signals:
//    void typePriceRefChanged();
//    void stockRefChanged();
//    void typePriceChanged();
//    void stockChanged();
    void urlChanged();

    void displayError(const QString& what, const QString& err);
    void connectionSuccess(); //при успешной авторизации
    void connectionChanged(bool state);

    void updateHsConfiguration(const QString& hsHost, const QString& hsUser, const QString& hsPwd);
    void updateDavConfiguration(const QString& davHost, const QString& davUser, const QString& davPwd);

    void notify(const QString &message);

    void readDocument(const QString& jsonModel);
    void readDocuments(const QString& jsonModel);
    void readDocumentTable(const QString& jsonModel);

    void startAsyncSynchronize(const QString& operationName);
    void endAsyncSynchronize(const QString& operationName);
};

#endif // WEBSOCKETS_HPP
