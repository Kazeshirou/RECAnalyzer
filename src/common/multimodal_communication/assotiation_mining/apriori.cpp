#include "apriori.hpp"

#include <functional>
#include <map>
#include <vector>

namespace mc::assotiation_mining::algorithm {

case_t apriori_sets(const apriori_settings_t& settings,
                    const case_t&             transactions) {
    struct node_t {
        size_t                   value{-1ull};
        std::vector<entry_t*>    transactions;
        std::map<size_t, node_t> children;
    };
    size_t size = transactions[0]->size();
    node_t hash;
    case_t sets;

    for (auto& transaction : transactions) {
        std::vector<node_t*> current_nodes;
        hash.transactions.push_back(transaction);
        current_nodes.push_back(&hash);
        for (size_t i{0}; i < size; i++) {
            if (!transaction->check_bit(i)) {
                continue;
            }
            size_t current_size = current_nodes.size();
            for (size_t j{0}; j < current_size; j++) {
                if (current_nodes[j]->children[i].value == -1ull) {
                    current_nodes[j]->children[i].value = i;
                }
                current_nodes[j]->children[i].transactions.push_back(
                    transaction);
                current_nodes.push_back(&(current_nodes[j]->children[i]));
            }
        }
    }

    std::function<void(node_t&, Bit_mask)> add_to_sets =
        [&](node_t& current_node, Bit_mask current_mask) {
            double support =
                current_node.transactions.size() / double(transactions.size());
            if (support < settings.min_support) {
                return;
            }
            if (support <= settings.max_support) {
                sets.push_back(new set_t{current_mask, support});
            }
            for (auto& child : current_node.children) {
                Bit_mask child_mask = current_mask;
                child_mask.set_bit(child.first);
                add_to_sets(child.second, child_mask);
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