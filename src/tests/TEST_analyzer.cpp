#include <gtest/gtest.h>

#include "analyzer.hpp"

namespace tests {

TEST(analyzer_tests, analyzer) {
    Analyzer analyzer;
    ASSERT_TRUE(analyzer.run("src/tests/analysis_cfg.json"));
}

}  // namespace tests