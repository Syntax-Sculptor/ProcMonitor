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
#include <sstream>
#include <system_error>
#include <charconv>
#include <cstdint>

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

std::optional<uint64_t> StatParser::column_to_uint64_t(const std::string& str) {
    if (str.empty() || str.at(0) == '-') {
        return std::nullopt;
    }

    uint64_t value;

    auto [ptr, err] = std::from_chars(str.data(), str.data() + str.size(), value);

    if (err == std::errc{} && ptr == str.data() + str.size()) {
        return value;
    }
    else {
        return std::nullopt;
    }
}

static bool parse_column(const std::string& column, uint64_t& val) {
    std::optional<uint64_t> res = StatParser::column_to_uint64_t(column);

    if (!res) {
        return false;
    }
    else {
        val = res.value();
        return true;
    }
}

std::optional<CPUTimes> StatParser::parse_lines(const std::vector<std::string>& lines) {
    if (lines.size() == 0) {
        return std::nullopt;
    }

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

    using StatParser::CPUTimesParserState;

    CPUTimesParserState state = CPUTimesParserState::PARSE_CPU;

    std::istringstream i_stream(lines[0]);
    std::string column;

    while (i_stream >> column) {
        switch (state) {
            case CPUTimesParserState::PARSE_CPU: {
                if (column != "cpu") {
                    return std::nullopt;
                }

                state = CPUTimesParserState::PARSE_USER_TIME;

                break;
            }
            case CPUTimesParserState::PARSE_USER_TIME: {
                bool res = parse_column(column, times.user_time);

                if (!res) {
                    return std::nullopt;
                }

                state = CPUTimesParserState::PARSE_NICE_TIME;

                break;
            }
            case CPUTimesParserState::PARSE_NICE_TIME: {
                bool res = parse_column(column, times.nice_time);

                if (!res) {
                    return std::nullopt;
                }

                state = CPUTimesParserState::PARSE_SYSTEM_TIME;
                
                break;
            }
            case CPUTimesParserState::PARSE_SYSTEM_TIME: {
                bool res = parse_column(column, times.system_time);

                if (!res) {
                    return std::nullopt;
                }

                state = CPUTimesParserState::PARSE_IDLE_TIME;

                break;
            }
            case CPUTimesParserState::PARSE_IDLE_TIME: {
                bool res = parse_column(column, times.idle_time);

                if (!res) {
                    return std::nullopt;
                }

                state = CPUTimesParserState::PARSE_IO_WAIT;

                break;
            }
            case CPUTimesParserState::PARSE_IO_WAIT: {
                bool res = parse_column(column, times.io_wait);

                if (!res) {
                    return std::nullopt;
                }

                state = CPUTimesParserState::PARSE_IRQ;

                break;
            }
            case CPUTimesParserState::PARSE_IRQ: {
                bool res = parse_column(column, times.irq);

                if (!res) {
                    return std::nullopt;
                }

                state = CPUTimesParserState::PARSE_SOFT_IRQ;

                break;
            }
            case CPUTimesParserState::PARSE_SOFT_IRQ: {
                bool res = parse_column(column, times.soft_irq);

                if (!res) {
                    return std::nullopt;
                }

                state = CPUTimesParserState::PARSE_STEAL_TIME;

                break;
            }
            case CPUTimesParserState::PARSE_STEAL_TIME: {
                bool res = parse_column(column, times.steal_time);

                if (!res) {
                    return std::nullopt;
                }

                state = CPUTimesParserState::PARSE_GUEST_TIME;

                break;
            }
            case CPUTimesParserState::PARSE_GUEST_TIME: {
                bool res = parse_column(column, times.guest_time);

                if (!res) {
                    return std::nullopt;
                }

                state = CPUTimesParserState::PARSE_GUEST_NICE_TIME;

                break;
            }
            case CPUTimesParserState::PARSE_GUEST_NICE_TIME: {
                bool res = parse_column(column, times.guest_nice_time);

                if (res) {
                    return times;
                }
                else {
                    return std::nullopt;
                }

                break;
            }
            default: {
                return std::nullopt;
            }
        }
    }

    // If the parser is on this state, it means no tokens were consumed
    // due to it being whitespace only. Reject.
    if (state == CPUTimesParserState::PARSE_CPU) {
        return std::nullopt;
    }
    else {
        return times;
    }
}