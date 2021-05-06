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
    static bool parse_annotation_document_attributes(
        const ::pugi::xml_node& node,
        annotation_document_t&  annotation_document);
    static bool parse_header(const ::pugi::xml_node& node, header_t& header);
    static bool parse_tiers(const ::pugi::xml_node&        node,
                            std::map<std::string, tier_t>& tiers);
    static bool parse_tier(const ::pugi::xml_node& node, tier_t& tier);
    static bool parse_linguistic_types(
        const ::pugi::xml_node&                   node,
        std::map<std::string, linguistic_type_t>& linguistic_types);
    static bool parse_linguistic_type(const ::pugi::xml_node& node,
                                      linguistic_type_t&      linguistic_type);
    static bool parse_constraints(
        const ::pugi::xml_node&              node,
        std::map<std::string, constraint_t>& constraints);
    static bool parse_constraint(const ::pugi::xml_node& node,
                                 constraint_t&           constraint);
    static bool parse_vocabularies(
        const ::pugi::xml_node&              node,
        std::map<std::string, vocabulary_t>& vocabularies);
    static bool parse_vocabulary(const ::pugi::xml_node& node,
                                 vocabulary_t&           vocabulary);
};

}  // namespace rec::etf