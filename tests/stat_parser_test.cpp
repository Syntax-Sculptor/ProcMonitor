/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <string>
#include <vector>

#include "stat_parser.h"
#include "CppUTest/TestHarness.h"

TEST_GROUP(StatParser) {

};

TEST(StatParser, rejects_non_existant_file) {
    CHECK_FALSE(StatParser::parse_file("this is not a real file...!!!!!!"));
}

TEST(StatParser, rejects_invalid_text) {
    CHECK_FALSE(StatParser::parse_lines({"foo bar baz"}));
}

TEST(StatParser, rejects_empty_vector) {
    CHECK_FALSE(StatParser::parse_lines({}));
}

TEST(StatParser, rejects_non_numeric_columns) {
    CHECK_FALSE(StatParser::parse_lines({"cpu 869179 0 339263 140893a64 0 22400 0 0 0 0"}));
}

TEST(StatParser, rejects_empty_line) {
    CHECK_FALSE(StatParser::parse_lines({""}));
}

TEST(StatParser, rejects_whitespace_only_line) {
    CHECK_FALSE(StatParser::parse_lines({"               "}));
}

TEST(StatParser, rejects_negative_columns) {
    CHECK_FALSE(StatParser::parse_lines({"cpu -869179 0 339263 -14089364 0 22400 0 0 0 0"}));
}

TEST(StatParser, accepts_line) {
    std::optional<CPUTimes> times = StatParser::parse_lines({"cpu 869179 0 339263 14089364 0 22400 0 0 0 0"});
    
    CHECK_TRUE(times.has_value());
    UNSIGNED_LONGS_EQUAL(869179, times->user_time);
    UNSIGNED_LONGS_EQUAL(0, times->nice_time);
    UNSIGNED_LONGS_EQUAL(339263, times->system_time);
    UNSIGNED_LONGS_EQUAL(14089364, times->idle_time);
    UNSIGNED_LONGS_EQUAL(0, times->io_wait);
    UNSIGNED_LONGS_EQUAL(22400, times->irq);
    UNSIGNED_LONGS_EQUAL(0, times->soft_irq);
    UNSIGNED_LONGS_EQUAL(0, times->steal_time);
    UNSIGNED_LONGS_EQUAL(0, times->guest_time);
    UNSIGNED_LONGS_EQUAL(0, times->guest_nice_time);
}