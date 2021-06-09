#include "mc_window_mining.hpp"

namespace mc::transaction::algorithm {

void Window_mining::set_window_settings(
    const window_settings_t& window_settings) {
    window_settings_ = window_settings;
}

case_t Window_mining::run(rec::rec_entry_t& rec_entry) const {
    case_t new_case;
    if (!rec_entry.annotations.size() || (rec_entry.time_slots.size() < 2)) {
        return new_case;
    }

    size_t t1 = 0;
    size_t t2 = t1 + window_settings_.size;
    while (t1 < rec_entry.time_slots[rec_entry.time_slots.size() - 1].value) {
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
        t1 += window_settings_.size * window_settings_.step_multiplier;
        t2 = t1 + window_settings_.size;
    }

    return new_case;
}

}  // namespace mc::transaction::algorithm