#include "include/database_struct.hpp"
#include "include/SyncData.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include "include/query_builder.hpp"

SyncData::SyncData(QObject *parent):
QObject(parent),
m_message("")
{

}

bool SyncData::running() const
{
    return m_running;
}

QString SyncData::message() const
{
    return m_message;
}

void SyncData::setComparisonOfDocuments(const nlohmann::json& resp)
{
    srv_resp = resp;
}

void SyncData::run()
{
//    count = 0;
//    // Переменная m_running отвечает за работу объекта в потоке.
//    // При значении false работа завершается
//    while (m_running)
//    {
//        count++;
//        emit sendMessage(m_message); // Высылаем данные, которые будут передаваться в другой поток
//        qDebug() << m_message << " " << count;
//    }
    synchronizeBaseDocuments();

    emit finished();
}

void SyncData::setRunning(bool running)
{
    if (m_running == running)
        return;

    m_running = running;
    emit runningChanged(running);
}

void SyncData::setMessage(QString message)
{
    if (m_message == message)
        return;

    m_message = message;
    emit messageChanged(message);
}

void SyncData::synchronizeBaseDocuments()
{
    qDebug() << __FUNCTION__;

    auto sqlDatabase = QSqlDatabase::addDatabase("QSQLITE");
    sqlDatabase.setDatabaseName(db_path);

    using namespace arcirk::database;

    if (!sqlDatabase.open()) {
        qCritical() << sqlDatabase.lastError().text();
        return;
    }

    auto objects = srv_resp.value("objects", nlohmann::json{});
    auto comparison_table = srv_resp.value("comparison_table", nlohmann::json{});
    std::map<std::string, std::pair<int,int>> m_ext_table;
    std::vector<std::string> m_vec_new_documents;
    if (comparison_table.is_array() && !comparison_table.empty()) {
        for (auto itr = comparison_table.begin(); itr != comparison_table.end(); ++itr) {
            nlohmann::json r = *itr;
            int ver1 = r["ver1"];
            int ver2 = r["ver2"];
            std::string ref = r["ref"];
            m_ext_table.emplace(ref, std::pair<int,int>(ver1, ver2));
            if(ver1 < ver2){
                m_vec_new_documents.push_back(ref);
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
    nlohmann::json result{};
    if(m_vec_new_documents.size() > 0){
        foreach (auto const& itr, m_vec_new_documents) {
            result += builder::query_builder::data_synchronization_get_object(
                        arcirk::enum_synonym(tables::tbDocuments), itr, sqlDatabase
                        );
        }
    }

    sqlDatabase.close();
}

void SyncData::setDatabaseFileName(const QString &file)
{
    db_path = file;
}
