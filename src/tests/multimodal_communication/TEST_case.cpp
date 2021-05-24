#include <gtest/gtest.h>

#include "mc_entities.hpp"

namespace mc::tests {

TEST(mc_entities_tests, case_test) {
    case_t new_case;

    Bit_mask target0(3);
    target0.set_bit(0);
    Bit_mask target1(3);
    target1.set_bit(1);
    Bit_mask target2(3);
    target2.set_bit(2);

    new_case.push_back(new entry_t(target1));

    auto target_it = new_case.find(target0);
    ASSERT_EQ(target_it, new_case.end());

    target_it = new_case.find(target2);
    ASSERT_EQ(target_it, new_case.end());

    target_it = new_case.find(target1);
    ASSERT_EQ(target_it, new_case.begin());

    new_case.push_back(new entry_t(target0));
    new_case.sort();

    target_it = new_case.find(target0);
    ASSERT_EQ(target_it, new_case.begin());

    target_it = new_case.find(target1);
    ASSERT_EQ(target_it, new_case.begin() + 1);

    target_it = new_case.find(target2);
    ASSERT_EQ(target_it, new_case.end());

    new_case.push_back(new entry_t(target2));
    new_case.sort();

    target_it = new_case.find(target0);
    ASSERT_EQ(target_it, new_case.begin());

    target_it = new_case.find(target1);
    ASSERT_EQ(target_it, new_case.begin() + 1);

    target_it = new_case.find(target2);
    ASSERT_EQ(target_it, new_case.begin() + 2);

    const auto& const_new_case = new_case;

    auto target_const_it = const_new_case.find(target0);
    ASSERT_EQ(target_const_it, const_new_case.begin());

    target_const_it = const_new_case.find(target1);
    ASSERT_EQ(target_const_it, const_new_case.begin() + 1);

    target_const_it = const_new_case.find(target2);
    ASSERT_EQ(target_const_it, const_new_case.begin() + 2);
}

}  // namespace mc::tests