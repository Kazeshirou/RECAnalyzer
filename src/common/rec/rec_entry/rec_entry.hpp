#pragma once

#include <fmt/format.h>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "eaf_data.hpp"
#include "logger.hpp"
#include "rec_template.hpp"

namespace rec {

struct rec_entry_t {
    struct time_slot_t {
        std::string id;
        size_t      value;
        bool        operator<(const time_slot_t& other) const {
            return value < other.value;
        }
        bool operator<=(const time_slot_t& other) const {
            return value <= other.value;
        }
        bool operator>(const time_slot_t& other) const {
            return value > other.value;
        }
        bool operator>=(const time_slot_t& other) const {
            return value >= other.value;
        }
        bool operator==(const time_slot_t& other) const {
            return value == other.value;
        }
        bool operator!=(const time_slot_t& other) const {
            return value != other.value;
        }
    };

    struct annotation_t {
        size_t annotation_id;
        size_t ts1;
        size_t ts2;
    };

    rec_entry_t() = delete;
    rec_entry_t(rec_template_t& rec_template_ref);
    rec_entry_t(rec_template_t& rec_template_ref, const eaf::eaf_t& eaf);
    rec_entry_t(const rec_entry_t&) = default;
    rec_entry_t(rec_entry_t&&)      = default;
    rec_entry_t& operator=(const rec_entry_t&) = default;
    rec_entry_t& operator=(rec_entry_t&&) = default;
    ~rec_entry_t()                        = default;

    bool operator==(const rec_entry_t& other) const;
    bool operator!=(const rec_entry_t& other) const {
        return !(*this == other);
    }

    void add_time_slots(const eaf::eaf_t& eaf);
    void add_tier(const eaf::eaf_t& eaf, const std::string& id,
                  const eaf::tier_t& eaf_tier);
    void add_annotation(size_t                   current_tier_i,
                        const eaf::annotation_t& eaf_annotation);
    void resolve_tier_parents(const eaf::eaf_t& eaf);

    std::function<bool(const annotation_t&, const annotation_t&)>
        compare_annotations = [this](const annotation_t& first,
                                     const annotation_t& second) -> bool {
        if ((first.ts1 >= time_slots.size()) ||
            (first.ts2 >= time_slots.size()) ||
            (second.ts1 >= time_slots.size()) ||
            (second.ts2 >= time_slots.size())) {
            my_log::Logger::warning(
                "rec::rec_entry_t",
                fmt::format("{}: При сортировке аннотаций в сравнение "
                            "попала невалидная аннотация с битым ts",
                            filename));
            return false;
        }
        size_t ts11 = time_slots[first.ts1].value;
        size_t ts12 = time_slots[first.ts2].value;
        size_t ts21 = time_slots[second.ts1].value;
        size_t ts22 = time_slots[second.ts2].value;
        if (ts11 < ts21) {
            return true;
        }
        if (ts21 > ts11) {
            return false;
        }

        if (ts12 < ts22) {
            return true;
        }

        return false;
    };


    bool reduce_by_tiers(std::vector<std::string> ignore_tiers);
    bool reduce_by_annotations(std::vector<std::string> ignore_annotations);
    bool reduce_by_annotation(const std::string& ignore_annotation);

    rec_template_t& rec_template;
    std::string     author;
    std::string     date;
    std::string     format;
    std::string     version;
    std::string     time_units;

    std::vector<time_slot_t>  time_slots;
    std::vector<annotation_t> annotations;

    std::map<std::string, size_t> time_slots_map;
    std::map<std::string, size_t> annotation_id_map;

    std::string filename;
};

}  // namespace rec