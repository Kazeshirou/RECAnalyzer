#include <gtest/gtest.h>

#include "apriori.hpp"

namespace mc::assotiation_mining::algorithm::tests {

TEST(apriori_tests, apriori) {
    case_t test_case;
    test_case.push_back(new entry_t(3));
    test_case.push_back(new entry_t(3));
    test_case.push_back(new entry_t(3));
    test_case[0]->set_bit(0);
    test_case[0]->set_bit(2);
    test_case[1]->set_bit(1);
    test_case[1]->set_bit(2);
    test_case[2]->set_bit(1);
    auto&& sets = apriori_sets(apriori_settings_t{}, test_case);
    ASSERT_EQ(sets.size(), 5);
    ASSERT_EQ(sets[0]->data()[0], 1);
    ASSERT_NEAR(static_cast<set_t*>(sets[0])->support, 0.3333, 0.0001);
    ASSERT_EQ(sets[1]->data()[0], 5);
    ASSERT_NEAR(static_cast<set_t*>(sets[1])->support, 0.3333, 0.0001);
    ASSERT_EQ(sets[2]->data()[0], 2);
    ASSERT_NEAR(static_cast<set_t*>(sets[2])->support, 0.6666, 0.0001);
    ASSERT_EQ(sets[3]->data()[0], 6);
    ASSERT_NEAR(static_cast<set_t*>(sets[3])->support, 0.3333, 0.0001);
    ASSERT_EQ(sets[4]->data()[0], 4);
    ASSERT_NEAR(static_cast<set_t*>(sets[4])->support, 0.6666, 0.0001);

    sets.sort();

    auto&& rules = apriori_rules(apriori_settings_t{}, sets);
    ASSERT_EQ(rules.size(), 4);
    ASSERT_EQ(rules[0]->data()[0], 5);
    ASSERT_NEAR(static_cast<rule_t*>(rules[0])->support, 0.3333, 0.0001);
    ASSERT_EQ(static_cast<rule_t*>(rules[0])->target, 0);
    ASSERT_NEAR(static_cast<rule_t*>(rules[0])->confidence, 0.5, 0.0001);
    ASSERT_NEAR(static_cast<rule_t*>(rules[1])->support, 0.3333, 0.0001);
    ASSERT_EQ(static_cast<rule_t*>(rules[1])->target, 2);
    ASSERT_NEAR(static_cast<rule_t*>(rules[1])->confidence, 1., 0.0001);
    ASSERT_NEAR(static_cast<rule_t*>(rules[2])->support, 0.3333, 0.0001);
    ASSERT_EQ(static_cast<rule_t*>(rules[2])->target, 1);
    ASSERT_NEAR(static_cast<rule_t*>(rules[2])->confidence, 0.5, 0.0001);
    ASSERT_NEAR(static_cast<rule_t*>(rules[3])->support, 0.3333, 0.0001);
    ASSERT_EQ(static_cast<rule_t*>(rules[3])->target, 2);
    ASSERT_NEAR(static_cast<rule_t*>(rules[3])->confidence, 0.5, 0.0001);
}

}  // namespace mc::assotiation_mining::algorithm::tests