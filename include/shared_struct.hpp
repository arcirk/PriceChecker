#ifndef ARCIRK_SHARED_MODULE_HPP
#define ARCIRK_SHARED_MODULE_HPP

#include "includes.hpp"

#define ARCIRK_VERSION "1.1.0"
#define ARCIRK_SERVER_CONF "server_conf.json"


BOOST_FUSION_DEFINE_STRUCT(
        (arcirk::client), client_param,
        (std::string, app_name)
        (std::string, user_uuid)
        (std::string, user_name)
        (std::string, hash)
        (std::string, host_name)
        (std::string, password)
        (std::string, session_uuid)
        (std::string, system_user)
        (std::string, device_id)
)

BOOST_FUSION_DEFINE_STRUCT(
        (arcirk::server), server_response,
        (std::string, command)
        (std::string, message)
        (std::string, param)
        (std::string, result)
        (std::string, sender)
        (std::string, receiver)
        (std::string, uuid_form)
        (std::string, app_name)
        (std::string, sender_name)
        (std::string, sender_uuid)
        (std::string, receiver_name)
        (std::string, receiver_uuid)
        (std::string, version)
)

BOOST_FUSION_DEFINE_STRUCT(
        (arcirk::client), checker_conf,
        (std::string, typePriceRef)
        (std::string, stockRef)
        (std::string, typePrice)
        (std::string, stock)
)

namespace arcirk::server{

    enum server_commands{
        ServerVersion, //Версия сервера
        ServerOnlineClientsList, //Список активных пользователей
        SetClientParam, //Параметры клиента
        ServerConfiguration, //Конфигурация сервера
        UserInfo, //Информация о пользователе (база данных)
        InsertOrUpdateUser, //Обновить или добавить пользователя (база данных)
        CommandToClient, //Команда клиенту (подписчику)
        ServerUsersList, //Список пользователей (база данных)
        ExecuteSqlQuery, //выполнить запрос к базе данных
        GetMessages, //Список сообщений
        UpdateServerConfiguration, //Обновить конфигурацию сервера
        HttpServiceConfiguration, //Получить конфигурацию http сервиса 1С
        InsertToDatabaseFromArray, //Добавить массив записей в базу //устарела удалить
        SetNewDeviceId, //Явная установка идентификатора на устройствах где не возможно его получить
        ObjectSetToDatabase, //Синхронизация объекта 1С с базой
        ObjectGetFromDatabase, //Получить объект типа 1С из базы данных для десериализации
        SyncGetDiscrepancyInData, //Получить расхождения в данных между базами на клиенте и на Сервере
        SyncUpdateDataOnTheServer,
        WebDavServiceConfiguration,
        CMD_INVALID=-1,
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(server_commands, {
         {CMD_INVALID, nullptr}    ,
         {ServerVersion, "ServerVersion"}  ,
         {ServerOnlineClientsList, "ServerOnlineClientsList"}    ,
         {SetClientParam, "SetClientParam"}    ,
         {ServerConfiguration, "ServerConfiguration"}    ,
         {UserInfo, "UserInfo"}    ,
         {InsertOrUpdateUser, "InsertOrUpdateUser"}    ,
         {CommandToClient, "CommandToClient"}    ,
         {ServerUsersList, "ServerUsersList"}    ,
         {ExecuteSqlQuery, "ExecuteSqlQuery"}    ,
         {GetMessages, "GetMessages"}    ,
         {UpdateServerConfiguration, "UpdateServerConfiguration"}    ,
         {HttpServiceConfiguration, "HttpServiceConfiguration"}    ,
         {InsertToDatabaseFromArray, "InsertToDatabaseFromArray"}    ,
         {SetNewDeviceId, "SetNewDeviceId"}    ,
         {ObjectSetToDatabase, "ObjectSetToDatabase"}    ,
         {ObjectGetFromDatabase, "ObjectGetFromDatabase"}    ,
         {SyncGetDiscrepancyInData, "SyncGetDiscrepancyInData"}    ,
         {SyncUpdateDataOnTheServer, "SyncUpdateDataOnTheServer"}    ,
         {WebDavServiceConfiguration, "WebDavServiceConfiguration"}    ,
    })

}

BOOST_FUSION_DEFINE_STRUCT(
        (arcirk::server), server_config,
        (std::string, ServerHost)
        (int, ServerPort)
        (std::string, ServerUser)
        (std::string, ServerUserHash)
        (std::string, ServerName)
        (std::string, ServerHttpRoot)
        (std::string, ServerWorkingDirectory)
        (bool, AutoConnect)
        (bool, UseLocalWebDavDirectory)
        (std::string, LocalWebDavDirectory)
        (std::string, WebDavHost)
        (std::string, WebDavUser)
        (std::string, WebDavPwd)
        (bool, WebDavSSL)
        (int, SQLFormat)
        (std::string, SQLHost)
        (std::string, SQLUser)
        (std::string, SQLPassword)
        (std::string, HSHost)
        (std::string, HSUser)
        (std::string, HSPassword)
        (bool, ServerSSL)
        (std::string, SSL_crt_file)
        (std::string, SSL_key_file)
        (bool, UseAuthorization)
        (std::string, ApplicationProfile)
        (int, ThreadsCount)
        (std::string, Version)
        (bool, ResponseTransferToBase64)
        (bool, AllowDelayedAuthorization)
        (bool, AllowHistoryMessages)
        (std::string, ServerProtocol)
);

BOOST_FUSION_DEFINE_STRUCT(
        (arcirk::client), client_private_config,
        (bool, showImage)
        (bool, keyboardInputMode)
        (bool, priceCheckerMode)
        (std::string, deviceId)
);

#endif
