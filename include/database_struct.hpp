#ifndef DATABASE_STRUCT_HPP
#define DATABASE_STRUCT_HPP

#include "includes.hpp"

BOOST_FUSION_DEFINE_STRUCT(
        (arcirk::database), workplaces,
        (int, _id)
        (std::string, first)
        (std::string, second)
        (std::string, ref)
        (std::string, cache)
        (std::string, server)

);

BOOST_FUSION_DEFINE_STRUCT(
        (arcirk::database), devices,
        (int, _id)
        (std::string, first)
        (std::string, second)
        (std::string, ref)
        (std::string, cache)
        (std::string, deviceType)
        (std::string, address)
        (std::string, workplace)
        (std::string, price_type)
        (std::string, warehouse)
        (std::string, subdivision)
        (std::string, organization)
);

BOOST_FUSION_DEFINE_STRUCT(
        (arcirk::database), devices_view,
        (std::string, ref)
        (std::string, workplace)
        (std::string, price)
        (std::string, warehouse)
        (std::string, subdivision)
        (std::string, organization)
);
BOOST_FUSION_DEFINE_STRUCT(
        (arcirk::database), documents,
        (int, _id)
        (std::string, first)
        (std::string, second)
        (std::string, ref)
        (std::string, cache)
        (std::string, number)
        (int, date)
        (std::string, xml_type)
        (int, version)
        (std::string, device_id)

);

BOOST_FUSION_DEFINE_STRUCT(
        (arcirk::database), document_table,
        (int, _id)
        (std::string, first)
        (std::string, second)
        (std::string, ref)
        (std::string, cache)
        (double, price)
        (double, quantity)
        (std::string, barcode)
        (std::string, parent)
);
namespace arcirk::database{
    enum tables{
        tbUsers,
        tbMessages,
        tbOrganizations,
        tbSubdivisions,
        tbWarehouses,
        tbPriceTypes,
        tbWorkplaces,
        tbDevices,
        tbDevicesType,
        tbDocuments,
        tbDocumentsTables,
        tbNomenclature,
        tables_INVALID=-1,
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(tables, {
        {tables_INVALID, nullptr}    ,
        {tbUsers, "Users"}  ,
        {tbMessages, "Messages"}  ,
        {tbOrganizations, "Organizations"}  ,
        {tbSubdivisions, "Subdivisions"}  ,
        {tbWarehouses, "Warehouses"}  ,
        {tbPriceTypes, "PriceTypes"}  ,
        {tbWorkplaces, "Workplaces"}  ,
        {tbDevices, "Devices"}  ,
        {tbDevicesType, "DevicesType"}  ,
        {tbDocuments, "Documents"}  ,
        {tbDocumentsTables, "DocumentsTables"}  ,
        {tbNomenclature, "Nomenclature"}  ,
    })

    enum views{
        dvDevicesView,
        views_INVALID=-1,
    };
    NLOHMANN_JSON_SERIALIZE_ENUM(views, {
        { views_INVALID, nullptr }    ,
        { dvDevicesView, "DevicesView" }  ,
    });

    enum devices_type{
        devDesktop,
        devServer,
        devPhone,
        devTablet,
        devExtendedLib,
        dev_INVALID=-1
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(devices_type, {
        {dev_INVALID, nullptr},
        {devDesktop, "Desktop"},
        {devServer, "Server"},
        {devPhone, "Phone"},
        {devTablet, "Tablet"},
        {devExtendedLib, "ExtendedLib"},
    });

    const std::string documents_table_ddl = "CREATE TABLE Documents (\n"
                                          "    _id             INTEGER   PRIMARY KEY AUTOINCREMENT,\n"
                                          "    [first]         TEXT,\n"
                                          "    second          TEXT,\n"
                                          "    ref             TEXT (36) UNIQUE\n"
                                          "                             NOT NULL,\n"
                                          "    cache           TEXT      DEFAULT \"\",\n"
                                          "    number          TEXT      DEFAULT \"\",\n"
                                          "    date            INTEGER NOT NULL DEFAULT(0),\n"
                                          "    xml_type        TEXT      DEFAULT \"\",\n"
                                          "    version         INTEGER NOT NULL DEFAULT(0),\n"
                                          "    device_id       TEXT (36) DEFAULT [00000000-0000-0000-0000-000000000000]\n"
                                          ");";

