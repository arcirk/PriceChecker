#include "include/wsSettings.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QSysInfo>


namespace arcirk{

    Settings::Settings(QObject *parent)
        : QObject{parent}
    {

        read_private_conf();
        if(client_conf.deviceId.empty())
            init_device_id();
        else{
            m_device_id = QUuid::fromString(QString::fromStdString(client_conf.deviceId));
        }
        m_product = QSysInfo::prettyProductName();
        read_conf();



//        auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
//        QDir dir(path);
//        if(!dir.exists())
//            dir.mkpath(path);

//        auto fileName= path + "/price_checker_conf.json";

//        qDebug() << __FUNCTION__ << fileName;

//        //m_device_id = QSysInfo::machineUniqueId();
//        m_product = QSysInfo::prettyProductName();

//        if(QFile::exists(fileName)){
//            QFile f(fileName);
//            f.open(QIODevice::ReadOnly);
//            auto doc = QJsonDocument::fromJson(f.readAll());
//            auto obj = doc.object();
//            f.close();
//            m_host =        obj.value("host").toString();
//            m_userName =    obj.value("userName").toString();
//            m_hash =        obj.value("hash").toString();
//            m_port =        obj.value("port").toInt();
//            //m_device_id =   obj.value("device_id").toString();
//            //m_product =     obj.value("product").toString();
//            m_httpService = obj.value("httpService").toString();
//            m_httpPwd =     obj.value("httpPwd").toString();
//            if(m_product.isEmpty())
//                m_product = QSysInfo::prettyProductName();
//            m_keyboardInputMode = obj.value("keyboardInputMode").toBool();
//            //m_priceCheckerMode = obj.value("priceCheckerMode").toBool();
//            //if(!m_priceCheckerMode)
//            m_priceCheckerMode = true; //в этом проекте всегда истина
//            m_showImage = obj.value("showImage").toBool();

//        }else{
//            m_host = "ws://localhost";
//            m_port = 8080;
//            m_userName = "IIS_1C";
//            m_hash = "";
//            m_httpService = "http://localhost/trade/hs/http_trade";
//        }

//        if(m_device_id.isEmpty()){
//            m_device_id = QUuid::createUuid().toString();
//        }

//        if(m_device_id.length() > 36)
//            m_device_id.replace("{", "").replace("}","");

//        if(m_userName.isEmpty())
//            m_userName = "IIS_1C";

    }

    void Settings::read_conf(){

        qDebug() << __FUNCTION__;

        conf.ServerHost = "127.0.0.1";
        conf.ServerPort = 8080;
        conf.ServerUser = "IIS_1C";
        conf.HSHost = "http://localhost/trade/hs/http_trade";
        conf.Version = ARCIRK_VERSION;

        auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(path);
        if(!dir.exists())
            dir.mkpath(path);

        auto fileName= path + "/price_checker_conf.json";
        if(QFile::exists(fileName)){

            QFile f(fileName);
            f.open(QIODevice::ReadOnly);

            std::string m_text = f.readAll().toStdString();
            f.close();

            try {
                conf = pre::json::from_json<server::server_config>(m_text);
            } catch (std::exception& e) {
                qCritical() << __FUNCTION__ << e.what();
            }
        }
    }

    void Settings::read_private_conf(){

        qDebug() << __FUNCTION__;

        client_conf.showImage = false;
        client_conf.keyboardInputMode = true;
        client_conf.priceCheckerMode = true;

        auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(path);
        if(!dir.exists())
            dir.mkpath(path);

        auto fileName= path + "/price_checker_private_conf.json";
        if(QFile::exists(fileName)){
            QFile f(fileName);
            f.open(QIODevice::ReadOnly);

            std::string m_text = f.readAll().toStdString();
            f.close();

            try {
                client_conf = pre::json::from_json<client::client_private_config>(m_text);
            } catch (std::exception& e) {
                qCritical() << __FUNCTION__ << e.what();
            }
        }
    }

    void Settings::write_conf(){
        try {
            std::string result = pre::json::to_json(conf).dump() ;
            auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            auto fileName= path + "/price_checker_conf.json";
            QFile f(fileName);
            qDebug() << fileName;
            f.open(QIODevice::WriteOnly);
            f.write(QByteArray::fromStdString(result));
            f.close();
        } catch (std::exception& e) {
            qCritical() << __FUNCTION__ << e.what();
        }
    }

    void Settings::write_private_conf(){
        try {
            client_conf.deviceId = deviceId().toStdString();
            std::string result = pre::json::to_json(client_conf).dump() ;
            auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            auto fileName= path + "/price_checker_private_conf.json";
            qDebug() << fileName;
            QFile f(fileName);
            f.open(QIODevice::WriteOnly);
            f.write(QByteArray::fromStdString(result));
            f.close();
        } catch (std::exception& e) {
            qCritical() << __FUNCTION__ << e.what();
        }
    }

    void Settings::init_device_id(){
        QString m_id = QSysInfo::machineUniqueId();
        if(m_id.isEmpty())
            m_device_id = QUuid::createUuid();
        else{
           m_device_id = QUuid::fromString(m_id);
        }
        client_conf.deviceId = m_device_id.toString().toStdString();
    }

    QString Settings::host() const
    {
        return QString::fromStdString(conf.ServerHost);//m_host;
    }

    int Settings::port()
    {
        return conf.ServerPort;//m_port;
    }

    QString Settings::userName() const
    {
        return QString::fromStdString(conf.ServerUser);//m_userName;
    }

    QString Settings::hash() const
    {
        return QString::fromStdString(conf.ServerUserHash);//m_hash;
    }

    void Settings::setHost(const QString &value)
    {
        //m_host = value;
        conf.ServerHost = value.toStdString();
    }

