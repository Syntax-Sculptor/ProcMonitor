/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PROC_MONITOR_H
#define PROC_MONITOR_H

#include <optional>
#include <cstdint>

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
    bool operator==(const CPUTimes&) const = default;
};

namespace ProcMonitor {
    uint64_t compute_total_time(const CPUTimes& times);
    uint64_t compute_idle_time(const CPUTimes& times);

    std::optional<double> compute_utilization(
        const CPUTimes& prev, 
        const CPUTimes& curr
    );

    struct Sample {
        uint64_t timestamp;
        CPUTimes times;
        bool operator==(const Sample&) const = default;
    };
};

#endif