#include "include/arcirk.hpp"

namespace arcirk {

QString get_sha1(const QByteArray& p_arg){
    auto sha = QCryptographicHash::hash(p_arg, QCryptographicHash::Sha1);
    return sha.toHex();
}

QString get_hash(const QString& first, const QString& second){
    QString _usr = first.trimmed();
    _usr = _usr.toUpper();
    return get_sha1(QString(_usr + second).toUtf8());
}

QUuid nil_uuid(){
    return {};
}

QString nil_string_uuid(){
    return QUuid().toString();
}

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

QString crypt(const std::string &source, const std::string& key) {

    #define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))
    void * text = (void *) source.c_str();
    void * pass = (void *) key.c_str();
    _crypt(text, ARR_SIZE(source.c_str()), pass, ARR_SIZE(key.c_str()));

    std::string result((char*)text);

    return QString::fromStdString(result);
}

_ws_message::_ws_message()
       : uuid(nil_uuid())
       , name("noname")
       , uuid_channel(nil_uuid())
       , message("")
       , uuid_form(nil_uuid())
       , command("")
       , hash("")
       , app_name("unknown")
       , result("ok")
       , role("user")
       , presentation("noname")
       , uuid_user(nil_uuid())
       , contentType("HTML")
       , object_name("")
       , msg_ref(nil_string_uuid())
       , ip_address("0.0.0.0")
       , host_name(QSysInfo::machineHostName())
       , user_name("")
       , device_id("")
{
}

void _ws_message::set_value(const QString& nameField, QVariant& val){

   setFun f = get_set_function(nameField);
       if (f)
           f(val);
   }

   void _ws_message::set_uuid(QVariant& val){
       if (val.type() == QVariant::Type::Uuid)
           uuid = val.toUuid();
   }
   void _ws_message::set_uuid_user(QVariant& val){
       if (val.type() == QVariant::Type::Uuid)
           uuid_user = val.toUuid();
   }
   void _ws_message::set_uuid_form(QVariant& val){
       if (val.type() == QVariant::Type::Uuid)
           uuid_form = val.toUuid();
   }
   void _ws_message::set_uuid_channel(QVariant& val){
       if (val.type() == QVariant::Type::Uuid)
           uuid_form = val.toUuid();
   }
   void _ws_message::set_name(QVariant& val){
       if (val.type() == QVariant::Type::String)
           name = val.toString();
   }
   void _ws_message::set_message(QVariant& val){
       if (val.type() == QVariant::Type::String)
           message = val.toString();
   }
   void _ws_message::set_command(QVariant& val){
       if (val.type() == QVariant::Type::String)
           command = val.toString();
   }
   void _ws_message::set_hash(QVariant& val){
       if (val.type() == QVariant::Type::String)
           hash = val.toString();
   }
   void _ws_message::set_app_name(QVariant& val){
       if (val.type() == QVariant::Type::String)
           app_name = val.toString();
   }
   void _ws_message::set_result(QVariant& val){
       if (val.type() == QVariant::Type::String)
           result = val.toString();
   }
   void _ws_message::set_role(QVariant& val){
       if (val.type() == QVariant::Type::String)
           role = val.toString();
   }
   void _ws_message::set_presentation(QVariant& val){
       if (val.type() == QVariant::Type::String)
           presentation = val.toString();
   }
   void _ws_message::set_contentType(QVariant& val){
       if (val.type() == QVariant::Type::String)
           contentType = val.toString();
   }
   void _ws_message::set_channel_name(QVariant& val){
       if (val.type() == QVariant::Type::String)
           channel_name = val.toString();
   }
   void _ws_message::set_object_name(QVariant& val){
       if (val.type() == QVariant::Type::String)
           object_name = val.toString();
   }
   void _ws_message::set_msg_ref(QVariant& val){
       if (val.type() == QVariant::Type::String)
           msg_ref = val.toString();
   }
   void _ws_message::set_ip_address(QVariant& val){
       if (val.type() == QVariant::Type::String)
           ip_address = val.toString();
   }
   void _ws_message::set_host_name(QVariant& val){
       if (val.type() == QVariant::Type::String)
           host_name = val.toString();
   }
   void _ws_message::set_user_name(QVariant& val){
       if (val.type() == QVariant::Type::String)
           host_name = val.toString();
   }
   void _ws_message::set_device_id(QVariant& val){
       if (val.type() == QVariant::Type::String)
           device_id = val.toString();
   }