    const std::string document_table_table_ddl = "CREATE TABLE DocumentsTables (\n"
                                          "    _id             INTEGER   PRIMARY KEY AUTOINCREMENT,\n"
                                          "    [first]         TEXT,\n"
                                          "    second          TEXT,\n"
                                          "    ref             TEXT (36) UNIQUE\n"
                                          "                             NOT NULL,\n"
                                          "    cache           TEXT      DEFAULT \"\",\n"
                                          "    price           DOUBLE DEFAULT (0),\n"
                                          "    quantity        DOUBLE DEFAULT (0),\n"
                                          "    barcode         TEXT      DEFAULT \"\",\n"
                                          "    parent          TEXT (36) DEFAULT [00000000-0000-0000-0000-000000000000]\n"
                                          ");";

    const std::string nomenclature_table_ddl = "CREATE TABLE Nomenclature (\n"
                                                 "    _id             INTEGER   PRIMARY KEY AUTOINCREMENT,\n"
                                                 "    [first]         TEXT,\n"
                                                 "    second          TEXT,\n"
                                                 "    ref             TEXT (36) UNIQUE\n"
                                                 "                             NOT NULL,\n"
                                                 "    cache           TEXT      DEFAULT \"\",\n"
                                                 "    parent          TEXT (36) DEFAULT [00000000-0000-0000-0000-000000000000]\n"
                                                 ");";


    static inline nlohmann::json table_default_json(arcirk::database::tables table) {

        //using namespace arcirk::database;
        switch (table) {
            case tbUsers:{
//                auto usr_info = user_info();
//                usr_info.ref = arcirk::uuids::nil_string_uuid();
//                usr_info.parent = arcirk::uuids::nil_string_uuid();
//                usr_info.is_group = 0;
//                usr_info.deletion_mark = 0;
//                return pre::json::to_json(usr_info);
                break;
            }
            case tbMessages:{
//                auto tbl = messages();
//                tbl.ref = arcirk::uuids::nil_string_uuid();
//                tbl.content_type ="Text";
//                return pre::json::to_json(tbl);
                break;
            }
            case tbOrganizations:{
//                auto tbl = organizations();
//                tbl.ref = arcirk::uuids::nil_string_uuid();
//                return pre::json::to_json(tbl);
                break;
            }
            case tbSubdivisions:{
//                auto tbl = subdivisions();
//                tbl.ref = arcirk::uuids::nil_string_uuid();
//                return pre::json::to_json(tbl);
                break;
            }
            case tbWarehouses:{
//                auto tbl = warehouses();
//                tbl.ref = arcirk::uuids::nil_string_uuid();
//                return pre::json::to_json(tbl);
                break;
            }
            case tbPriceTypes:{
//                auto tbl = price_types();
//                tbl.ref = arcirk::uuids::nil_string_uuid();
//                return pre::json::to_json(tbl);
                break;
            }
            case tbWorkplaces:{
//                auto tbl = workplaces();
//                tbl.ref = arcirk::uuids::nil_string_uuid();
//                tbl.server = arcirk::uuids::nil_string_uuid();
//                return pre::json::to_json(tbl);
                break;
            }
            case tbDevices:{
//                auto tbl = devices();
//                tbl.ref = arcirk::uuids::nil_string_uuid();
//                tbl.deviceType = "Desktop";
//                tbl.address = "127.0.0.1";
//                tbl.workplace = arcirk::uuids::nil_string_uuid();
//                tbl.price_type = arcirk::uuids::nil_string_uuid();
//                tbl.warehouse = arcirk::uuids::nil_string_uuid();
//                tbl.subdivision = arcirk::uuids::nil_string_uuid();
//                tbl.organization = arcirk::uuids::nil_string_uuid();
//                return pre::json::to_json(tbl);
                break;
            }
            case tbDocumentsTables: {
                auto tbl = document_table();
                tbl.ref = arcirk::uuids::nil_string_uuid();
                tbl.price = 0;
                tbl.quantity = 0;
                return pre::json::to_json(tbl);

            }
            case tbDocuments: {
                auto tbl = documents();
                tbl.ref = arcirk::uuids::nil_string_uuid();
                tbl.device_id = arcirk::uuids::nil_string_uuid();
                tbl.date = 0;
                return pre::json::to_json(tbl);
            }
            case tables_INVALID:{
                break;
            }
            case tbDevicesType:
                break;
        }

        return {};
    }

    template<typename T>
    static inline T table_default_struct(arcirk::database::tables table){
        auto j = table_default_json(table);
        auto result = pre::json::from_json<T>(j);
        return result;
    }
}
#endif // DATABASE_STRUCT_HPP
