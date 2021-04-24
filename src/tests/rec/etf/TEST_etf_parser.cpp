#include <gtest/gtest.h>

#include "etf_parser.hpp"

namespace rec::etf::tests {

TEST(rec_etf_parser_tests, parser_header) {
    auto etf_opt = Parser::parse_file("src/tests/rec/etf/test.etf");
    ASSERT_TRUE(etf_opt.has_value());

    auto etf = etf_opt.value();


    ASSERT_EQ(etf.annotation.author, "author1");
    ASSERT_EQ(etf.annotation.date, "2021-01-30T11:15:11+03:00");
    ASSERT_EQ(etf.annotation.format, "3.0");
    ASSERT_EQ(etf.annotation.version, "3.0");


    ASSERT_EQ(etf.header.media_file, "media_file");
    ASSERT_EQ(etf.header.time_units, "time_units");


    auto tier = etf.tiers.find("tid1");
    ASSERT_TRUE(tier != etf.tiers.end());
    ASSERT_EQ(tier->second.id, "tid1");
    ASSERT_EQ(tier->second.parent_ref, "tid3");
    ASSERT_EQ(tier->second.linguistic_type_ref, "lt1");

    tier = etf.tiers.find("tid2");
    ASSERT_TRUE(tier != etf.tiers.end());
    ASSERT_EQ(tier->second.id, "tid2");
    ASSERT_EQ(tier->second.linguistic_type_ref, "lt2");

    tier = etf.tiers.find("tid3");
    ASSERT_TRUE(tier != etf.tiers.end());
    ASSERT_EQ(tier->second.id, "tid3");
    ASSERT_EQ(tier->second.linguistic_type_ref, "lt3");


    auto ling_type = etf.linguistic_types.find("lt1");
    ASSERT_TRUE(ling_type != etf.linguistic_types.end());
    ASSERT_EQ(ling_type->second.id, "lt1");
    ASSERT_EQ(ling_type->second.vocabulary_ref, "cv1");

    ling_type = etf.linguistic_types.find("lt2");
    ASSERT_TRUE(ling_type != etf.linguistic_types.end());
    ASSERT_EQ(ling_type->second.id, "lt2");
    ASSERT_EQ(ling_type->second.vocabulary_ref, "cv2");
    ASSERT_EQ(ling_type->second.constraint_refs.size(), 1);
    ASSERT_EQ(ling_type->second.constraint_refs[0], "Included_In");

    ling_type = etf.linguistic_types.find("lt3");
    ASSERT_TRUE(ling_type != etf.linguistic_types.end());
    ASSERT_EQ(ling_type->second.id, "lt3");
    ASSERT_EQ(ling_type->second.constraint_refs.size(), 1);
    ASSERT_EQ(ling_type->second.constraint_refs[0], "Symbolic_Subdivision");


    auto constr = etf.constraints.find("Symbolic_Subdivision");
    ASSERT_TRUE(constr != etf.constraints.end());
    ASSERT_EQ(constr->second.id, "Symbolic_Subdivision");
    ASSERT_EQ(constr->second.description, "constr1 description");

    constr = etf.constraints.find("Included_In");
    ASSERT_TRUE(constr != etf.constraints.end());
    ASSERT_EQ(constr->second.id, "Included_In");
    ASSERT_EQ(constr->second.description, "constr2 description");


    auto vocabulary = etf.vocabularies.find("cv1");
    ASSERT_TRUE(vocabulary != etf.vocabularies.end());
    ASSERT_EQ(vocabulary->second.id, "cv1");
    ASSERT_EQ(vocabulary->second.description, "cv1 description");
    auto voc_entry = vocabulary->second.entries.find("cv1_0");
    ASSERT_TRUE(voc_entry != vocabulary->second.entries.end());
    ASSERT_EQ(voc_entry->second.id, "cv1_0");
    ASSERT_EQ(voc_entry->second.description, "cv1_0 description");
    voc_entry = vocabulary->second.entries.find("cv1_1");
    ASSERT_TRUE(voc_entry != vocabulary->second.entries.end());
    ASSERT_EQ(voc_entry->second.id, "cv1_1");
    ASSERT_EQ(voc_entry->second.description, "cv1_1 description");
    voc_entry = vocabulary->second.entries.find("cv1_2");
    ASSERT_TRUE(voc_entry != vocabulary->second.entries.end());
    ASSERT_EQ(voc_entry->second.id, "cv1_2");
    ASSERT_EQ(voc_entry->second.description, "cv1_2 description");

    vocabulary = etf.vocabularies.find("cv2");
    ASSERT_TRUE(vocabulary != etf.vocabularies.end());
    ASSERT_EQ(vocabulary->second.id, "cv2");
    ASSERT_EQ(vocabulary->second.description, "cv2 description");
    voc_entry = vocabulary->second.entries.find("cv2_0");
    ASSERT_TRUE(voc_entry != vocabulary->second.entries.end());
    ASSERT_EQ(voc_entry->second.id, "cv2_0");
    ASSERT_EQ(voc_entry->second.description, "cv2_0 description");
    voc_entry = vocabulary->second.entries.find("cv2_1");
    ASSERT_TRUE(voc_entry != vocabulary->second.entries.end());
    ASSERT_EQ(voc_entry->second.id, "cv2_1");
    ASSERT_EQ(voc_entry->second.description, "cv2_1 description");
    voc_entry = vocabulary->second.entries.find("cv2_2");
    ASSERT_TRUE(voc_entry != vocabulary->second.entries.end());
    ASSERT_EQ(voc_entry->second.id, "cv2_2");
    ASSERT_EQ(voc_entry->second.description, "cv2_2 description");
}

}  // namespace rec::etf::tests