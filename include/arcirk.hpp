#ifndef ARCIRK_HPP
#define ARCIRK_HPP

//#include <QApplication>

#include <QUuid>
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>

namespace arcirk {

    QString get_sha1(const QByteArray& p_arg);
    QString get_hash(const QString& first, const QString& second);
    QUuid nil_uuid();
    QString nil_string_uuid();
    QString crypt(const std::string &source, const std::string& key);

    typedef std::function<void(QVariant&)> setFun;

    typedef struct _ws_message{
        public:
            QUuid   uuid;
            QString name;
            QUuid   uuid_channel;
            QString message;
            QUuid   uuid_form;
            QString command;
            QString hash;
            QString app_name;
            QString result;
            QString role;
            QString presentation;
            QUuid   uuid_user;
            QString contentType;
            QString channel_name;
            QString object_name;
            QString msg_ref;
            QString ip_address;
            QString host_name;
            QString user_name;
            QString device_id;

            _ws_message();

            static QVector<QString> get_fields();
            void set_value(const QString& nameField, QVariant& val);

        private:
            void set_uuid(QVariant& val);
            void set_uuid_user(QVariant& val);
            void set_uuid_form(QVariant& val);
            void set_uuid_channel(QVariant& val);
            void set_name(QVariant& val);
            void set_message(QVariant& val);
            void set_command(QVariant& val);
            void set_hash(QVariant& val);
            void set_app_name(QVariant& val);
            void set_result(QVariant& val);
            void set_role(QVariant& val);
            void set_presentation(QVariant& val);
            void set_contentType(QVariant& val);
            void set_channel_name(QVariant& val);
            void set_object_name(QVariant& val);
            void set_msg_ref(QVariant& val);
            void set_ip_address(QVariant& val);
            void set_host_name(QVariant& val);
            void set_user_name(QVariant& val);
            void set_device_id(QVariant& val);

            setFun get_set_function(const QString& nameField);

        }_ws_message;
    class Message
    {
    public:
        explicit
        Message(_ws_message &defMessage);
        explicit
        Message(const QString &jsonText);

        explicit
        Message();

        QString get_json(bool to_base64_);

        bool is_parse();

        QString to_string();

        _ws_message& message();
    private:
        _ws_message m_message;
        QJsonDocument m_doc{};
    };


}

#endif // ARCIRK_H
