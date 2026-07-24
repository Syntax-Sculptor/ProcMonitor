#ifndef STAT_PARSER_H
#define STAT_PARSER_H

#include <fstream>
#include <optional>
#include <cstdint>
#include <vector>
#include <string>

struct CPUTimes {
    uint64_t user_time;
    uint64_t nice_time;
    uint64_t system_time;
    uint64_t idle_time;
    uint64_t io_wait;
    uint64_t irq;
    uint64_t soft_irq;
    uint64_t steal_time;
    uint64_t guest_time;
    uint64_t guest_nice_time;    
};

namespace StatParser {
    std::optional<CPUTimes> parse_lines(const std::vector<std::string>& lines);
    std::optional<CPUTimes> parse_file(const std::string& file_path);
};

#endif