   setFun _ws_message::get_set_function(const QString& nameField){
       if (nameField == "uuid")
       {
           return std::bind(&_ws_message::set_uuid, this, std::placeholders::_1);
       }else if (nameField == "uuid_user")
       {
           return std::bind(&_ws_message::set_uuid_user, this, std::placeholders::_1);
       }else if (nameField == "uuid_form")
       {
           return std::bind(&_ws_message::set_uuid_form, this, std::placeholders::_1);
       }else if (nameField == "uuid_channel")
       {
           return std::bind(&_ws_message::set_uuid_channel, this, std::placeholders::_1);
       }else if (nameField == "presentation")
       {
           return std::bind(&_ws_message::set_presentation, this, std::placeholders::_1);
       }else if (nameField == "role")
       {
           return std::bind(&_ws_message::set_role, this, std::placeholders::_1);
       }else if (nameField == "result")
       {
           return std::bind(&_ws_message::set_result, this, std::placeholders::_1);
       }else if (nameField == "app_name")
       {
           return std::bind(&_ws_message::set_app_name, this, std::placeholders::_1);
       }else if (nameField == "hash")
       {
           return std::bind(&_ws_message::set_hash, this, std::placeholders::_1);
       }else if (nameField == "command")
       {
           return std::bind(&_ws_message::set_command, this, std::placeholders::_1);
       }else if (nameField == "message")
       {
           return std::bind(&_ws_message::set_message, this, std::placeholders::_1);
       }else if (nameField == "name")
       {
           return std::bind(&_ws_message::set_name, this, std::placeholders::_1);
       }else if (nameField == "contentType")
       {
           return std::bind(&_ws_message::set_contentType, this, std::placeholders::_1);
       } else if (nameField == "channel_name")
       {
           return std::bind(&_ws_message::set_channel_name, this, std::placeholders::_1);
       } else if (nameField == "object_name")
       {
           return std::bind(&_ws_message::set_object_name, this, std::placeholders::_1);
       }else if (nameField == "msg_ref")
       {
           return std::bind(&_ws_message::set_msg_ref, this, std::placeholders::_1);
       }else if (nameField == "ip_address")
       {
           return std::bind(&_ws_message::set_ip_address, this, std::placeholders::_1);
       }else if (nameField == "host_name")
       {
           return std::bind(&_ws_message::set_host_name, this, std::placeholders::_1);
       }else if (nameField == "user_name")
       {
           return std::bind(&_ws_message::set_user_name, this, std::placeholders::_1);
       }else if (nameField == "device_id")
       {
           return std::bind(&_ws_message::set_device_id, this, std::placeholders::_1);
       }else
           return nullptr;

}

QVector<QString> _ws_message::get_fields(){
    return {"uuid", "name", "uuid_channel", "message", "uuid_form", "command"
            , "hash", "app_name", "result", "role", "presentation", "uuid_user", "object_name", "host_name", "user_name", "device_id"};
}

Message::Message()
{
    m_message = _ws_message();
    m_doc = QJsonDocument();
}

Message::Message(_ws_message &defMessage) {
    m_message = defMessage;
}

Message::Message(const QString &jsonText) {
    {
        m_doc.fromJson(jsonText.toUtf8());

        if (!m_doc.isEmpty()) {
            auto obj = m_doc.object();
            foreach (const QString& fName, _ws_message::get_fields()) {
                QVariant v = obj.value(fName).toVariant();
                m_message.set_value(fName, v);
            }
        }
    }
}

QString Message::get_json(bool to_base64_){
    m_doc = QJsonDocument();
    auto obj = QJsonObject();
    obj.insert("uuid", m_message.uuid.toString(QUuid::WithoutBraces));
    obj.insert("name", m_message.name);
    obj.insert("uuid_channel", m_message.uuid_channel.toString(QUuid::WithoutBraces));
    obj.insert("message", m_message.message);
    obj.insert("uuid_form", m_message.uuid_form.toString(QUuid::WithoutBraces));
    obj.insert("command", m_message.command);
    obj.insert("result", m_message.result);
    obj.insert("app_name", m_message.app_name);
    obj.insert("role", m_message.role);
    obj.insert("user_uuid", m_message.uuid_user.toString(QUuid::WithoutBraces));
    obj.insert("contentType", m_message.contentType);
    obj.insert("channel_name", m_message.channel_name);
    obj.insert("object_name", m_message.object_name);
    obj.insert("msg_ref", m_message.msg_ref);
    obj.insert("ip_address", m_message.ip_address);
    obj.insert("host_name", m_message.host_name);
    obj.insert("user_name", m_message.user_name);
    obj.insert("device_id", m_message.device_id);

    m_doc.setObject(obj);
    QByteArray result = m_doc.toJson();

    if (!to_base64_)
        return result;
    else
        return result.toBase64();
}

bool Message::is_parse() {
    return m_doc.isEmpty();
}

QString Message::to_string() {
    return get_json(false);
}

_ws_message& Message::message() {
    return m_message;
}
}
