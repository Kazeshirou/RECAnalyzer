#pragma once

#include <vector>

#include "mc_entities.hpp"

namespace mc::transaction::algorithm {

enum class ANNOTATION_ENTRY_TYPE { ANY = 0, FULL = 1, PART = 2, MOMENT = 3 };
struct annotation_settings_t {
    ANNOTATION_ENTRY_TYPE type{ANNOTATION_ENTRY_TYPE::ANY};
    double                value{0.0};
};

struct transaction_mining_settings_t {
    std::vector<annotation_settings_t> annotation_settings;
};

class Transaction_mining_interface {
public:
    virtual ~Transaction_mining_interface() = default;

    virtual case_t run(rec::rec_entry_t& rec_entry) const = 0;

    virtual bool event_in_time_interval(
        const rec::rec_entry_t&               rec_entry,
        const rec::rec_entry_t::annotation_t& annotation, size_t ts1,
        size_t ts2) const {
        if (annotation.annotation_id >= settings_.annotation_settings.size()) {
            settings_.annotation_settings.resize(annotation.annotation_id + 1);
        }
        size_t event_ts1 = rec_entry.time_slots[annotation.ts1].value;
        size_t event_ts2 = rec_entry.time_slots[annotation.ts2].value;
        if ((event_ts1 >= ts2) || (event_ts2 <= ts1)) {
            return false;
        }

        auto& annotation_settings =
            settings_.annotation_settings[annotation.annotation_id];
        switch (annotation_settings.type) {
            case ANNOTATION_ENTRY_TYPE::ANY:
                return true;
            case ANNOTATION_ENTRY_TYPE::FULL:
                if ((event_ts1 >= ts1) && (event_ts2 <= ts2)) {
                    return true;
                }
                return false;
            case ANNOTATION_ENTRY_TYPE::PART: {
                if (!annotation_settings.value) {
                    return true;
                }
                int64_t expected_part =
                    (event_ts2 - event_ts1) * annotation_settings.value + 0.5;

                int64_t real_part = ts2 - ts1;
                real_part -= (event_ts1 - ts1);
                real_part -= (ts2 - event_ts2);

                if (real_part < expected_part) {
                    return false;
                }
                return true;
            }

            case ANNOTATION_ENTRY_TYPE::MOMENT: {
                size_t moment =
                    event_ts1 +
                    (event_ts2 - event_ts1) * annotation_settings.value + 0.5;
                if ((moment >= ts1) && (moment <= ts2)) {
                    return true;
                }
                return false;
            }
        }

        return false;
    }

    void set_settings(const transaction_mining_settings_t settings) {
        settings_ = settings;
    }

private:
    mutable transaction_mining_settings_t settings_;
};

}  // namespace mc::transaction::algorithm