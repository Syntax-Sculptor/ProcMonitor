/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PROC_MONITOR_H
#define PROC_MONITOR_H

#include <optional>
#include <cstdint>
#include <iostream>

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
    /**
     * Returns the total elapsed time by summing together all CPU time counters
     * except for `guest` and `guest_nice`, as they are already a part of `user` and
     * `nice`.
     * 
     * @return The total elapsed CPU time.
     */
    uint64_t compute_total_time(const CPUTimes& times);
    
    /**
     * Returns the total CPU time of `idle` and `io_wait` combined.
     * 
     * @return The total elapsed idle CPU time.
     */
    uint64_t compute_idle_time(const CPUTimes& times);

    /**
     * Computes the system utilization between two points with a range of 0-100.
     * 
     * @return The system utilization. If `curr` is less than `prev`, `std::nullopt`
     *         is returned instead.
     */
    std::optional<double> compute_utilization(
        const CPUTimes& prev, 
        const CPUTimes& curr
    );

    struct Sample {
        uint64_t timestamp;
        CPUTimes times;
        bool operator==(const Sample&) const = default;
        friend std::ostream& operator<<(std::ostream &os, const Sample& sample);
    };
};

#endif