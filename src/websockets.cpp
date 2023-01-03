#include "include/websockets.hpp"

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

QString WebSocketClient::typePriceRef() const
{
    return QString::fromStdString(chk_conf.typePriceRef);
}

QString WebSocketClient::stockRef() const
{
    return QString::fromStdString(chk_conf.stockRef);
}

void WebSocketClient::setTypePriceRef(const QString &value)
{
    chk_conf.typePriceRef = value.toStdString();
}

void WebSocketClient::setStockRef(const QString &value)
{
    chk_conf.stockRef = value.toStdString();
}

QString WebSocketClient::typePrice() const
{
    return QString::fromStdString(chk_conf.typePrice);
}

QString WebSocketClient::stock() const
{
    return QString::fromStdString(chk_conf.stock);
}

void WebSocketClient::setTypePrice(const QString &value)
{
    chk_conf.typePrice = value.toStdString();
}

void WebSocketClient::setStock(const QString &value)
{
    chk_conf.stock = value.toStdString();
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
    param.app_name = "PriceCjecker";
    param.host_name = QSysInfo::machineHostName().toStdString();
    param.system_user = "root";

    if(wsSettings){
        param.user_name = wsSettings->userName().toStdString();
        param.hash = wsSettings->hash().toStdString();
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
    try {
        auto msg = pre::json::from_json<arcirk::server::server_response>(resp.toStdString());
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
        }
    } catch (std::exception& e) {
        emit displayError("parse_response", e.what());
    }
}

void WebSocketClient::send_command(arcirk::server::server_commands cmd, const nlohmann::json &param)
{
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
        if(resp.message == "OK" && !table_name.empty()){

            nlohmann::json tb_name = table_name;
            if(tb_name.get<arcirk::database::tables>() == arcirk::database::tables::tbDevices){
                emit notify("Устройство успешно зарегистрировано!");
            }
        }

    } catch (const std::exception& e) {
        qCritical() << e.what();
    }
}

QString WebSocketClient::get_hash(const QString& first, const QString& second){
    QString _usr = first.trimmed();
    _usr = _usr.toUpper();
    return get_sha1(QString(_usr + second).toUtf8());
}

void WebSocketClient::updateHttpServiceConfiguration()
{
    if(isStarted()){
        send_command(arcirk::server::server_commands::HttpServiceConfiguration);
    }
}

QString WebSocketClient::crypt(const QString &source, const QString &key)
{
    #define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))
    std::string _source = source.toStdString();
    std::string _key = source.toStdString();
    void * text = (void *) _source.c_str();
    void * pass = (void *) _key.c_str();

    _crypt(text, ARR_SIZE(_source.c_str()), pass, ARR_SIZE(_key.c_str()));

    std::string result((char*)text);

    return QString::fromStdString(result);
}

void WebSocketClient::registerDevice()
{
    if(wsSettings){
        QUuid devId = QUuid::fromString(wsSettings->deviceId());
        if(devId.isNull()){
            qCritical() << __FUNCTION__ << "Не верный идентификатор устройства!";
            return;
        }

        QString product = wsSettings->product();

        auto record = arcirk::database::devices();
        record.ref = devId.toString(QUuid::StringFormat::WithoutBraces).toStdString();
        record.deviceType = "AndroidTablet";
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
