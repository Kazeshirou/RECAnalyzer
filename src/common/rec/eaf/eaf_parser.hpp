#pragma once

#include <optional>

#include "eaf_data.hpp"
#include "etf_parser.hpp"

namespace rec::eaf {

using Template_parser = etf::Parser;

class Parser : public Template_parser {
public:
    static std::optional<eaf_t> parse_file(const std::string& filename);

protected:
    static std::optional<eaf_t> parse(const ::pugi::xml_document& doc);
    static bool parse_annotation_document(const ::pugi::xml_node& doc,
                                          eaf_t&                  eaf);
    static bool parse_header(const ::pugi::xml_node& node, header_t& header);
    static bool parse_time_slots(
        const ::pugi::xml_node&             node,
        std::map<std::string, time_slot_t>& time_slots);
    static bool parse_tiers(const ::pugi::xml_node&        node,
                            std::map<std::string, tier_t>& tiers);
    static bool parse_tier(const ::pugi::xml_node& node, tier_t& tier);
    static bool parse_annotation(const ::pugi::xml_node& node,
                                 annotation_t&           annotation);
    static bool parse_alignable_annotation(
        const ::pugi::xml_node& alignable_annotation_node,
        annotation_t&           annotation);
    static bool parse_ref_annotation(const ::pugi::xml_node& node,
                                     annotation_t&           annotation);
    static bool parse_linguistic_types(
        const ::pugi::xml_node&                   node,
        std::map<std::string, linguistic_type_t>& linguistic_types);
    static bool parse_constraints(
        const ::pugi::xml_node&              node,
        std::map<std::string, constraint_t>& constraints);
    static bool parse_vocabularies(
        const ::pugi::xml_node&              node,
        std::map<std::string, vocabulary_t>& vocabularies);
};

}  // namespace rec::eaf