#pragma once

#include <vector>

#include "bit_mask.hpp"
#include "rec_entry.hpp"

namespace mc {

using entry_t = Bit_mask;

struct template_t : public entry_t {};

struct transaction_t : public entry_t {
    transaction_t(rec::rec_entry_t& new_rec_entry, size_t new_ts1,
                  size_t new_ts2)
            : entry_t(new_rec_entry.rec_template.annotations.size()),
              rec_entry{new_rec_entry},
              ts1{new_ts1},
              ts2{new_ts2} {}
    rec::rec_entry_t& rec_entry;
    size_t            ts1;
    size_t            ts2;
};

struct case_t : public std::vector<entry_t*> {
    ~case_t() {
        for (auto& entry : *this) {
            delete entry;
        }
    }
};

}  // namespace mc