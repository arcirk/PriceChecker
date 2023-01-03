#include "include/websockets.hpp"


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
            }else{
                emit displayError("SetClientParam", "Ошибка авторизации");
            }
        }
    } catch (std::exception& e) {
        emit displayError("parse_response", e.what());
    }
}

QString WebSocketClient::get_hash(const QString& first, const QString& second){
    QString _usr = first.trimmed();
    _usr = _usr.toUpper();
    return get_sha1(QString(_usr + second).toUtf8());
}
