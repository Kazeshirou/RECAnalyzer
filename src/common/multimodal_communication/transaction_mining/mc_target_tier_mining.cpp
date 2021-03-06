#include "mc_target_tier_mining.hpp"

namespace mc::transaction::algorithm {

void Target_tier_mining::set_tier_settings(
    const tier_settings_t& tier_settings) {
    tier_settings_ = tier_settings;
}

case_t Target_tier_mining::run(rec::rec_entry_t& rec_entry) const {
    case_t new_case;
    if (!rec_entry.annotations.size() || (rec_entry.time_slots.size() < 2)) {
        return new_case;
    }

    size_t t1 = 0;
    size_t t2 = t1;
    size_t target_tier{0};
    if (rec_entry.rec_template.tiers_map.count(tier_settings_.target_tier)) {
        target_tier =
            rec_entry.rec_template.tiers_map[tier_settings_.target_tier];
    }
    for (const auto& target_event : rec_entry.annotations) {
        size_t tier_id =
            rec_entry.rec_template.annotations[target_event.annotation_id].tier;
        if (tier_id != target_tier) {
            continue;
        }

        if (!tier_settings_.ignore_intervals_without_target_tier) {
            t1 = t2;
            t2 = rec_entry.time_slots[target_event.ts1].value;
            if (t1 != t2) {
                Bit_mask mask(rec_entry.rec_template.annotations.size());
                for (const auto& event : rec_entry.annotations) {
                    if (rec_entry.time_slots[event.ts1].value >= t2) {
                        break;
                    }
                    if (event_in_time_interval(rec_entry, event, t1, t2)) {
                        mask.set_bit(event.annotation_id);
                    }
                }
                if (mask.ones()) {
                    new_case.push_back(new transaction_t{mask, t1, t2});
                }
            }
        }

        t1 = rec_entry.time_slots[target_event.ts1].value;
        t2 = rec_entry.time_slots[target_event.ts2].value;
        Bit_mask mask(rec_entry.rec_template.annotations.size());
        for (const auto& event : rec_entry.annotations) {
            if (rec_entry.time_slots[event.ts1].value >= t2) {
                break;
            }
            if (event_in_time_interval(rec_entry, event, t1, t2)) {
                mask.set_bit(event.annotation_id);
            }
        }
        if (mask.ones()) {
            new_case.push_back(new transaction_t{mask, t1, t2});
        }
    }


    if (!tier_settings_.ignore_intervals_without_target_tier) {
        t1 = t2;
        t2 = rec_entry.time_slots[rec_entry.time_slots.size() - 1].value;
        if (t1 != t2) {
            Bit_mask mask(rec_entry.rec_template.annotations.size());
            for (const auto& event : rec_entry.annotations) {
                if (rec_entry.time_slots[event.ts1].value >= t2) {
                    break;
                }
                if (event_in_time_interval(rec_entry, event, t1, t2)) {
                    mask.set_bit(event.annotation_id);
                }
            }
            if (mask.ones()) {
                new_case.push_back(new transaction_t{mask, t1, t2});
            }
        }
    }

    return new_case;
}

}  // namespace mc::transaction::algorithm