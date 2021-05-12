#include <gtest/gtest.h>

#include "eaf_parser.hpp"
#include "rec_entry.hpp"
#include "rec_template.hpp"

namespace rec::tests {

TEST(rec_entry_tests, rec_entry_from_eaf) {
    rec_template_t rec_template;

    auto eaf_opt = eaf::Parser::parse_file("src/tests/rec/eaf/test.eaf");
    ASSERT_TRUE(eaf_opt.has_value());

    rec_entry_t rec_entry(rec_template, eaf_opt.value());


    ASSERT_EQ(rec_entry.author, "author1");
    ASSERT_EQ(rec_entry.date, "2021-01-30T11:15:11+03:00");
    ASSERT_EQ(rec_entry.format, "3.0");
    ASSERT_EQ(rec_entry.version, "3.0");
    ASSERT_EQ(rec_entry.time_units, "time_units");


    auto tier = rec_template.tiers_map.find("tid1");
    ASSERT_TRUE(tier != rec_template.tiers_map.end());
    ASSERT_EQ(rec_template.tiers[tier->second].name, "tid1");
    ASSERT_TRUE(rec_template.tiers[tier->second].parent.has_value());
    ASSERT_EQ(rec_template.tiers[tier->second].parent.value(), 2);
    ASSERT_EQ(rec_template.tiers[tier->second].constraint, CONSTRAINT::NONE);

    tier = rec_template.tiers_map.find("tid2");
    ASSERT_TRUE(tier != rec_template.tiers_map.end());
    ASSERT_EQ(rec_template.tiers[tier->second].name, "tid2");
    ASSERT_FALSE(rec_template.tiers[tier->second].parent.has_value());
    ASSERT_EQ(rec_template.tiers[tier->second].constraint,
              CONSTRAINT::INCLUDED_IN);

    tier = rec_template.tiers_map.find("tid3");
    ASSERT_TRUE(tier != rec_template.tiers_map.end());
    ASSERT_EQ(rec_template.tiers[tier->second].name, "tid3");
    ASSERT_FALSE(rec_template.tiers[tier->second].parent.has_value());
    ASSERT_EQ(rec_template.tiers[tier->second].constraint,
              CONSTRAINT::SYMBOLIC_SUBDIVISION);

    auto annotation = rec_template.annotations_map.find("tid1_cv1_0");
    ASSERT_NE(annotation, rec_template.annotations_map.end());
    ASSERT_EQ(rec_template.annotations[annotation->second].value, "cv1_0");
    ASSERT_EQ(rec_template.annotations[annotation->second].description,
              "cv1_0 description");
    ASSERT_EQ(rec_template.annotations[annotation->second].tier, 0);
    annotation = rec_template.annotations_map.find("tid1_cv1_1");
    ASSERT_TRUE(annotation != rec_template.annotations_map.end());
    ASSERT_EQ(rec_template.annotations[annotation->second].value, "cv1_1");
    ASSERT_EQ(rec_template.annotations[annotation->second].description,
              "cv1_1 description");
    ASSERT_EQ(rec_template.annotations[annotation->second].tier, 0);
    annotation = rec_template.annotations_map.find("tid1_cv1_2");
    ASSERT_TRUE(annotation != rec_template.annotations_map.end());
    ASSERT_EQ(rec_template.annotations[annotation->second].value, "cv1_2");
    ASSERT_EQ(rec_template.annotations[annotation->second].description,
              "cv1_2 description");
    ASSERT_EQ(rec_template.annotations[annotation->second].tier, 0);

    annotation = rec_template.annotations_map.find("tid2_cv1_0");
    ASSERT_TRUE(annotation != rec_template.annotations_map.end());
    ASSERT_EQ(rec_template.annotations[annotation->second].value, "cv1_0");
    ASSERT_EQ(rec_template.annotations[annotation->second].description,
              "cv2_0 description");
    ASSERT_EQ(rec_template.annotations[annotation->second].tier, 1);
    annotation = rec_template.annotations_map.find("tid2_cv2_1");
    ASSERT_TRUE(annotation != rec_template.annotations_map.end());
    ASSERT_EQ(rec_template.annotations[annotation->second].value, "cv2_1");
    ASSERT_EQ(rec_template.annotations[annotation->second].description,
              "cv2_1 description");
    ASSERT_EQ(rec_template.annotations[annotation->second].tier, 1);
    annotation = rec_template.annotations_map.find("tid2_cv2_2");
    ASSERT_TRUE(annotation != rec_template.annotations_map.end());
    ASSERT_EQ(rec_template.annotations[annotation->second].value, "cv2_2");
    ASSERT_EQ(rec_template.annotations[annotation->second].description,
              "cv2_2 description");
    ASSERT_EQ(rec_template.annotations[annotation->second].tier, 1);


    ASSERT_EQ(rec_entry.time_slots.size(), 3);
    auto ts = rec_entry.time_slots_map.find("t1");
    ASSERT_NE(ts, rec_entry.time_slots_map.end());
    ASSERT_EQ(rec_entry.time_slots[ts->second].value, 10);
    ASSERT_EQ(ts->second, 0);
    ts = rec_entry.time_slots_map.find("t2");
    ASSERT_NE(ts, rec_entry.time_slots_map.end());
    ASSERT_EQ(rec_entry.time_slots[ts->second].value, 20);
    ASSERT_EQ(ts->second, 1);
    ts = rec_entry.time_slots_map.find("t3");
    ASSERT_NE(ts, rec_entry.time_slots_map.end());
    ASSERT_EQ(rec_entry.time_slots[ts->second].value, 30);
    ASSERT_EQ(ts->second, 2);


    ASSERT_EQ(rec_entry.annotations.size(), 3);
    auto event =
        rec_template.annotations[rec_entry.annotations[0].annotation_id];
    ASSERT_EQ(rec_entry.annotations[0].ts1, 0);
    ASSERT_EQ(rec_entry.annotations[0].ts2, 1);
    ASSERT_EQ(
        rec_template.annotations[rec_entry.annotations[1].annotation_id].value,
        "aval3");
    ASSERT_EQ(rec_entry.annotations[1].ts1, 0);
    ASSERT_EQ(rec_entry.annotations[1].ts2, 2);
    ASSERT_EQ(
        rec_template.annotations[rec_entry.annotations[2].annotation_id].value,
        "aval2");
    ASSERT_EQ(rec_entry.annotations[2].ts1, 1);
    ASSERT_EQ(rec_entry.annotations[2].ts2, 2);
}

}  // namespace rec::tests