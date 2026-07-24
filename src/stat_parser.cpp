/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <optional>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "stat_parser.h"

std::optional<CPUTimes> StatParser::parse_file(const std::string& file_path) {
    std::ifstream stat_file;
    stat_file.open(file_path);

    if (!stat_file.is_open()) {
        return std::nullopt;
    }
    
    std::vector<std::string> lines{};
    std::string line;

    while (std::getline(stat_file, line)) {
        lines.push_back(line);
    }

    return StatParser::parse_lines(lines);
}

std::optional<CPUTimes> StatParser::parse_lines([[maybe_unused]] std::vector<std::string> lines) {
    CPUTimes times {
        .user_time = 0,
        .nice_time = 0,
        .system_time = 0,
        .idle_time = 0,
        .io_wait = 0,
        .irq = 0,
        .soft_irq = 0,
        .steal_time = 0,
        .guest_time = 0,
        .guest_nice_time = 0,
    };



    return times;
}