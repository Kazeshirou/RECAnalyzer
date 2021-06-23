#include "apriori.hpp"

#include <functional>
#include <vector>

#define MAX_DEEP 20

namespace mc::assotiation_mining::algorithm {

case_t apriori_sets(const apriori_settings_t& settings,
                    const case_t&             transactions) {
    size_t size = transactions[0]->size();
    case_t sets;
    float  trans_size = transactions.size();

    float                 support = 0;
    std::vector<Bit_mask> prev;
    for (size_t i{0}; i < size; i++) {
        Bit_mask mask{size};
        mask.set_bit(i);
        size_t value{0};
        for (auto& transaction : transactions) {
            if ((mask & *transaction) != mask) {
                continue;
            }

            ++value;
        }
        support = value / trans_size;
        if (support < settings.min_support) {
            continue;
        }
        sets.push_back(new set_t{mask, support});
        prev.push_back(mask);
    }


    size_t level = 2;
    while (prev.size() && (level <= MAX_DEEP)) {
        std::vector<Bit_mask> current;
        for (size_t i{0}; i < prev.size() - 1; i++) {
            for (size_t j{i + 1}; j < prev.size(); j++) {
                auto mask = prev[i] | prev[j];
                if (mask.ones() != level) {
                    continue;
                }
                if (std::find(current.begin(), current.end(), mask) !=
                    current.end()) {
                    continue;
                }
                size_t value{0};
                for (auto& transaction : transactions) {
                    if ((mask & *transaction) != mask) {
                        continue;
                    }

                    ++value;
                }
                support = value / trans_size;
                if (support < settings.min_support) {
                    continue;
                }

                current.push_back(mask);
                sets.push_back(new set_t{mask, support});
            }
        }
        prev = current;
        ++level;
    }

    return sets;
}

case_t apriori_rules(const apriori_settings_t& settings,
                     const case_t&             sorted_sets) {
    case_t rules;
    for (const auto& set : sorted_sets) {
        if (set->ones() <= 1) {
            continue;
        }
        double support = static_cast<const set_t*>(set)->support;
        for (size_t i{0}; i < set->size(); i++) {
            if (!set->check_bit(i)) {
                continue;
            }

            Bit_mask condition(*set);
            condition.set_bit(i, false);

            auto it = sorted_sets.find(condition);
            if (it == sorted_sets.end()) {
                continue;
            }

            double confidence =
                support / static_cast<const set_t*>(*it)->support;
            if ((confidence >= settings.min_confidence) &&
                (confidence <= settings.max_confidence)) {
                rules.push_back(new rule_t(*set, support, confidence, i));
            }
        }
    }
    return rules;
}

}  // namespace mc::assotiation_mining::algorithm