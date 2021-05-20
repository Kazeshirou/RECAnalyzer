#pragma once

#include "mc_entities.hpp"

namespace mc::assotiation_mining::algorithm {

struct apriori_settings_t {
    double min_support{0.};
    double max_support{0.99};
    double min_confidence{0.};
    double max_confidence{1.};
};

case_t apriori_sets(const apriori_settings_t& settings,
                    const case_t&             transactions);
case_t apriori_rules(const apriori_settings_t& settings, const case_t& sets);


}  // namespace mc::assotiation_mining::algorithm