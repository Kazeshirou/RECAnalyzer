#pragma once

#include <vector>

#include "mc_entities.hpp"

namespace mc::clustering::algorithm {

struct cluster_t {
    size_t              transaction_count{0};  // N
    size_t              unique_item_count{0};  // W
    size_t              square{0};             // S
    std::vector<size_t> items_occurence;       // Occ
};

std::vector<size_t> clope(const case_t& transactions, double r = 2.);

}  // namespace mc::clustering::algorithm