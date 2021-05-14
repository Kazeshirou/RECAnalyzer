#include <gtest/gtest.h>

#include "eaf_parser.hpp"
#include "mc_by_time_slots_mining.hpp"
#include "rec_entry.hpp"
#include "rec_template.hpp"

namespace mc::transaction::algorithm::tests {

TEST(mc_by_time_slots_mining_tests, mc_by_time_slots_mining) {
    rec::rec_template_t rec_template;

    auto eaf_opt = rec::eaf::Parser::parse_file("src/tests/rec/eaf/test.eaf");
    ASSERT_TRUE(eaf_opt.has_value());

    rec::rec_entry_t rec_entry(rec_template, eaf_opt.value());

    By_time_slots_mining alg;
    auto&&               test_case = alg.run(rec_entry);
    ASSERT_EQ(test_case.size(), rec_entry.time_slots.size() - 1);
    ASSERT_EQ(test_case[0]->size(), rec_template.annotations.size());
    Bit_mask tran(rec_template.annotations.size());
    tran.set_bit(rec_template.annotations_map.at("tid1_aval1"));
    tran.set_bit(rec_template.annotations_map.at("tid2_aval3"));
    ASSERT_EQ(*test_case[0], tran);
    tran.data()[0] = 0ul;
    tran.set_bit(rec_template.annotations_map.at("tid1_aval2"));
    tran.set_bit(rec_template.annotations_map.at("tid2_aval3"));
    ASSERT_EQ(*test_case[1], tran);
}

}  // namespace mc::transaction::algorithm::tests