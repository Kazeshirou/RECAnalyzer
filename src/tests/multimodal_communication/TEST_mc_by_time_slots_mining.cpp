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

    // ANNOTATION_ENTRY_TYPE::ANY
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


    transaction_mining_settings_t settings;
    settings.annotation_settings.resize(rec_template.annotations.size());
    size_t a1_id = rec_template.annotations_map.at("tid1_aval1");
    rec::rec_entry_t::annotation_t a1{a1_id, 0, 2};
    auto& a1_settings = settings.annotation_settings[a1_id];

    a1_settings.type = ANNOTATION_ENTRY_TYPE::FULL;
    alg.set_settings(settings);
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 0, 10));
    ASSERT_TRUE(alg.event_in_time_interval(rec_entry, a1, 0, 30));

    a1_settings.type  = ANNOTATION_ENTRY_TYPE::PART;
    a1.ts2            = 1;
    a1_settings.value = 0.;
    alg.set_settings(settings);
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 0, 10));
    ASSERT_TRUE(alg.event_in_time_interval(rec_entry, a1, 10, 15));
    a1_settings.value = 0.5;
    alg.set_settings(settings);
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 0, 10));
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 9, 14));
    ASSERT_TRUE(alg.event_in_time_interval(rec_entry, a1, 9, 15));
    ASSERT_TRUE(alg.event_in_time_interval(rec_entry, a1, 10, 30));
    a1_settings.value = 1.;
    alg.set_settings(settings);
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 0, 10));
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 9, 14));
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 9, 15));
    ASSERT_TRUE(alg.event_in_time_interval(rec_entry, a1, 10, 20));

    a1_settings.type  = ANNOTATION_ENTRY_TYPE::MOMENT;
    a1_settings.value = 0.;
    alg.set_settings(settings);
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 0, 9));
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 0, 10));
    ASSERT_TRUE(alg.event_in_time_interval(rec_entry, a1, 10, 15));
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 11, 15));
    a1_settings.value = 0.5;
    alg.set_settings(settings);
    ASSERT_TRUE(alg.event_in_time_interval(rec_entry, a1, 10, 15));
    ASSERT_TRUE(alg.event_in_time_interval(rec_entry, a1, 15, 16));
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 16, 20));
    a1_settings.value = 1.;
    alg.set_settings(settings);
    ASSERT_TRUE(alg.event_in_time_interval(rec_entry, a1, 10, 20));
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 20, 21));
    ASSERT_FALSE(alg.event_in_time_interval(rec_entry, a1, 21, 30));
}

}  // namespace mc::transaction::algorithm::tests