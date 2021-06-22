#include "apriori.hpp"

#include <functional>
#include <vector>

namespace mc::assotiation_mining::algorithm {

case_t apriori_sets(const apriori_settings_t& settings,
                    const case_t&             transactions) {
    struct node_t {
        node_t(size_t size) : children(size, nullptr) {}
        ~node_t() {
            for (auto child : children) {
                if (child) {
                    delete child;
                }
            }
        }
        size_t               value{0};
        std::vector<node_t*> children;
    };
    size_t size = transactions[0]->size();
    node_t hash(size);
    hash.value = transactions.size();
    case_t sets;

    for (auto& transaction : transactions) {
        node_t* node = &hash;
        for (size_t i{0}; i < size; i++) {
            if (!transaction->check_bit(i)) {
                continue;
            }

            if (!node->children[i]) {
                node->children[i] = new node_t(size);
            }

            node = node->children[i];
            ++node->value;
        }
    }

    std::function<void(const node_t&, Bit_mask)> add_to_sets =
        [&](const node_t& current_node, Bit_mask current_mask) {
            double support = current_node.value / double(transactions.size());
            if (support < settings.min_support) {
                return;
            }
            if (support <= settings.max_support) {
                sets.push_back(new set_t{current_mask, support});
            }
            for (size_t i{0}; i < size; i++) {
                if (!current_node.children[i]) {
                    continue;
                }

                Bit_mask child_mask = current_mask;
                child_mask.set_bit(i);
                add_to_sets(*current_node.children[i], child_mask);
            }
        };

    add_to_sets(hash, Bit_mask(size));

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