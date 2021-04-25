#include <gtest/gtest.h>

#include "etf_parser.hpp"
#include "rec_template_json_file.hpp"

namespace rec::files::json::tests {

TEST(rec_template_json_files_tests, rec_template_json_files_tests) {
    auto etf_opt = etf::Parser::parse_file("src/tests/rec/etf/test.etf");
    ASSERT_TRUE(etf_opt.has_value());

    rec_template_t rec_template = etf_opt.value();

    ASSERT_TRUE(to_file(rec_template, "bin/test_rec_template.json"));

    rec_template_t parsed_rec_template;

    ASSERT_TRUE(from_file("bin/test_rec_template.json", parsed_rec_template));
    ASSERT_EQ(rec_template, parsed_rec_template);
}

}  // namespace rec::files::json::tests