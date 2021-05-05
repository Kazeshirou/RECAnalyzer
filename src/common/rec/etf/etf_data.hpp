#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace rec::etf {

struct annotation_document_t {
    std::string author;
    std::string date;
    std::string format;
    std::string version;
};

struct header_t {
    std::string media_file;
    std::string time_units;
};

struct tier_t {
    std::string id;
    std::string parent_ref;
    std::string linguistic_type_ref;
};

struct linguistic_type_t {
    std::string              id;
    std::vector<std::string> constraint_refs;
    std::string              vocabulary_ref;
};

struct constraint_t {
    std::string id;
    std::string description;
};

struct vocabulary_entry_t {
    std::string id;
    std::string description;
};

struct vocabulary_t {
    std::string                               id;
    std::string                               description;
    std::map<std::string, vocabulary_entry_t> entries;
};

struct etf_t {
    annotation_document_t                    annotation_document;
    header_t                                 header;
    std::map<std::string, tier_t>            tiers;
    std::map<std::string, linguistic_type_t> linguistic_types;
    std::map<std::string, constraint_t>      constraints;
    std::map<std::string, vocabulary_t>      vocabularies;
};

}  // namespace rec::etf