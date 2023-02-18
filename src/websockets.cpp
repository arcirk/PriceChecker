#include "include/websockets.hpp"
#include "include/database_struct.hpp"
#include "include/query_builder.hpp"
#include "include/verify_database.hpp"

//#include <QNetworkInterface>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QDateTime>
#include <memory>

#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))
void* _crypt(void* data, unsigned data_size, void* key, unsigned key_size)
{
    assert(data && data_size);
    if (!key || !key_size) return data;

    auto* kptr = (uint8_t*)key; // начало ключа
    uint8_t* eptr = kptr + key_size; // конец ключа

    for (auto* dptr = (uint8_t*)data; data_size--; dptr++)
    {
        *dptr ^= *kptr++;
        if (kptr == eptr) kptr = (uint8_t*)key; // переход на начало ключа
    }
    return data;
}

std::string crypt(const std::string &source, const std::string& key) {

    void * text = (void *) source.c_str();
    void * pass = (void *) key.c_str();
    _crypt(text, ARR_SIZE(source.c_str()), pass, ARR_SIZE(key.c_str()));

    std::string result((char*)text);


    return result;
}

WebSocketClient::WebSocketClient(const QUrl &url, QObject *parent)
    : QObject{parent},
    m_url(url)
{
    m_started = false;

    m_client = new QWebSocket();

    connect(m_client, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(m_client, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(m_client, &QWebSocket::textMessageReceived, this, &WebSocketClient::onTextMessageReceived);

    connect(m_client, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
        [=](QAbstractSocket::SocketError error){ onError(error); });

    m_reconnect = new QTimer(this);
    connect(m_reconnect,SIGNAL(timeout()),this,SLOT(onReconnect()));
    m_tmr_synchronize = new QTimer(this);
    connect(m_tmr_synchronize,SIGNAL(timeout()),this,SLOT(onSynchronize()));

    syncData = new SyncData(this);
    syncDataCreateConnections();

    sqlDatabase = QSqlDatabase::addDatabase("QSQLITE");
#ifndef Q_OS_WINDOWS
    sqlDatabase.setDatabaseName("pricechecker.sqlite");
    syncData->setDatabaseFileName("pricechecker.sqlite");
#else
    auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(path);
    if(!dir.exists())
        dir.mkpath(path);

    auto fileName= path + "/pricechecker.sqlite";
    sqlDatabase.setDatabaseName(fileName);
    syncData->setDatabaseFileName(fileName);
#endif

    if (!sqlDatabase.open()) {
        qCritical() << sqlDatabase.lastError().text();
    }

    is_offline = true;

    startSynchronize();
}

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject{parent}
{
    m_started = false;

    m_client = new QWebSocket();
    connect(m_client, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(m_client, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(m_client, &QWebSocket::textMessageReceived, this, &WebSocketClient::onTextMessageReceived);

    connect(m_client, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            [=](QAbstractSocket::SocketError error){ onError(error); });

    m_reconnect = new QTimer(this);
    connect(m_reconnect,SIGNAL(timeout()),this,SLOT(onReconnect()));
    m_tmr_synchronize = new QTimer(this);
    connect(m_tmr_synchronize,SIGNAL(timeout()),this,SLOT(onSynchronize()));

    syncData = new SyncData(this);
    syncDataCreateConnections();

    sqlDatabase = QSqlDatabase::addDatabase("QSQLITE");
#ifndef Q_OS_WINDOWS
    sqlDatabase.setDatabaseName("pricechecker.sqlite");
#else
    auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(path);
    if(!dir.exists())
        dir.mkpath(path);

    auto fileName= path + "/pricechecker.sqlite";
    sqlDatabase.setDatabaseName(fileName);
#endif

    if (!sqlDatabase.open()) {
        qDebug() << sqlDatabase.lastError().text();
    }

    is_offline = true;

    startSynchronize();
}

WebSocketClient::~WebSocketClient()
{
    if(isStarted())
        m_client->close();
    m_tmr_synchronize->stop();
    m_reconnect->stop();
    syncOperatiions.exit();
}

bool WebSocketClient::isStarted()
{
    return m_client->state() == QAbstractSocket::ConnectedState;
}

void WebSocketClient::open(arcirk::Settings * sett)
{
    if(!m_url.isValid()){
        emit displayError("Error", "Не задан адрес сервера!");
        return;
    }

    if(isStarted())
        return;

    qDebug() << __FUNCTION__;

    wsSettings = sett;
    m_url = wsSettings->url();
    m_client->open(m_url);
}

void WebSocketClient::close()
{
    if(m_client)
        m_client->close();
}

void WebSocketClient::reconnect()
{
    open(wsSettings);
}

void WebSocketClient::setUrl(const QUrl &url)
{
    m_url = url;
}

QUrl WebSocketClient::getUrl() const
{
    return m_url;
}
void WebSocketClient::onConnected()
{
    qDebug() << __FUNCTION__;

    auto param = arcirk::client::client_param();
    param.app_name = "PriceChecker";
    param.host_name = QSysInfo::machineHostName().toStdString();
    param.system_user = "root";

    if(wsSettings){
        param.user_name = wsSettings->userName().toStdString();
        param.hash = wsSettings->hash().toStdString();
        param.device_id = wsSettings->deviceId().toStdString();
    }

    std::string p = pre::json::to_json(param).dump();
    QByteArray ba(p.c_str());
    nlohmann::json _param = {
        {"SetClientParam", QString(ba.toBase64()).toStdString()}
    };

    QString cmd = "cmd SetClientParam " + QString::fromStdString(_param.dump()).toUtf8().toBase64();

    m_client->sendTextMessage(cmd);
}

void WebSocketClient::onDisconnected()
{
    qDebug() << __FUNCTION__;
    emit connectionChanged(false);

}

void WebSocketClient::onTextMessageReceived(const QString &message)
{
    qDebug() << __FUNCTION__;

    if (message == "\n" || message.isEmpty() || message == "pong")
        return;

    //qDebug() << message;
    parse_response(message);

}

void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    qDebug() << __FUNCTION__;
}

void WebSocketClient::onReconnect()
{
    qDebug() << __FUNCTION__;
    if(isStarted()){
        if(m_reconnect->isActive())
            m_reconnect->stop();
    }else{
        m_async_await.append(std::bind(&WebSocketClient::reconnect, this));
        asyncAwait();
    }
}

void WebSocketClient::onSynchronize()
{

    qDebug() << __FUNCTION__;
    if(!isStarted())
        return;
    if(syncData->running())
        return;

    if(m_async_await.size() > 0){
        m_async_await.append(std::bind(&WebSocketClient::synchronizeBase, this));
    }else
        synchronizeBase();

}

void WebSocketClient::onEndSynchronize(bool isValid, const nlohmann::json &objects)
{
    qDebug() << __FUNCTION__ << isValid;
    if(!isValid)
        return;

    if(!isStarted())
        return;

    send_command(arcirk::server::server_commands::SyncUpdateDataOnTheServer, {
                     {"device_id", wsSettings->deviceId().toStdString()},
                     {"table_name", arcirk::enum_synonym(arcirk::database::tables::tbDocuments)},
                     {"base64_param", QByteArray::fromStdString(objects.dump()).toBase64().toStdString()},
                 });
}

QString WebSocketClient::generateHash(const QString &usr, const QString &pwd)
{
    return get_hash(usr, pwd);
}

QString WebSocketClient::get_sha1(const QByteArray& p_arg){
    auto sha = QCryptographicHash::hash(p_arg, QCryptographicHash::Sha1);
    return sha.toHex();
}

void WebSocketClient::parse_response(const QString &resp)
{
    //qDebug() << __FUNCTION__;
    try {
        auto msg = pre::json::from_json<arcirk::server::server_response>(resp.toStdString());
        qDebug() << __FUNCTION__ << QString::fromStdString(msg.command);
        if(msg.command == arcirk::enum_synonym(arcirk::server::server_commands::SetClientParam)){
            if(msg.message == "OK"){
                QString result = QByteArray::fromBase64(msg.param.data());
                auto param = pre::json::from_json<arcirk::client::client_param>(result.toStdString());
                m_currentSession = QUuid::fromString(QString::fromStdString(param.session_uuid));
                m_currentUserUuid = QUuid::fromString(QString::fromStdString(param.user_uuid));
                emit connectionSuccess();
                emit connectionChanged(true);
                //поочередное выполнение
                m_async_await.append(std::bind(&WebSocketClient::updateHttpServiceConfiguration, this));
                m_async_await.append(std::bind(&WebSocketClient::get_workplace_options, this));
                //m_async_await.append(std::bind(&WebSocketClient::synchronizeBase, this));
                asyncAwait();
            }else{
                emit displayError("SetClientParam", "Ошибка авторизации");
            }
        }else if(msg.command == arcirk::enum_synonym(arcirk::server::server_commands::HttpServiceConfiguration)){
            if(msg.result != "error")
                update_server_configuration("httpService", msg.result);
        }else if(msg.command == arcirk::enum_synonym(arcirk::server::server_commands::ExecuteSqlQuery)){
            if(msg.result != "error")
                parse_exec_query_result(msg);
            else
                emit displayError("ExecuteSqlQuery", QString::fromStdString(msg.message));
        }else if(msg.command == arcirk::enum_synonym(arcirk::server::server_commands::CommandToClient)){
            //qDebug() << __FUNCTION__ << "CommandToClient";
            parse_command_to_client(msg.receiver, msg.sender, msg.param);
        }else if(msg.command == arcirk::enum_synonym(arcirk::server::server_commands::SyncGetDiscrepancyInData)){
            synchronizeBaseNext(msg);
        }
    } catch (std::exception& e) {
        qCritical() << __FUNCTION__ << e.what();
        emit displayError("parse_response", e.what());
    }
}

void WebSocketClient::parse_command_to_client(const std::string& receiver, const std::string& sender, const std::string& param){

    try {
        auto param_ = nlohmann::json::parse(QByteArray::fromBase64(param.data()).toStdString());
        std::string command = param_.value("command", "");
        auto cmd_param = nlohmann::json::parse(QByteArray::fromBase64(param_.value("param", "").data()).toStdString());
        if(command.empty()){
            qCritical() << __FUNCTION__ << "Ошибка формата параметров команды";
            return;
        }

        if(command == arcirk::enum_synonym(arcirk::server::server_commands::SetNewDeviceId)){
            if(wsSettings){
                if(m_currentSession.toString(QUuid::WithoutBraces).toStdString() != sender){
                    std::string new_device_id = cmd_param.value("device_id", "");
                    if(new_device_id.empty()){
                        qCritical() << __FUNCTION__ << "Передан не корректный идентификатор устройства!";
                        return;
                    }
                    wsSettings->setDeviceId(QString::fromStdString(new_device_id));
                    wsSettings->save();
                }
            }
        }
    } catch (const std::exception& e) {
        qCritical() << e.what();
    }


}

void WebSocketClient::send_command(arcirk::server::server_commands cmd, const nlohmann::json &param)
{
    qDebug() << __FUNCTION__ << QString::fromStdString(arcirk::enum_synonym(cmd));
    std::string p = param.dump();
    QByteArray ba(p.c_str());
    nlohmann::json _param = {
        {arcirk::enum_synonym(cmd), QString(ba.toBase64()).toStdString()}
    };

    QString cmd_text = "cmd " + QString::fromStdString(arcirk::enum_synonym(cmd)) + " " + QString::fromStdString(_param.dump()).toUtf8().toBase64();

    m_client->sendTextMessage(cmd_text);
}

void WebSocketClient::update_server_configuration(const QString &typeConf, const std::string &srv_resp)
{
    qDebug() << __FUNCTION__;
    try {
        std::string p = QByteArray::fromBase64(srv_resp.data()).toStdString();
        nlohmann::json resp = nlohmann::json::parse(p);
        if(resp.is_discarded())
            return;

        QString hsService = QString::fromStdString(resp.value("HSHost", ""));
        QString hsUser = QString::fromStdString(resp.value("HSUser", ""));
        QString hsPwd = QString::fromStdString(resp.value("HSPassword", ""));

        emit updateHsConfiguration(hsService, hsUser, hsPwd);

        asyncAwait();

    } catch (const std::exception& e) {
        qCritical() << e.what();
    }


}

void WebSocketClient::parse_exec_query_result(arcirk::server::server_response &resp)
{

    try {
        nlohmann::json _p = nlohmann::json::parse(QByteArray::fromBase64(resp.param.data()).toStdString());
        if(_p.is_discarded()){
            return;
        }
        std::string p_str = _p.value("query_param", "");
        if(p_str.empty())
            return;
        std::string p = QByteArray::fromBase64(p_str.data()).toStdString();
        auto param = nlohmann::json::parse(p);
        std::string table_name = param.value("table_name", "");
        std::string query_type = param.value("query_type", "");
        std::string operation = param.value("operation", "");

        qDebug() << __FUNCTION__ << "table_name:" << table_name.c_str() << "query_type: " << query_type.c_str();

        if(resp.message == "OK" && !table_name.empty()){
            nlohmann::json tb_name = table_name;
            if(tb_name.get<arcirk::database::tables>() == arcirk::database::tables::tbDevices){
                if(query_type == "insert_or_update")
                    emit notify("Устройство успешно зарегистрировано!");
                else if(query_type == "select"){
                    nlohmann::json table = nlohmann::json::parse(QByteArray::fromBase64(resp.result.data()).toStdString());
                    auto rows = table.value("rows", nlohmann::json{});
                    if(rows.size() > 0 && wsSettings){
                        auto item = rows[0];
                        if(item.is_object()){
                            std::string id = item.value("ref","");
                            if(wsSettings->deviceId().toStdString() == id){
                                wsSettings->update_workplace_data(item);
                                m_async_await.append(std::bind(&WebSocketClient::get_workplace_view_options, this));
                            }
                        }
                    }
                }
            }else if(tb_name.get<arcirk::database::views>() == arcirk::database::views::dvDevicesView){
                nlohmann::json table = nlohmann::json::parse(QByteArray::fromBase64(resp.result.data()).toStdString());
                auto rows = table.value("rows", nlohmann::json{});
                if(rows.size() > 0 && wsSettings){
                    auto item = rows[0];
                    if(item.is_object()){
                        std::string id = item.value("ref","");
                        if(wsSettings->deviceId().toStdString() == id){
                            wsSettings->update_workplace_view(item);
                        }
                    }
                }
            }else if(tb_name.get<arcirk::database::tables>() == arcirk::database::tables::tbDocuments){
                if(query_type == "select"){
                    if(operation.empty()){
                        QString table = QByteArray::fromBase64(resp.result.data());
                        emit readDocuments(table);
                    }

                }else if(query_type == "update_or_insert")
                    getDocuments();
            }else if(tb_name.get<arcirk::database::tables>() == arcirk::database::tables::tbDocumentsTables){
                if(query_type == "select"){
                    QString table = QByteArray::fromBase64(resp.result.data());
                    emit readDocumentTable(table);
                }
            }
        }

        asyncAwait();

    } catch (const std::exception& e) {
        qCritical() << __FUNCTION__ << e.what();
    }
}

void WebSocketClient::get_workplace_options()
{
    qDebug() << __FUNCTION__ << wsSettings->deviceId();

    if(wsSettings){
        QUuid devId = QUuid::fromString(wsSettings->deviceId());
        if(devId.isNull()){
            qCritical() << __FUNCTION__ << "Не верный идентификатор устройства!";
            return;
        }
        std::string ref = devId.toString(QUuid::StringFormat::WithoutBraces).toStdString();
        qDebug() << QString::fromStdString(ref);
        nlohmann::json query_param = {
            {"table_name", "Devices"},
            {"query_type", "select"},
            {"where_values", nlohmann::json({
                 {"ref", ref}
             })}
        };

        std::string base64_param = QByteArray::fromStdString(query_param.dump()).toBase64().toStdString();
        send_command(arcirk::server::server_commands::ExecuteSqlQuery, {
                         {"query_param", base64_param}
                     });

    }else
        qCritical() << __FUNCTION__ << "Не инициализирован объект свойств!";
}

void WebSocketClient::get_workplace_view_options()
{
    qDebug() << __FUNCTION__;

    if(wsSettings){
        QUuid devId = QUuid::fromString(wsSettings->deviceId());
        if(devId.isNull()){
            qCritical() << __FUNCTION__ << "Не верный идентификатор устройства!";
            return;
        }
        nlohmann::json query_param = {
            {"table_name", "DevicesView"},
            {"query_type", "select"},
            {"where_values", nlohmann::json({
                 {"ref", devId.toString(QUuid::StringFormat::WithoutBraces).toStdString()}
             })}
        };

        std::string base64_param = QByteArray::fromStdString(query_param.dump()).toBase64().toStdString();
        send_command(arcirk::server::server_commands::ExecuteSqlQuery, {
                         {"query_param", base64_param}
                     });

    }
}

void WebSocketClient::synchronizeBase()
{
    qDebug() << __FUNCTION__;

    if(!isStarted()){
        asyncAwait();
        return;
    }

    //Получаем список документов с севрвера
    QUuid devId = QUuid::fromString(wsSettings->deviceId());
    if(devId.isNull()){
        qCritical() << __FUNCTION__ << "Не верный идентификатор устройства!";
        asyncAwait();
        return;
    }

    if(!sqlDatabase.isOpen()){
        qDebug() << __FUNCTION__ << "База данных не открыта!";
        asyncAwait();
        return;
    }

    QSqlQuery rs;
    rs.exec("select ref,version from Documents");
    nlohmann::json t_docs{};
    while (rs.next()) {
        t_docs += nlohmann::json{
            {"ref", rs.value("ref").toString().toStdString()},
            {"version", rs.value("version").toInt()}
        };
    };

    std::string base64_param = QByteArray::fromStdString(t_docs.dump()).toBase64().toStdString();

    using namespace arcirk::database;

    send_command(arcirk::server::server_commands::SyncGetDiscrepancyInData, {
                     {"base64_param", base64_param},
                     {"device_id", wsSettings->deviceId().toStdString()},
                     {"table_name", arcirk::enum_synonym(tables::tbDocuments)}
                 });

    asyncAwait();
}

void WebSocketClient::synchronizeBaseNext(const arcirk::server::server_response &resp)
{
    qDebug() << __FUNCTION__;


    if(resp.result == "error")
        return;

    using namespace arcirk::database;

    if(!sqlDatabase.isOpen())
        return;

    auto result = nlohmann::json::parse(QByteArray::fromBase64(resp.result.data()).toStdString());

    if(!syncData->running()){
        syncData->setComparisonOfDocuments(result);
        syncOperatiions.start();
    }
}

QString WebSocketClient::get_hash(const QString& first, const QString& second){
    QString _usr = first.trimmed();
    _usr = _usr.toUpper();
    return get_sha1(QString(_usr + second).toUtf8());
}

void WebSocketClient::updateHttpServiceConfiguration()
{
    qDebug() << __FUNCTION__;
    if(isStarted()){
        send_command(arcirk::server::server_commands::HttpServiceConfiguration);
    }
}

QString WebSocketClient::cryptPass(const QString &source, const QString &key)
{
    std::string result = crypt(source.toStdString(), key.toStdString());

    return QString::fromStdString(result);
}

void WebSocketClient::registerDevice()
{
    qDebug() << __FUNCTION__;

    if(wsSettings){
        QUuid devId = QUuid::fromString(wsSettings->deviceId());
        if(devId.isNull()){
            qCritical() << __FUNCTION__ << "Не верный идентификатор устройства!";
            return;
        }

        QString product = wsSettings->product();

        auto record = arcirk::database::devices();
        record.ref = devId.toString(QUuid::StringFormat::WithoutBraces).toStdString();
        record.deviceType = arcirk::enum_synonym(arcirk::database::devices_type::devTablet);
        record.first = product.toStdString();
        record.second = product.toStdString();

        auto j = pre::json::to_json<arcirk::database::devices>(record);

        nlohmann::json struct_query_param = {
            {"table_name", arcirk::enum_synonym(arcirk::database::tables::tbDevices)},
            {"query_type", "update_or_insert"},
            {"values", j}
        };

        std::string query_param = QByteArray::fromStdString(struct_query_param.dump()).toBase64().toStdString();

        send_command(arcirk::server::server_commands::ExecuteSqlQuery, {
                         {"query_param", query_param}
                     });
    }
}

void WebSocketClient::get_barcode_information(const QString &barcode, BarcodeInfo* bInfo, bool skip_data)
{

    if(!wsSettings){
        qCritical() << "Не инициализирован объект настроек!";
        return;
    }

    if(!skip_data){
        if(wsSettings->workplace_options().warehouse.empty() || wsSettings->workplace_options().price_type.empty()){
            const QString err = "Не инициализированы настройки рабочего места!";
            qCritical() << err;
            emit displayError("WebSocketClient::get_barcode_information", err);
            return;
        }

        if(wsSettings->getHttpService().isEmpty()){
            qCritical() << "HTTP сервис 1С предприятия не указан!";
            return;
        }

        QEventLoop loop;
        int httpStatus = 200;
        QByteArray httpData;
        QNetworkAccessManager httpService;

        auto finished = [&loop, &httpStatus, &httpData](QNetworkReply* reply) -> void
        {
           QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
           if(status_code.isValid()){
               httpStatus = status_code.toInt();
               if(httpStatus != 200){
                    qCritical() << __FUNCTION__ << "Error: " << httpStatus << " " + reply->errorString() ;
               }else
               {
                   httpData = reply->readAll();
               }
           }
           loop.quit();

        };

        loop.connect(&httpService, &QNetworkAccessManager::finished, finished);

        QNetworkRequest request(wsSettings->getHttpService() + "/info");
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QString pwd = cryptPass(wsSettings->getHttpPassword(), "my_key");
        QString concatenated = wsSettings->getHttpUser() + ":" + pwd;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        QString headerData = "Basic " + data;
        request.setRawHeader("Authorization", headerData.toLocal8Bit());

        auto bi = arcirk::client::barcode_info();

        //временный код, для совместимости, http серис используется старой весией прайс чекера
    //    nlohmann::json param = {
    //        {"barcode", barcode.toStdString()},
    //        {"command", "InfoFromBarcode"},
    //        {"barcode_info", pre::json::to_json<arcirk::client::barcode_info>(bi)},
    //        {"typePrice", wsSettings->workplace_options().price},
    //        {"stockRef", wsSettings->workplace_options().warehouse},
    //        {"eng", true},
    //        {"byteArray", true}
    //    };

        nlohmann::json param = {
            {"barcode", barcode.toStdString()},
            {"command", "BarcodeInfo"},
            {"barcode_info", pre::json::to_json<arcirk::client::barcode_info>(bi)},
            {"price", wsSettings->workplace_options().price_type},
            {"warehouse", wsSettings->workplace_options().warehouse},
            {"image_data", false} //wsSettings->isShowImage() тормозит
        };

        httpService.post(request, QByteArray::fromStdString(param.dump()));
        loop.exec();

        if(httpStatus != 200){
            return;
        }

        if(httpData.isEmpty())
            return;

        if(httpData == "error"){
             bInfo->clear("Ошибка чтения штрихкода");
             emit displayError("get_barcode_information", "Не верный формат данных.");
             return;
        }

        //bool is_error = false;
        try {
            bInfo->set_barcode_info_object(httpData.toStdString());
        } catch (const std::exception& e) {
            qCritical() << __FUNCTION__ << e.what();
            emit displayError("get_barcode_information", "Не верный формат данных.");
            bInfo->clear("Ошибка чтения штрихкода");
            //is_error = true;
        }
    }else{
        auto barcode_inf =  arcirk::client::barcode_info();
        barcode_inf.barcode = barcode.toStdString();
        bInfo->set_barcode_info(barcode_inf);
    }
//    if(wsSettings->isShowImage() && !is_error){
//        if(!bInfo->uuid().empty()){
//            param = {
//                    {"uuid", bInfo->uuid()},
//                    {"command", "GetImage"}
//                };
//            httpService.post(request, QByteArray::fromStdString(param.dump()));
//            loop.exec();

//            if(httpStatus != 200){
//                return;
//            }

//            if(httpData.isEmpty())
//                return;

//            bInfo->set_image_data(httpData.toStdString());
//        }
//    }
}

void WebSocketClient::get_image_data(BarcodeInfo *bInfo)
{
    if(!wsSettings->isShowImage() || bInfo->uuid().empty())
        return;

    QEventLoop loop;
    int httpStatus = 200;
    QByteArray httpData;
    QNetworkAccessManager httpService;

    auto finished = [&loop, &httpStatus, &httpData](QNetworkReply* reply) -> void
    {
       QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
       if(status_code.isValid()){
           httpStatus = status_code.toInt();
           if(httpStatus != 200){
                qCritical() << __FUNCTION__ << "Error: " << httpStatus << " " + reply->errorString() ;
           }else
           {
               httpData = reply->readAll();
           }
       }
       loop.quit();

    };

    loop.connect(&httpService, &QNetworkAccessManager::finished, finished);

    QNetworkRequest request(wsSettings->getHttpService() + "/info");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString pwd = cryptPass(wsSettings->getHttpPassword(), "my_key");
    QString concatenated = wsSettings->getHttpUser() + ":" + pwd;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    request.setRawHeader("Authorization", headerData.toLocal8Bit());

    nlohmann::json param = {
            {"uuid", bInfo->uuid()},
            {"command", "GetImage"}
        };
    httpService.post(request, QByteArray::fromStdString(param.dump()));
    loop.exec();

    if(httpStatus != 200){
        return;
    }

    if(httpData.isEmpty())
        return;

    //bInfo->set_image_from_base64(httpData.toStdString());
    bInfo->set_image(httpData);


}

void WebSocketClient::checkConnection()
{
    if(!m_reconnect->isActive())
        startReconnect();
}

void WebSocketClient::deleteDocument(const QString &ref, const int ver)
{
    int v = ver;
    if(v < 0)
        v = 0;

    if(!is_offline){
        nlohmann::json param = {
            {"table_name", arcirk::enum_synonym(arcirk::database::tables::tbDocuments)},
            {"query_type", "update"},
            {"values", nlohmann::json{
                 {"deleted_mark", 1},
                 {"version", v + 1}
             }},
            {"where_values", nlohmann::json{{"ref", ref.toStdString()}}}
        };

        std::string query_param = QByteArray::fromStdString(param.dump()).toBase64().toStdString();
        send_command(arcirk::server::server_commands::ExecuteSqlQuery, {
                         {"query_param", query_param}
                     });
    }else{
        if(!sqlDatabase.isOpen())
            return;

        using namespace arcirk::database;
        nlohmann::json result{};
        auto br = builder::query_builder();
        br.use(nlohmann::json{
                   {"deleted_mark", 1},
                   {"version", v + 1}
               });

        QSqlQuery rs;
        rs.exec(br.update(arcirk::enum_synonym(tables::tbDocuments), true)
        .where(nlohmann::json{
               {"ref", ref.toStdString()}
           }, true).prepare().c_str());

    }
}

void WebSocketClient::getDocuments()
{
    if(!is_offline){
        nlohmann::json param = {
            {"table_name", arcirk::enum_synonym(arcirk::database::tables::tbDocuments)},
            {"query_type", "select"},
            {"values", nlohmann::json{}},
            {"where_values", nlohmann::json{
                 {"device_id", wsSettings->deviceId().toStdString()},
                 {"deleted_mark", 0}
             }}
        };

        std::string query_param = QByteArray::fromStdString(param.dump()).toBase64().toStdString();
        send_command(arcirk::server::server_commands::ExecuteSqlQuery, {
                         {"query_param", query_param}
                     });
    }else{
        if(!sqlDatabase.isOpen())
            return;

        using namespace arcirk::database;
        nlohmann::json result{};
        arcirk::database::execute(
                    builder::query_builder().select(nlohmann::json{"*"})
                    .from(arcirk::enum_synonym(tables::tbDocuments))
                    .where(nlohmann::json{
                               {"device_id", wsSettings->deviceId().toStdString()},
                               {"deleted_mark", 0}
                            }, true).prepare(), sqlDatabase, result);
        emit readDocuments(QString::fromStdString(result.dump()));
    }

}

void WebSocketClient::getDocumentInfo(const QString &ref)
{
    if(!is_offline){
        nlohmann::json param = {
            {"table_name", arcirk::enum_synonym(arcirk::database::tables::tbDocuments)},
            {"query_type", "select"},
            {"values", nlohmann::json{}},
            {"where_values", nlohmann::json{
                 {"ref", ref.toStdString()}
             }}
        };

        std::string query_param = QByteArray::fromStdString(param.dump()).toBase64().toStdString();
        send_command(arcirk::server::server_commands::ExecuteSqlQuery, {
                         {"query_param", query_param}
                     });
    }else{
        if(!sqlDatabase.isOpen())
            return;

        using namespace arcirk::database;
        nlohmann::json result{};
        arcirk::database::execute(
                    builder::query_builder().select(nlohmann::json{"*"})
                    .from(arcirk::enum_synonym(tables::tbDocuments))
                    .where(nlohmann::json{
                                               {"ref", ref.toStdString()}
                                           }, true).prepare(), sqlDatabase, result);
        emit readDocument(QString::fromStdString(result.dump()));
    }
}

void WebSocketClient::getDocumentContent(const QString &ref)
{
    if(!is_offline){
        nlohmann::json param = {
            {"table_name", arcirk::enum_synonym(arcirk::database::tables::tbDocumentsTables)},
            {"query_type", "select"},
            {"values", nlohmann::json{}},
            {"where_values", nlohmann::json{
                 {"parent", ref.toStdString()}
             }}
        };

        std::string query_param = QByteArray::fromStdString(param.dump()).toBase64().toStdString();
        send_command(arcirk::server::server_commands::ExecuteSqlQuery, {
                         {"query_param", query_param}
                     });
    }else{
        if(!sqlDatabase.isOpen())
            return;

        using namespace arcirk::database;
        nlohmann::json result{};
        arcirk::database::execute(
                    builder::query_builder().select(nlohmann::json{"*"})
                    .from(arcirk::enum_synonym(tables::tbDocumentsTables))
                    .where(nlohmann::json{
                                               {"parent", ref.toStdString()}
                                           }, true).prepare(), sqlDatabase, result);

        if(result["columns"].size() == 1){
            auto table_info = arcirk::database::table_info(sqlDatabase, arcirk::database::tables::tbDocumentsTables);
            //nlohmann::json cols{};
            for(auto itr = table_info.begin(); itr != table_info.end();  ++itr){
                //cols += itr.key();
                result["columns"] += itr.key();
            }
            //result["columns"] = cols;
        }
        emit readDocumentTable(QString::fromStdString(result.dump()));
    }
}

QString WebSocketClient::documentDate(const int value) const
{
    if(value > 0){
        return QDateTime::fromSecsSinceEpoch(value).toString("dd.MM.yyyy hh:mm:ss");
    }else{
        return {};
    }
}

void WebSocketClient::addDocument(const QString &number, const int date, const QString &comment)
{

}

void WebSocketClient::documentContentUpdate(const QString &barcode, const int quantity, const QString& parent, const QString &ref, QJsonTableModel* model)
{
    if(barcode.isEmpty() || parent.isEmpty())
        return;

    auto row = arcirk::database::document_table();
    row.barcode = barcode.toStdString();
    if(ref.isEmpty())
        row.ref = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    else
        row.ref = ref.toStdString();
    row.parent = parent.toStdString();
    row.quantity = quantity;
    row.first = "Штрихкоды";
    row.second = "Штрихкоды";

    auto Index = model->getColumnIndex("barcode");
    auto IndexVer = model->getColumnIndex("version");
    int version = 0;
    QModelIndex bIndex{};
    if(Index != -1){
        bIndex = model->findInTable(barcode, Index, false);
        if(bIndex.isValid()){
            version = model->data(bIndex, Qt::UserRole + IndexVer).toInt() + 1;
        }
    }

    row.version = version;

    if(!is_offline){
        nlohmann::json param = {
            {"table_name", arcirk::enum_synonym(arcirk::database::tables::tbDocumentsTables)},
            {"query_type", "update_or_insert"},
            {"values", pre::json::to_json(row)}
        };

        std::string query_param = QByteArray::fromStdString(param.dump()).toBase64().toStdString();
        send_command(arcirk::server::server_commands::ExecuteSqlQuery, {
                         {"query_param", query_param}
                     });
    }else{
        if(!sqlDatabase.isOpen()){
            qCritical() << "Нет подключения к базе данных!";
            return;
        }

        using namespace arcirk::database;

        int count = 0;
        QSqlQuery rs(builder::query_builder().row_count().from(arcirk::enum_synonym(arcirk::database::tables::tbDocumentsTables)).where(nlohmann::json{{"ref", row.ref}}, true).prepare().c_str());
        while (rs.next()) {
            count += rs.value(0).toInt();
        }
        rs.clear();
        auto br = builder::query_builder();
        br.use(pre::json::to_json(row));
        if(count > 0){
            rs.exec(br.update(arcirk::enum_synonym(arcirk::database::tables::tbDocumentsTables), true).where(nlohmann::json{{"ref", row.ref}}, true).prepare().c_str());
        }else{
            rs.exec(br.insert(arcirk::enum_synonym(arcirk::database::tables::tbDocumentsTables), true).prepare().c_str());
        }

        rs.clear();
        br.clear();
        rs.exec(br.select({"*"}).from(arcirk::enum_synonym(arcirk::database::tables::tbDocuments)).where(nlohmann::json{{"ref", row.parent}}, true).prepare().c_str());
        int version = 1;
        while (rs.next()) {
            version += rs.value("version").toInt();
        }
        rs.clear();
        br.clear();
        br.use(nlohmann::json{
                   {"version", version}
               });
        rs.exec(br.update(arcirk::enum_synonym(arcirk::database::tables::tbDocuments), true).where(nlohmann::json{{"ref", row.parent}}, true).prepare().c_str());

    }


    if(Index != -1){
        auto bIndex = model->findInTable(barcode, Index, false);
        if(!bIndex.isValid()){
            //model->addRow(QString::fromStdString(pre::json::to_json(row).dump()));
            model->insertRow(0, QString::fromStdString(pre::json::to_json(row).dump()));
        }else{
            model->updateRow(barcode, quantity, bIndex.row());
            model->moveTop(bIndex.row());
        }
        //if(model->currentRow() != 0){
            model->setCurrentRow(0);
            model->reset();
//        }else
//            model->dataChanged(bIndex, bIndex);
    }
}

void WebSocketClient::documentUpdate(const QString &number, const QString &date, const QString& comment, const QString& source)
{

    auto source_ = arcirk::database::table_default_struct<arcirk::database::documents>(arcirk::database::tbDocuments);

    try {
        source_ = pre::json::from_json<arcirk::database::documents>(nlohmann::json::parse(source.toStdString()));
    } catch (...) {
        source_ = arcirk::database::table_default_struct<arcirk::database::documents>(arcirk::database::tbDocuments);
    }

    nlohmann::json cache{};

    try {
        cache = nlohmann::json::parse(source_.cache);
    } catch (...) {
    }

    if(source_.ref == arcirk::uuids::nil_string_uuid() || source_.ref.empty()){
        source_.ref = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
        source_.first = "ПодборШтрихкодов";
        source_.second = "Подбор штрихкодов";
        source_.xml_type = "DocumentRef.ПодборШтрихкодов";
        source_.device_id = wsSettings->deviceId().toStdString();
    }

    if(cache.is_discarded()){
        cache = {
            {"comment", comment.toStdString()}
        };
    }else{
        cache["comment"] = comment.toStdString();
    }

    source_.number = number.toStdString();

    auto dt = QDateTime::fromString(date, "dd.MM.yyyy hh:mm:ss");
    source_.date = dt.toSecsSinceEpoch();// + dt.offsetFromUtc();
    source_.cache = cache.dump();
    source_.version++;

    if(!is_offline){
        nlohmann::json param = {
            {"table_name", arcirk::enum_synonym(arcirk::database::tables::tbDocuments)},
            {"query_type", "update_or_insert"},
            {"values", pre::json::to_json(source_)}
        };

        std::string query_param = QByteArray::fromStdString(param.dump()).toBase64().toStdString();
        send_command(arcirk::server::server_commands::ExecuteSqlQuery, {
                         {"query_param", query_param}
                     });
    }else{
        if(!sqlDatabase.isOpen()){
            qCritical() << __FUNCTION__  << "База данных не подключена!";
            return;
        }

        using namespace arcirk::database;
        auto query = builder::query_builder();
        int count = 0;
        QSqlQuery rs(query.row_count().from(arcirk::enum_synonym(tables::tbDocuments)).where(nlohmann::json{
                                                                                             {"ref", source_.ref}
                                                                                             }, true).prepare().c_str());
        while (rs.next()) {
            count += rs.value(0).toInt();
        }
        query.clear();
        query.use(pre::json::to_json(source_));
        if(count == 0){
            query.insert(arcirk::enum_synonym(tables::tbDocuments), true);
        }else{
            query.update(arcirk::enum_synonym(tables::tbDocuments), true).where(nlohmann::json{
                                                                                     {"ref", source_.ref}
                                                                                     }, true);
        }
        rs.clear();
        rs.exec(query.prepare().c_str());

        if(rs.lastError().isValid())
            qCritical() << __FUNCTION__ << rs.lastError().text();

        getDocuments();
    }
}

QString WebSocketClient::documentGenerateNewNumber(const int id)
{
    return QString("%1").arg(id, 9, 'g', -1, '0');
}

void WebSocketClient::removeRecord(const QString &ref, QJsonTableModel* model)
{
    if(!sqlDatabase.isOpen()){
        qCritical() << __FUNCTION__  << "База данных не подключена!";
        return;
    }

    using namespace arcirk::database;
    auto query = builder::query_builder();

    QSqlQuery rs;
    rs.exec(query.remove().from(arcirk::enum_synonym(tables::tbDocumentsTables)).where(nlohmann::json{
                                                                                           {"ref", ref.toStdString()}
                                                                                           }, true).prepare().c_str());
    auto index = model->getColumnIndex("ref");
    auto v_index = model->getColumnIndex("parent");
    auto nIndex = model->findInTable(ref, index, false);

    if(v_index != -1){
        if(nIndex.isValid()){

            QString parent = model->data(nIndex, Qt::UserRole + v_index).toString();
            rs.clear();
            query.clear();
            rs.exec(query.select({"*"}).from(arcirk::enum_synonym(arcirk::database::tables::tbDocuments)).where(nlohmann::json{{"ref", parent.toStdString()}}, true).prepare().c_str());
            int version = 1;
            while (rs.next()) {
                version += rs.value("version").toInt();
            }
            rs.clear();
            query.clear();
            query.use(nlohmann::json{
                          {"version", version}
                      });
            rs.exec(query.update(arcirk::enum_synonym(tables::tbDocuments), true).where(nlohmann::json{
                                                                                      {"ref", parent.toStdString()}
                                                                                  }, true).prepare().c_str());
        }
    }
    if(index != -1){
        //auto nIndex = model->findInTable(ref, index, false);
        if(nIndex.isValid()){
            model->removeRow(nIndex.row());
        }
    }

}

void WebSocketClient::debugViewTime()
{
    qDebug() << __FUNCTION__ << QTime::currentTime();
}

void WebSocketClient::syncDataCreateConnections()
{
    connect(&syncOperatiions, &QThread::started, syncData, &SyncData::run);
    connect(syncData, &SyncData::finished, &syncOperatiions, &QThread::terminate);
    connect(syncData, &SyncData::endSynchronize, this, &WebSocketClient::onEndSynchronize);
    syncData->moveToThread(&syncOperatiions);
}
