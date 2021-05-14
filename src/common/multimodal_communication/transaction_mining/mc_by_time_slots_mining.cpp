#include "mc_by_time_slots_mining.hpp"

namespace mc::transaction::algorithm {
case_t By_time_slots_mining::run(rec::rec_entry_t& rec_entry) const {
    case_t new_case;
    if (!rec_entry.annotations.size() || (rec_entry.time_slots.size() < 2)) {
        return new_case;
    }

    for (size_t i{0}; i < rec_entry.time_slots.size() - 1; ++i) {
        auto transaction = new transaction_t{rec_entry, i, i + 1};
        for (const auto& event : rec_entry.annotations) {
            if (event.ts1 >= transaction->ts2) {
                break;
            }
            if ((event.ts1 <= transaction->ts1) &&
                (event.ts2 >= transaction->ts2)) {
                transaction->set_bit(event.annotation_id);
            }
        }
        new_case.push_back(transaction);
    }

    return new_case;
}

}  // namespace mc::transaction::algorithm