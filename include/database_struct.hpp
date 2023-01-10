#ifndef DATABASE_STRUCT_HPP
#define DATABASE_STRUCT_HPP

#include "includes.hpp"

BOOST_FUSION_DEFINE_STRUCT(
    (arcirk::database), documents,
    (int, _id)
    (std::string, first)
    (std::string, second)
    (std::string, ref)
    (int, date)
    );

BOOST_FUSION_DEFINE_STRUCT(
    (arcirk::database), document_table,
    (int, _id)
    (std::string, first)
    (std::string, second)
    (std::string, ref)
    (std::string, parent)
    (std::string, barcode)
    (int, item_count)
    );

#endif // DATABASE_STRUCT_HPP
