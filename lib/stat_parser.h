/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef STAT_PARSER_H
#define STAT_PARSER_H

#include <fstream>
#include <optional>
#include <vector>
#include <string>

#include "proc_monitor.h"

namespace StatParser {
    /** 
     * Parses `lines` into a `CPUTimes` struct.
     * 
     * @param lines The list of lines to parse through. As of now, only
     * the first line is parsed through.
     * @return A `CPUTimes` struct upon successful parsing. If the line is malformed
     * `std::nullopt` is returned.
    */
    std::optional<CPUTimes> parse_lines(const std::vector<std::string>& lines);

    /**
     * Opens a file at `file_path` and parses its contents into a `CPUTimes` struct.
     * 
     * @param file_path The path to the file to open and parse through.
     * @return A `CPUTimes` struct upon successful parsing. If the line is malformed
     * or the file is already open, `std::nullopt` is returned.
     */
    std::optional<CPUTimes> parse_file(const std::string& file_path);

    enum class CPUTimesParserState {
        PARSE_CPU = 0,
        PARSE_USER_TIME,
        PARSE_NICE_TIME,
        PARSE_SYSTEM_TIME,
        PARSE_IDLE_TIME,
        PARSE_IO_WAIT,
        PARSE_IRQ,
        PARSE_SOFT_IRQ,
        PARSE_STEAL_TIME,
        PARSE_GUEST_TIME,
        PARSE_GUEST_NICE_TIME,
    };

    /** 
     * Converts `str` into a `uint64_t`.
     * 
     * @return `uint64_t` if `str` was parsed, `std::nullopt` otherwise.
     */
    std::optional<uint64_t> column_to_uint64_t(const std::string& str);
};

#endif