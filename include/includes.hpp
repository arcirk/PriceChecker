#ifndef ARCIRK_INCLUDES_HPP
#define ARCIRK_INCLUDES_HPP

#include <iostream>
#include <exception>
#include <functional>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/variant.hpp>
#include <nlohmann/json.hpp>
#include <pre/json/from_json.hpp>
#include <pre/json/to_json.hpp>
//#include <soci/soci.h>
//#include <soci/sqlite3/soci-sqlite3.h>
#include <string_view>
//#include <execution>

#define UNDEFINED std::monostate()

namespace arcirk{
    template<typename T>
    static inline std::string enum_synonym(T value){
        using n_json = nlohmann::json;
        return n_json(value).get<std::string>();
    };

    template<typename T>
    nlohmann::json values_from_param(const nlohmann::json& param){
        if(param.empty())
            return {};
        T e = T();
        auto source = pre::json::to_json(e);
        nlohmann::json result = {};

        if(source.is_object()){
            for (auto itr = source.begin(); itr != source.end() ; ++itr) {
                auto i = param.find(itr.key());
                if( i != param.end()){
                    result[itr.key()] = i.value();
                }
            }
            return result;
        }else
            return {};
    }
    namespace uuids{
    //    bool is_valid_uuid(std::string const& maybe_uuid, boost::uuids::uuid& result);
    //    boost::uuids::uuid string_to_uuid(const std::string& sz_uuid, bool random_uuid = false);
        inline std::string nil_string_uuid() {return "00000000-0000-0000-0000-000000000000";};
    //    boost::uuids::uuid nil_uuid();
    //    std::string uuid_to_string(const boost::uuids::uuid& uuid);
    //    boost::uuids::uuid random_uuid();
    }
}

#endif
