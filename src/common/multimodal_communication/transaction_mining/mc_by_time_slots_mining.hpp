#pragma once

#include "mc_transaction_mining_interface.hpp"

namespace mc::transaction::algorithm {

class By_time_slots_mining : public Transaction_mining_interface {
public:
    virtual ~By_time_slots_mining() = default;
    case_t run(rec::rec_entry_t& rec_entry) const override;
};

}  // namespace mc::transaction::algorithm