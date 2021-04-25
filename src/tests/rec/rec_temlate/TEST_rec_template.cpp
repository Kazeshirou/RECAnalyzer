#include <gtest/gtest.h>

#include "etf_parser.hpp"
#include "rec_template.hpp"

namespace rec::tests {

TEST(rec_template_tests, rec_template_from_etf) {
    auto etf_opt = etf::Parser::parse_file("src/tests/rec/etf/test.etf");
    ASSERT_TRUE(etf_opt.has_value());

    rec_template_t rec_template = etf_opt.value();


    ASSERT_EQ(rec_template.author, "author1");
    ASSERT_EQ(rec_template.date, "2021-01-30T11:15:11+03:00");
    ASSERT_EQ(rec_template.format, "3.0");
    ASSERT_EQ(rec_template.version, "3.0");
    ASSERT_EQ(rec_template.time_units, "time_units");


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
    ASSERT_TRUE(annotation != rec_template.annotations_map.end());
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
}

}  // namespace rec::tests