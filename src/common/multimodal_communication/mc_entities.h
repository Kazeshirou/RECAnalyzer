#pragma once

#include <vector>

#include "bit_mask.hpp"
#include "rec_entry.hpp"

namespace rec::mc {

using entry_t = Bit_mask;

struct template_t : public entry_t {};

struct transaction_t : public entry_t {
    rec_entry_t& rec_entry;
    size_t       ts1;
    size_t       ts2;
};

struct case_t : public std::vector<entry_t*> {
    ~case_t() {
        for (auto& entry : *this) {
            delete entry;
        }
    }
};

}  // namespace rec::mc