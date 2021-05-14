#pragma once

#include "mc_entities.hpp"

namespace mc::transaction::algorithm {

class Transaction_mining_interface {
public:
    virtual ~Transaction_mining_interface() = default;

    virtual case_t run(rec::rec_entry_t& rec_entry) const = 0;
};

}  // namespace mc::transaction::algorithm