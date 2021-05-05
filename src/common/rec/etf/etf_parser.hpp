#pragma once

#include <optional>

#include "etf_data.hpp"

namespace pugi {
class xml_document;
class xml_node;
}  // namespace pugi

namespace rec::etf {

class Parser {
public:
    static std::optional<etf_t> parse_file(const std::string& filename);

protected:
    static std::optional<etf_t> parse(const ::pugi::xml_document& doc);
    static bool parse_annotation_document(const ::pugi::xml_node& doc,
                                          etf_t&                  etf);
    static bool parse_header(const ::pugi::xml_node& annotation, etf_t& etf);
    static bool parse_tiers(const ::pugi::xml_node& annotation, etf_t& etf);
    static bool parse_linguistic_types(const ::pugi::xml_node& annotation,
                                       etf_t&                  etf);
    static bool parse_constraints(const ::pugi::xml_node& annotation,
                                  etf_t&                  etf);
    static bool parse_vocabularies(const ::pugi::xml_node& annotation,
                                   etf_t&                  etf);
};

}  // namespace rec::etf