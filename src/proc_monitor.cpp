/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */


#include <optional>
#include <cstdint>

#include "proc_monitor.h"

namespace ProcMonitor {
    uint64_t compute_total_time(const CPUTimes& times) {
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

    uint64_t compute_idle_time(const CPUTimes& times) {
        return times.idle_time + times.io_wait;
    }

    std::optional<double> compute_utilization(
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

    std::ostream& operator<<(std::ostream &os, const Sample& sample) {
        os << "SAMPLE Time: " << sample.timestamp << std::endl;
        os << "User Time: " << sample.times.user_time << std::endl;
        os << "Nice Time: " << sample.times.nice_time << std::endl;
        os << "System Time: " << sample.times.system_time << std::endl;
        os << "Idle Time: " << sample.times.idle_time << std::endl;
        os << "IO Wait: " << sample.times.io_wait << std::endl;
        os << "IRQ: " << sample.times.irq << std::endl;
        os << "Soft IRQ: " << sample.times.soft_irq << std::endl;
        os << "Steal Time: " << sample.times.steal_time << std::endl;
        os << "Guest Time: " << sample.times.guest_time << std::endl;
        os << "Guest Nice Time: " << sample.times.guest_nice_time << std::endl;

        return os;
    }
};