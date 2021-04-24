#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "etf_data.hpp"

namespace rec {

enum class CONSTRAINT : uint8_t {
    NONE                 = 0,
    TIME_SUBDIVISION     = 1,
    SYMBOLIC_SUBDIVISION = 2,
    SYMBOLIC_ASSOCIATION = 3,
    INCLUDED_IN          = 4
};

struct rec_template_t {
    struct tier_t {
        std::string           name;
        std::optional<size_t> parent;
        CONSTRAINT            constraint;
    };

    struct annotation_t {
        std::string value;
        std::string description;
        size_t      tier;
    };

    rec_template_t()                      = default;
    rec_template_t(const rec_template_t&) = default;
    rec_template_t(const etf::etf_t& etf);
    rec_template_t(rec_template_t&&) = default;
    rec_template_t& operator=(const rec_template_t&) = default;
    rec_template_t& operator                         =(const etf::etf_t& etf);
    rec_template_t& operator=(rec_template_t&&) = default;
    ~rec_template_t()                           = default;

    std::string               author;
    std::string               date;
    std::string               format;
    std::string               version;
    std::string               time_units;
    std::vector<tier_t>       tiers;
    std::vector<annotation_t> annotations;

    std::map<std::string, size_t> tiers_map;
    std::map<std::string, size_t> annotations_map;
};

}  // namespace rec