#include <gtest/gtest.h>

#include "eaf_parser.hpp"

namespace rec::eaf::tests {

TEST(rec_eaf_parser_tests, parse_file) {
    auto eaf_opt = Parser::parse_file("src/tests/rec/eaf/test.eaf");
    ASSERT_TRUE(eaf_opt.has_value());

    auto eaf = eaf_opt.value();


    ASSERT_EQ(eaf.annotation_document.author, "author1");
    ASSERT_EQ(eaf.annotation_document.date, "2021-01-30T11:15:11+03:00");
    ASSERT_EQ(eaf.annotation_document.format, "3.0");
    ASSERT_EQ(eaf.annotation_document.version, "3.0");


    ASSERT_EQ(eaf.header.media_file, "media_file");
    ASSERT_EQ(eaf.header.time_units, "time_units");

    ASSERT_EQ(eaf.header.media_descriptors.size(), 2);

    ASSERT_EQ(eaf.header.media_descriptors[0].value, "");
    auto attr = eaf.header.media_descriptors[0].attributes.find("ATTR1");
    ASSERT_TRUE(attr != eaf.header.media_descriptors[0].attributes.end());
    ASSERT_EQ(attr->second, "attr1_value");
    attr = eaf.header.media_descriptors[0].attributes.find("ATTR2");
    ASSERT_TRUE(attr != eaf.header.media_descriptors[0].attributes.end());
    ASSERT_EQ(attr->second, "attr2_value");

    ASSERT_EQ(eaf.header.media_descriptors[1].value, "");
    attr = eaf.header.media_descriptors[1].attributes.find("ATTR3");
    ASSERT_TRUE(attr != eaf.header.media_descriptors[1].attributes.end());
    ASSERT_EQ(attr->second, "attr3_value");
    attr = eaf.header.media_descriptors[1].attributes.find("ATTR4");
    ASSERT_TRUE(attr != eaf.header.media_descriptors[1].attributes.end());
    ASSERT_EQ(attr->second, "attr4_value");

    ASSERT_EQ(eaf.header.properties.size(), 2);

    ASSERT_EQ(eaf.header.properties[0].value, "value1");
    attr = eaf.header.properties[0].attributes.find("ATTR");
    ASSERT_TRUE(attr != eaf.header.properties[0].attributes.end());
    ASSERT_EQ(attr->second, "prop_attr1");

    ASSERT_EQ(eaf.header.properties[1].value, "value2");
    attr = eaf.header.properties[1].attributes.find("ATTR");
    ASSERT_TRUE(attr != eaf.header.properties[1].attributes.end());
    ASSERT_EQ(attr->second, "prop_attr2");


    ASSERT_EQ(eaf.time_slots.size(), 3);

    auto ts = eaf.time_slots.find("t1");
    ASSERT_NE(ts, eaf.time_slots.end());
    ASSERT_EQ(ts->second.value, 10);
    ASSERT_EQ(ts->second.id, "t1");

    ts = eaf.time_slots.find("t2");
    ASSERT_NE(ts, eaf.time_slots.end());
    ASSERT_EQ(ts->second.value, 20);
    ASSERT_EQ(ts->second.id, "t2");

    ts = eaf.time_slots.find("t3");
    ASSERT_NE(ts, eaf.time_slots.end());
    ASSERT_EQ(ts->second.value, 30);
    ASSERT_EQ(ts->second.id, "t3");


    auto tier = eaf.tiers.find("tid1");
    ASSERT_NE(tier, eaf.tiers.end());
    ASSERT_EQ(tier->second.id, "tid1");
    ASSERT_EQ(tier->second.parent_ref, "tid3");
    ASSERT_EQ(tier->second.linguistic_type_ref, "lt1");
    ASSERT_EQ(tier->second.annotations.size(), 2);
    ASSERT_EQ(tier->second.annotations[0].id, "a1");
    ASSERT_EQ(tier->second.annotations[0].value, "aval1");
    ASSERT_EQ(tier->second.annotations[0].ts1_ref, "t1");
    ASSERT_EQ(tier->second.annotations[0].ts2_ref, "t2");
    ASSERT_EQ(tier->second.annotations[1].id, "a2");
    ASSERT_EQ(tier->second.annotations[1].value, "aval2");
    ASSERT_EQ(tier->second.annotations[1].ts1_ref, "t2");
    ASSERT_EQ(tier->second.annotations[1].ts2_ref, "t3");

    tier = eaf.tiers.find("tid2");
    ASSERT_NE(tier, eaf.tiers.end());
    ASSERT_EQ(tier->second.id, "tid2");
    ASSERT_EQ(tier->second.linguistic_type_ref, "lt2");
    ASSERT_EQ(tier->second.annotations.size(), 1);
    ASSERT_EQ(tier->second.annotations[0].id, "a3");
    ASSERT_EQ(tier->second.annotations[0].value, "aval3");
    ASSERT_EQ(tier->second.annotations[0].ts1_ref, "t1");
    ASSERT_EQ(tier->second.annotations[0].ts2_ref, "t3");

    tier = eaf.tiers.find("tid3");
    ASSERT_NE(tier, eaf.tiers.end());
    ASSERT_EQ(tier->second.id, "tid3");
    ASSERT_EQ(tier->second.linguistic_type_ref, "lt3");


    auto ling_type = eaf.linguistic_types.find("lt1");
    ASSERT_TRUE(ling_type != eaf.linguistic_types.end());
    ASSERT_EQ(ling_type->second.id, "lt1");
    ASSERT_EQ(ling_type->second.vocabulary_ref, "cv1");

    ling_type = eaf.linguistic_types.find("lt2");
    ASSERT_TRUE(ling_type != eaf.linguistic_types.end());
    ASSERT_EQ(ling_type->second.id, "lt2");
    ASSERT_EQ(ling_type->second.vocabulary_ref, "cv2");
    ASSERT_EQ(ling_type->second.constraint_refs.size(), 1);
    ASSERT_EQ(ling_type->second.constraint_refs[0], "Included_In");

    ling_type = eaf.linguistic_types.find("lt3");
    ASSERT_TRUE(ling_type != eaf.linguistic_types.end());
    ASSERT_EQ(ling_type->second.id, "lt3");
    ASSERT_EQ(ling_type->second.constraint_refs.size(), 1);
    ASSERT_EQ(ling_type->second.constraint_refs[0], "Symbolic_Subdivision");


    auto constr = eaf.constraints.find("Symbolic_Subdivision");
    ASSERT_TRUE(constr != eaf.constraints.end());
    ASSERT_EQ(constr->second.id, "Symbolic_Subdivision");
    ASSERT_EQ(constr->second.description, "constr1 description");

    constr = eaf.constraints.find("Included_In");
    ASSERT_TRUE(constr != eaf.constraints.end());
    ASSERT_EQ(constr->second.id, "Included_In");
    ASSERT_EQ(constr->second.description, "constr2 description");


    auto vocabulary = eaf.vocabularies.find("cv1");
    ASSERT_TRUE(vocabulary != eaf.vocabularies.end());
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

    vocabulary = eaf.vocabularies.find("cv2");
    ASSERT_TRUE(vocabulary != eaf.vocabularies.end());
    ASSERT_EQ(vocabulary->second.id, "cv2");
    ASSERT_EQ(vocabulary->second.description, "cv2 description");
    voc_entry = vocabulary->second.entries.find("cv1_0");
    ASSERT_TRUE(voc_entry != vocabulary->second.entries.end());
    ASSERT_EQ(voc_entry->second.id, "cv1_0");
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

}  // namespace rec::eaf::tests