#include <gtest/gtest.h>

#include "eaf_parser.hpp"
#include "rec_entry_json_file.hpp"

namespace rec::files::json::tests {

TEST(rec_entry_json_files_tests, rec_entry_json_files_tests) {
    auto eaf_opt = eaf::Parser::parse_file("src/tests/rec/eaf/test.eaf");
    ASSERT_TRUE(eaf_opt.has_value());

    rec_template_t rec_template;
    rec_entry_t    rec_entry(rec_template, eaf_opt.value());

    ASSERT_TRUE(to_file(rec_entry, "bin/test_rec_entry.json"));

    rec_entry_t parsed_rec_entry(rec_template);

    ASSERT_TRUE(from_file("bin/test_rec_entry.json", parsed_rec_entry));
    ASSERT_EQ(rec_entry, parsed_rec_entry);
}

}  // namespace rec::files::json::tests