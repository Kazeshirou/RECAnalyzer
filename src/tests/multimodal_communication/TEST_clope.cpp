#include <gtest/gtest.h>

#include "clope.hpp"

namespace mc::clustering::algorithm::tests {

TEST(clope_tests, clope_test) {
    case_t test_case;
    test_case.push_back(new entry_t(6));
    test_case.push_back(new entry_t(6));
    test_case.push_back(new entry_t(6));
    test_case.push_back(new entry_t(6));
    test_case.push_back(new entry_t(6));

    test_case[0]->data()[0] = 3;
    test_case[1]->data()[0] = 7;
    test_case[2]->data()[0] = 5;
    test_case[3]->data()[0] = 24;
    test_case[4]->data()[0] = 56;

    auto [clusters_count, distribution] = clope(test_case, 2.);
    ASSERT_EQ(clusters_count, 2);
    ASSERT_EQ(static_cast<mc::cluster_t*>(distribution[0])->cluster, 0);
    ASSERT_EQ(static_cast<mc::cluster_t*>(distribution[1])->cluster, 0);
    ASSERT_EQ(static_cast<mc::cluster_t*>(distribution[2])->cluster, 0);
    ASSERT_EQ(static_cast<mc::cluster_t*>(distribution[3])->cluster, 1);
    ASSERT_EQ(static_cast<mc::cluster_t*>(distribution[4])->cluster, 1);
}

}  // namespace mc::clustering::algorithm::tests