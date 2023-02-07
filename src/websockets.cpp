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
}

WebSocketClient::~WebSocketClient()
{
    if(isStarted())
        m_client->close();
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

//QString WebSocketClient::typePriceRef() const
//{
//    return QString::fromStdString(wsSettings.workplace_options().);
//}

//QString WebSocketClient::stockRef() const
//{
//    return QString::fromStdString(chk_conf.stockRef);
//}

//void WebSocketClient::setTypePriceRef(const QString &value)
//{
//    chk_conf.typePriceRef = value.toStdString();
//}

//void WebSocketClient::setStockRef(const QString &value)
//{
//    chk_conf.stockRef = value.toStdString();
//}

//QString WebSocketClient::typePrice() const
//{
//    return QString::fromStdString(chk_conf.typePrice);
//}

//QString WebSocketClient::stock() const
//{
//    return QString::fromStdString(chk_conf.stock);
//}

//void WebSocketClient::setTypePrice(const QString &value)
//{
//    chk_conf.typePrice = value.toStdString();
//}

//void WebSocketClient::setStock(const QString &value)
//{
//    chk_conf.stock = value.toStdString();
//}

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
                m_async_await.append(std::bind(&WebSocketClient::synchronizeBase, this));
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
        asyncAwait();
        return;
    }

    //1.) Получаем список локальных документов
    QSqlQuery rs;
    rs.exec("select ref,version from Documents");
    nlohmann::json t_docs{};
    while (rs.next()) {
        t_docs += nlohmann::json{
            {"ref", rs.value("ref").toString().toStdString()},
            {"version", rs.value("version").toInt()}
        };
    };

//    nlohmann::json query_param{};
//    query_param["base64_param"] = t_docs;

