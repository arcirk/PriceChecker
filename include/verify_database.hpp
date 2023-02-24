#ifndef VRIFY_DATABASE_HPP
#define VRIFY_DATABASE_HPP

#include <QtCore>
#include "database_struct.hpp"
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QUuid>

using namespace arcirk::database;

namespace arcirk::database{

    static inline QString get_ddl(tables table){
        switch (table) {
            case tbUsers: return "";
            case tbMessages: return "";
            case tbOrganizations: return "";
            case tbSubdivisions: return "";
            case tbWarehouses: return "";
            case tbPriceTypes: return "";
            case tbWorkplaces: return "";
            case tbDevices: return "";
            case tbDocumentsTables: return QString::fromStdString(document_table_table_ddl);
            case tbDocuments: return QString::fromStdString(documents_table_ddl);
            case tbNomenclature: return QString::fromStdString(nomenclature_table_ddl);
            case tbDatabaseConfig: return QString::fromStdString(database_config_table_ddl);
            case tbDevicesType:  return "";
            case tbPendingOperations: return QString::fromStdString(pending_operations_table_ddl);
            case tables_INVALID:{
                break;
            }
        }

        return {};
    }


    static inline QMap<std::string, table_info_sqlite>  table_info(QSqlDatabase& sql, tables table) {
        QMap<std::string, table_info_sqlite> result{};
        QString  query = QString("PRAGMA table_info(\"%1\");").arg(QString::fromStdString(arcirk::enum_synonym(table)));
        QSqlQuery rs(query);
        while (rs.next())
        {
            auto info = table_info_sqlite();
            info.name = rs.value("name").toString().toStdString();
            info.type = rs.value("type").toString().toStdString();
            result.insert(info.name, info);
        }
        return result;
    }
    static inline std::string query_insert(const std::string& table_name, nlohmann::json values){
        std::string result = QString("insert into %1 (").arg(QString::fromStdString(table_name)).toStdString();
        std::string string_values;
        std::vector<std::pair<std::string, nlohmann::json>> m_list;
        auto items_ = values.items();
        for (auto itr = items_.begin(); itr != items_.end(); ++itr) {
            m_list.emplace_back(itr.key(), itr.value());
        }

        for (auto itr = m_list.cbegin(); itr != m_list.cend() ; ++itr) {
            result.append("[" + itr->first + "]");
            std::string value;
            if(itr->second.is_string())
                value = itr->second.get<std::string>();
            else if(itr->second.is_number_float())
                value = std::to_string(itr->second.get<double>());
            else if(itr->second.is_number_integer())
                value = std::to_string(itr->second.get<long long>());

            if(value.empty())
                string_values.append("''");
            else
                string_values.append(QString("'%1'").arg(QString::fromStdString(value)).toStdString());
            if(itr != (--m_list.cend())){
                result.append(",\n");
                string_values.append(",\n");
            }
        }
        result.append(")\n");
        result.append("values(");
        result.append(string_values);
        result.append(")");

        return result;
    }

    static inline void rebase(QSqlDatabase& sql, tables table){

        QString table_name = QString::fromStdString(arcirk::enum_synonym(table));
        QString temp_query = QString("create temp table %1_temp as select * from %1;").arg(table_name);

        sql.transaction();
        QSqlQuery query;
        query.exec(temp_query);
        query.exec(QString("drop table %1;").arg(table_name));
        query.exec(get_ddl(table));
        sql.commit();

        sql.transaction();
        QSqlQuery rs(QString("select * from %1_temp;").arg(table_name));
        //std::vector<std::string> columns{};
        auto t_info = table_info(sql, table);
        int count = 0;
        while (rs.next())
        {
            QSqlRecord row = rs.record();
            nlohmann::json values{};
            count++;
            for(int i = 0; i < row.count(); ++i)
            {
                //const column_properties & props = row.get_properties(i);
                std::string column_name = row.fieldName(i).toStdString();

                if(t_info.find(column_name) == t_info.end())
                    continue;

                QVariant val = row.field(i).value();

                if(val.userType() == QMetaType::QString)
                    values[column_name] = val.toString().toStdString();
                else if(val.userType() == QMetaType::Double)
                    values[column_name] = val.toDouble();
                else if(val.userType() == QMetaType::Int)
                    values[column_name] = val.toInt();
                else if(val.userType() == QMetaType::LongLong)
                    values[column_name] = val.toLongLong();
                else if(val.userType() == QMetaType::ULongLong)
                    values[column_name] = val.toULongLong();


            }

            query.exec(QString::fromStdString(query_insert(table_name.toStdString(), values)));

        }

        query.exec(QString("drop table if exists %1_temp;").arg(table_name));

        sql.commit();

    }

    static inline std::map<tables, int> get_release_tables_versions(){
        std::map<tables, int> result;
        result.emplace(tables::tbDatabaseConfig, 2);
        result.emplace(tables::tbNomenclature, 2);
        result.emplace(tables::tbDocuments, 3);
        result.emplace(tables::tbDevices, 2);
        result.emplace(tables::tbMessages, 2);
        result.emplace(tables::tbUsers, 2);
        result.emplace(tables::tbDevicesType, 2);
        result.emplace(tables::tbDocumentsTables, 3);
        result.emplace(tables::tbOrganizations, 2);
        result.emplace(tables::tbPriceTypes, 2);
        result.emplace(tables::tbSubdivisions, 2);
        result.emplace(tables::tbWarehouses, 2);
        result.emplace(tables::tbWorkplaces, 2);
        result.emplace(tables::tbPendingOperations, 1);
        return result;
    }

