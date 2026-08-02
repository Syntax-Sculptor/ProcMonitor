/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */


#include <optional>
#include <cstdint>

#include "proc_monitor.h"

uint64_t ProcMonitor::compute_total_time(const CPUTimes& times) {
    return
        times.idle_time +
        times.io_wait +
        times.irq +
        times.nice_time +
        times.soft_irq +
        times.steal_time +
        times.system_time +
        times.user_time;
}

uint64_t ProcMonitor::compute_idle_time(const CPUTimes& times) {
    return times.idle_time + times.io_wait;
}

std::optional<double> ProcMonitor::compute_utilization(
    const CPUTimes& prev, 
    const CPUTimes& curr
) {
    uint64_t total_prev = ProcMonitor::compute_total_time(prev);
    uint64_t total_curr = ProcMonitor::compute_total_time(curr);

    if (total_curr < total_prev) {
        return std::nullopt;
    }

    uint64_t total_delta = total_curr - total_prev;

    if (total_delta == 0) {
        return 0.0;
    }

    uint64_t idle_time_prev = ProcMonitor::compute_idle_time(prev);
    uint64_t idle_time_curr = ProcMonitor::compute_idle_time(curr);

    if (idle_time_curr < idle_time_prev) {
        return std::nullopt;
    }
    
    uint64_t idle_delta = idle_time_curr - idle_time_prev;
    
    double utilization = 100.0 * (total_delta - idle_delta) / total_delta;

    return utilization;
}