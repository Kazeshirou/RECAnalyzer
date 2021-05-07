#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "etf_data.hpp"

namespace rec::eaf {

struct annotation_document_t : public etf::annotation_document_t {};

struct tag_t {
    std::map<std::string, std::string> attributes;
    std::string                        value;
};

struct header_t : public etf::header_t {
    std::vector<tag_t> media_descriptors;
    std::vector<tag_t> properties;
};

struct time_slot_t {
    std::string id;
    size_t      value;
};

struct annotation_t {
    std::string id;
    std::string value;
    std::string ts1_ref;
    std::string ts2_ref;
};

struct tier_t : public etf::tier_t {
    std::vector<annotation_t> annotations;
};

struct linguistic_type_t : public etf::linguistic_type_t {};

struct constraint_t : public etf::constraint_t {};

struct vocabulary_entry_t : public etf::vocabulary_entry_t {};

struct vocabulary_t : public etf::vocabulary_t {};

struct eaf_t {
    annotation_document_t                    annotation_document;
    header_t                                 header;
    std::map<std::string, time_slot_t>       time_slots;
    std::map<std::string, tier_t>            tiers;
    std::map<std::string, linguistic_type_t> linguistic_types;
    std::map<std::string, constraint_t>      constraints;
    std::map<std::string, vocabulary_t>      vocabularies;
};

}  // namespace rec::eaf