    void Settings::setPort(int value)
    {
        //m_port = value;
        conf.ServerPort = value;
    }

    void Settings::setUserName(const QString &value)
    {
        //m_userName = value;
        conf.ServerUser = value.toStdString();
    }

    void Settings::setHash(const QString &value)
    {
        //m_hash = value;
        conf.ServerUserHash = value.toStdString();
    }

    void Settings::setProduct(const QString &value)
    {
        m_product = value;
    }

    QUrl Settings::url() const
    {
        QUrl _url;
        _url.setHost(QString::fromStdString(conf.ServerHost));
        _url.setPort(conf.ServerPort);
        _url.setScheme(QString::fromStdString(conf.ServerProtocol));
        return _url;
//        QString  _url = QString::fromStdString(conf.ServerHost);//m_host;
//        QString _prot = QString::fromStdString(conf.ServerProtocol);
//        if(conf.ServerPort > 0)
//            _url = _prot + "://" + _url + ":" + QString::number(conf.ServerPort);

//        return QUrl(_url);
    }

    void Settings::setUrl(const QString &url)
    {
        QUrl _url(url);
        conf.ServerHost = _url.host().toStdString();
        conf.ServerPort = _url.port();
        conf.ServerProtocol = _url.scheme().toStdString();
//        QStringList lst = url.split(":");
//        if(lst.size() == 1)
//        {
//            conf.ServerHost = QString::frolst[0];

//        }else if(lst.size() == 2){
//            m_host = "ws:" + lst[1];
//        }else if(lst.size() == 3){
//            m_host = "ws:" + lst[1];
//            m_port = lst[2].toInt();
//        }
    }

    void Settings::save()
    {
        qDebug() << __FUNCTION__;

        write_conf();
        write_private_conf();
//        auto obj = QJsonObject();
//        obj.insert("host", m_host);
//        obj.insert("userName", m_userName);
//        obj.insert("hash", m_hash);
//        obj.insert("port", m_port);
//        obj.insert("device_id", m_device_id);
//        obj.insert("product", m_product);
//        obj.insert("httpService", m_httpService);
//        obj.insert("httpPwd", m_httpPwd);
//        obj.insert("keyboardInputMode", m_keyboardInputMode);
//        obj.insert("priceCheckerMode", m_priceCheckerMode);
//        obj.insert("showImage", m_showImage);

//        auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
//        auto fileName= path + "/price_checker_conf.json";
//        QFile f(fileName);
//        f.open(QIODevice::WriteOnly);
//        f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
//        f.close();
    }

    QString Settings::product() const
    {
        return m_product;
    }

    bool Settings::autoConnect() const
    {
        return conf.AutoConnect;
    }

    void Settings::update_workplace_data(const nlohmann::json &object)
    {
        try {
            m_workplace = pre::json::from_json<arcirk::database::devices>(object);
        } catch (const std::exception& e) {
            qCritical() << __FUNCTION__ << e.what();
        }

    }

    void Settings::update_workplace_view(const nlohmann::json &object)
    {
        try {
            m_workplace_view = pre::json::from_json<arcirk::database::devices_view>(object);
            emit updateWorkplaceView(QString::fromStdString(m_workplace_view.organization),
                                     QString::fromStdString(m_workplace_view.subdivision),
                                     QString::fromStdString(m_workplace_view.warehouse),
                                     QString::fromStdString(m_workplace_view.price));

        } catch (const std::exception& e) {
            qCritical() << __FUNCTION__ << e.what();
        }
    }

    arcirk::database::devices &Settings::workplace_options()
    {
        return m_workplace;
    }

    QString Settings::getHttpService() const
    {
        return httpService();
    }

    QString Settings::getHttpPassword() const
    {
        return httpPwd();
    }

    QString Settings::getHttpUser()
    {
        QString usr = httpUser();
        if(usr.isEmpty()){
            usr = userName();
            setHttpUser(usr);
            save();
        }
        return usr;
    }

    bool Settings::isShowImage()
    {
        return showImage();
    }

    QString Settings::deviceId() const
    {
        return m_device_id.toString(QUuid::WithoutBraces);
    }

    void Settings::setDeviceId(const QString &device_id)
    {
        m_device_id = QUuid(device_id);
    }

    void Settings::setHttpService(const QString &value)
    {
        //m_httpService = value;
        conf.HSHost = value.toStdString();
    }

    void Settings::setHttpPwd(const QString &value)
    {
        //m_httpPwd = value;
        conf.HSPassword = value.toStdString();
    }

    void Settings::setHttpUser(const QString &value)
    {
        conf.HSUser = value.toStdString();
    }

    void Settings::setKeyboardInputMode(bool value)
    {
        client_conf.keyboardInputMode = value;
    }

    void Settings::setPriceCheckerMode(bool value)
    {
       client_conf.priceCheckerMode = value;
    }

    bool Settings::keyboardInputMode()
    {
        return client_conf.keyboardInputMode;
    }

    bool Settings::priceCheckerMode()
    {
        return client_conf.priceCheckerMode;
    }

    void Settings::setShowImage(bool value)
    {
        client_conf.showImage = value;
    }

    void Settings::setAutoConnect(bool value)
    {
        conf.AutoConnect = value;
    }

    bool Settings::showImage()
    {
        return client_conf.showImage;// m_showImage;
    }

    QString Settings::httpService() const
    {
        return QString::fromStdString(conf.HSHost); //m_httpService;
    }

    QString Settings::httpPwd() const
    {
        return QString::fromStdString(conf.HSPassword); //m_httpPwd;
    }

    QString Settings::httpUser() const
    {
        return QString::fromStdString(conf.HSUser);
    }


}