//    nlohmann::json query_param = {
//        {"table_name", "DevicesView"},
//        {"query_type", "select"},
//        {"where_values", nlohmann::json({
//             {"ref", devId.toString(QUuid::StringFormat::WithoutBraces).toStdString()}
//         })}
//    };

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
    auto objects = result.value("objects", nlohmann::json{});
    auto comparison_table = result.value("comparison_table", nlohmann::json{});
    std::map<std::string, std::pair<int,int>> m_ext_table;
    std::vector<std::string> m_vec_new_documents;
    if (comparison_table.is_array() && !comparison_table.empty()) {
        for (auto itr = comparison_table.begin(); itr != comparison_table.end(); ++itr) {
            nlohmann::json r = *itr;
            m_ext_table.emplace(r["ref"], std::pair<int,int>(r["ver1"], r["ver2"]));
            if(r["ver1"] < r["ver2"]){
                m_vec_new_documents.push_back(r["ref"]);
            }
        }
    }

    if(objects.is_array()){
        //sqlDatabase.transaction();
        for (auto const & itr : objects) {
            if(itr.is_object()){
                auto items = itr.items();
                for (auto const & itr : items) {
                    //qDebug() << QString::fromStdString(itr.key());
                    auto std_attr = pre::json::from_json<documents>(itr.value()["object"]["StandardAttributes"]);
                    auto query = builder::query_builder();
                    query.use(pre::json::to_json(std_attr));
                    QSqlQuery q;
                    auto d_itr = m_ext_table.find(std_attr.ref);
                    QString query_str;
                    if(d_itr != m_ext_table.end()){
                        if(d_itr->second.second != -1){
                            query_str = QString::fromStdString(query.remove().from(arcirk::enum_synonym(tbDocuments)).where(nlohmann::json{
                                                                                                                                        {"ref", std_attr.ref}
                                                                                                                                    }, true).prepare());
                            q.exec(query_str);
                            if(q.lastError().isValid())
                                qCritical() << q.lastError().text();
                        }

                    }

                    query_str = QString::fromStdString(query.insert(arcirk::enum_synonym(tbDocuments), true).prepare());
                    //qDebug() << qPrintable(query_str);
                    q.exec(query_str);
                    if(q.lastError().isValid())
                        qCritical() << q.lastError().text();

                    auto m_rows = itr.value()["object"]["TabularSections"];
                    if(m_rows.is_array()){
                        for (auto const & tbl : m_rows) {
                            query.clear();
                            if(tbl.is_object()){
                                query.use(tbl);
                                query_str = QString::fromStdString(query.remove().from(arcirk::enum_synonym(tbDocumentsTables)).where(nlohmann::json{
                                                                                                                                          {"parent", std_attr.ref}
                                                                                                                                      }, true).prepare());
                                q.exec(query_str);
                                if(q.lastError().isValid())
                                    qCritical() << q.lastError().text();
                                query_str = QString::fromStdString(query.insert(arcirk::enum_synonym(tbDocumentsTables), true).prepare());
                                q.exec(query_str);
                                if(q.lastError().isValid())
                                    qCritical() << q.lastError().text();
                            }
                        }
                    }
                }
            }
        }
        //sqlDatabase.commit();
    }else if(objects.is_object()){
        auto items = objects.items();
        for (auto const & itr : items) {
            qDebug() << QString::fromStdString(itr.key());
        }
    }

    //Выгружаем на севрвер локальные документы с версией выше чем на сервере
    if(m_vec_new_documents.size() > 0){
        foreach (auto const& itr, m_vec_new_documents) {

        }
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

void WebSocketClient::get_barcode_information(const QString &barcode, BarcodeInfo* bInfo)
{

    if(!wsSettings){
        qCritical() << "Не инициализирован объект настроек!";
        return;
    }

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

    //bool is_error = false;
    try {
        bInfo->set_barcode_info_object(httpData.toStdString());
    } catch (const std::exception& e) {
        qCritical() << __FUNCTION__ << e.what();
        emit displayError("get_barcode_information", "Не верный формат данных.");
        bInfo->clear("Ошибка чтения штрихкода");
        //is_error = true;
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

void WebSocketClient::getDocuments()
{
    if(!is_offline){
        nlohmann::json param = {
            {"table_name", arcirk::enum_synonym(arcirk::database::tables::tbDocuments)},
            {"query_type", "select"},
            {"values", nlohmann::json{}},
            {"where_values", nlohmann::json{{"device_id", wsSettings->deviceId().toStdString()}}}
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
                                               {"device_id", wsSettings->deviceId().toStdString()}
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
        if(!sqlDatabase.isOpen())
            return;

        using namespace arcirk::database;

        int count = -0;
        QSqlQuery rs(builder::query_builder().row_count().from(arcirk::enum_synonym(arcirk::database::tables::tbDocumentsTables)).where(nlohmann::json{{"ref", row.ref}}, true).prepare().c_str());
        while (rs.next()) {
            count++;
        }
        rs.clear();
        auto br = builder::query_builder();
        br.use(pre::json::to_json(row));
        if(count > 0){
            rs.exec(br.update(arcirk::enum_synonym(arcirk::database::tables::tbDocumentsTables), true).where(nlohmann::json{{"ref", row.ref}}, true).prepare().c_str());
        }else{
            rs.exec(br.insert(arcirk::enum_synonym(arcirk::database::tables::tbDocumentsTables), true).prepare().c_str());
        }
    }


    if(Index != -1){
        auto bIndex = model->findInTable(barcode, Index, false);
        if(!bIndex.isValid()){
            model->addRow(QString::fromStdString(pre::json::to_json(row).dump()));
        }else{
            model->updateRow(barcode, quantity, bIndex.row());
        }
        model->reset();
    }
}

void WebSocketClient::documentUpdate(const QString &number, const QString &date, const QString comment, const QString source)
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

    nlohmann::json param = {
        {"table_name", arcirk::enum_synonym(arcirk::database::tables::tbDocuments)},
        {"query_type", "update_or_insert"},
        {"values", pre::json::to_json(source_)}
    };

    std::string query_param = QByteArray::fromStdString(param.dump()).toBase64().toStdString();
    send_command(arcirk::server::server_commands::ExecuteSqlQuery, {
                     {"query_param", query_param}
                 });

}

QString WebSocketClient::documentGenerateNewNumber(const int id)
{
    return QString("%1").arg(id, 9, 'g', -1, '0');
}

void WebSocketClient::initSyncData()
{
    connect(&syncOperatiions, &QThread::started, &syncData, &SyncData::run);
    connect(&syncData, &SyncData::finished, &syncOperatiions, &QThread::terminate);
    syncData.moveToThread(&syncOperatiions);
}
