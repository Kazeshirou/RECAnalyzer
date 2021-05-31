#include "mc_by_time_slots_mining.hpp"

namespace mc::transaction::algorithm {
case_t By_time_slots_mining::run(rec::rec_entry_t& rec_entry) const {
    case_t new_case;
    if (!rec_entry.annotations.size() || (rec_entry.time_slots.size() < 2)) {
        return new_case;
    }

    for (size_t i{0}; i < rec_entry.time_slots.size() - 1; ++i) {
        auto transaction =
            new transaction_t{rec_entry, rec_entry.time_slots[i].value,
                              rec_entry.time_slots[i + 1].value};
        for (const auto& event : rec_entry.annotations) {
            if (rec_entry.time_slots[event.ts1].value >= transaction->ts2) {
                break;
            }
            if (event_in_time_interval(rec_entry, event, transaction->ts1,
                                       transaction->ts2)) {
                transaction->set_bit(event.annotation_id);
            }
        }
        if (!transaction->ones()) {
            delete transaction;
            continue;
        }
        new_case.push_back(transaction);
    }

    return new_case;
}

}  // namespace mc::transaction::algorithm