    static inline std::vector<tables> tables_name_array(){
        std::vector<tables> result = {
//            tbUsers,
//            tbMessages,
//            tbOrganizations,
//            tbSubdivisions,
//            tbWarehouses,
//            tbPriceTypes,
//            tbWorkplaces,
//            tbDevices,
//            tbDevicesType,
            tbDocuments,
            tbDocumentsTables,
            tbNomenclature,
            tbDatabaseConfig,
            tbPendingOperations
        };
        return result;
    }

    static inline std::vector<std::string> get_database_tables(QSqlDatabase& sql){

        QSqlQuery rs("SELECT name FROM sqlite_master WHERE type='table';");
        std::vector<std::string> result;
        while (rs.next())
        {
            result.push_back(rs.value(0).toString().toStdString());
        }

        return result;
    }


    static inline void verify_database(QSqlDatabase& sql){

           auto release_table_versions = get_release_tables_versions(); //Текущие версии релиза
           auto database_tables = get_database_tables(sql); //Массив существующих таблиц
           auto tables_arr = tables_name_array(); //Массив имен таблиц
           //bool is_rebase = false;

           QSqlQuery rs;
           //Сначала проверим, существует ли таблица версий
           if(std::find(database_tables.begin(), database_tables.end(), arcirk::enum_synonym(tables::tbDatabaseConfig)) == database_tables.end()) {
               auto ddl = get_ddl(tables::tbDatabaseConfig);
               rs.exec(ddl);
           }

           //Заполним массив версий для сравнения
           std::map<tables, int> current_table_versions;
           rs.exec(QString("select * from %1%;").arg(QString::fromStdString(arcirk::enum_synonym(tables::tbDatabaseConfig))));
           while (rs.next())
           {
               nlohmann::json t_name = rs.value("first").toString().toStdString();
               auto t_ver = rs.value("version").toInt();
               current_table_versions.emplace(t_name.get<tables>(), t_ver);
           }

           //Выполним реструктуризацию
           for (auto t_name : tables_arr) {
               if(t_name == tables::tbDatabaseConfig)
                   continue;
               if(std::find(database_tables.begin(), database_tables.end(), arcirk::enum_synonym(t_name)) == database_tables.end()){
                   //Таблицы не существует, просто создаем новую
                   sql.transaction();
                   rs.exec(get_ddl(t_name));
                   rs.exec(QString::fromStdString(query_insert(arcirk::enum_synonym(tables::tbDatabaseConfig), nlohmann::json{
                           {"first", arcirk::enum_synonym(t_name)},
                           {"version", release_table_versions[t_name]},
                           {"ref", QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString()}
                   })));
                   sql.commit();
               }else{
                   //Если существует, проверяем версию таблицы если не совпадает запускаем реструктуризацию
                   int current_ver = 0;
                   auto itr_ver = current_table_versions.find(t_name);
                   if(itr_ver != current_table_versions.end())
                       current_ver = itr_ver->second;
                   if(release_table_versions[t_name] != current_ver){
                       rebase(sql, t_name);
                       sql.transaction();

                       if(current_ver == 0)
                           rs.exec(QString::fromStdString(query_insert(arcirk::enum_synonym(tables::tbDatabaseConfig), nlohmann::json{
                                   {"first", arcirk::enum_synonym(t_name)},
                                   {"version", release_table_versions[t_name]},
                                   {"ref",  QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString()}
                           })));
                       else
                           rs.exec(QString("update %1% set version='%2%' where [first]='%3%'").arg(
                                       QString::fromStdString(arcirk::enum_synonym(tables::tbDatabaseConfig)),
                                       QString::number(release_table_versions[t_name]),
                                       QString::fromStdString(arcirk::enum_synonym(t_name))));
                       sql.commit();
                   }
                   //is_rebase = true; //пересоздадим представления
               }
           }

       }

    static inline void execute(const std::string& query_text, QSqlDatabase& sql, nlohmann::json& result_table, const std::vector<std::string>& column_ignore = {}){

        using namespace nlohmann;

        QSqlQuery rs(QString::fromStdString(query_text));


        json columns = {"line_number"};
        json roms = {};
        int line_number = 0;

        while (rs.next())
        {
            line_number++;
            QSqlRecord row = rs.record();
            json j_row = {{"line_number", line_number}};

            for(int i = 0; i < row.count(); ++i)
            {
                //const column_properties & props = row.get_properties(i);
                std::string column_name = row.fieldName(i).toStdString();

                if((columns.size()) != row.count()){
                    columns.push_back(column_name);
                }

                if(std::find(column_ignore.begin(), column_ignore.end(), column_name) != column_ignore.end()){
                    j_row += {column_name, ""};
                    continue;
                }

                QVariant val = row.field(i).value();

                if(val.userType() == QMetaType::QString)
                    j_row += {column_name, val.toString().toStdString()};
                else if(val.userType() == QMetaType::Double)
                    j_row += {column_name, val.toDouble()};
                else if(val.userType() == QMetaType::Int)
                    j_row += {column_name, val.toInt()};
                else if(val.userType() == QMetaType::LongLong)
                    j_row += {column_name, val.toLongLong()};
                else if(val.userType() == QMetaType::ULongLong)
                    j_row += {column_name, val.toULongLong()};

            }

            roms += j_row;
        }

        result_table = {
                {"columns", columns},
                {"rows", roms}
        };

    }

}
#endif // VRIFY_DATABASE_HPP
