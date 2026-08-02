#include <cstdint>

#include "proc_monitor.h"
#include "CppUTest/TestHarness.h"

TEST_GROUP(ProcMonitor) {

};

TEST(ProcMonitor, compute_total_time_correct) {
    CPUTimes times {
        .user_time = 144761,
        .nice_time = 0,
        .system_time = 73975,
        .idle_time = 2406189,
        .io_wait = 0,
        .irq = 4593,
        .soft_irq = 0,
        .steal_time = 0,
        .guest_time = 0,
        .guest_nice_time = 0,
    };
    UNSIGNED_LONGS_EQUAL(2629518, ProcMonitor::compute_total_time(times));
}

TEST(ProcMonitor, compute_total_time_ignores_guest_times) {
    // guest_time is a part of user_time and guest_nice_time is a part of nice_time.
    // These fields shouldn't counted in the total at all.
    CPUTimes times {
        .user_time = 144761,
        .nice_time = 0,
        .system_time = 73975,
        .idle_time = 2406189,
        .io_wait = 0,
        .irq = 4593,
        .soft_irq = 0,
        .steal_time = 0,
        .guest_time = 100,
        .guest_nice_time = 200,
    };
    UNSIGNED_LONGS_EQUAL(2629518, ProcMonitor::compute_total_time(times));
}

TEST(ProcMonitor, compute_idle_time_correct) {
    CPUTimes times {
        .user_time = 144761,
        .nice_time = 0,
        .system_time = 73975,
        .idle_time = 15,
        .io_wait = 10,
        .irq = 0,
        .soft_irq = 0,
        .steal_time = 0,
        .guest_time = 0,
        .guest_nice_time = 0,
    };
    UNSIGNED_LONGS_EQUAL(25, ProcMonitor::compute_idle_time(times));
}

TEST(ProcMonitor, compute_utilization_rejects_larger_prev) {
    CPUTimes prev {
        .user_time       = 1150,
        .nice_time       = 0,
        .system_time     = 550,
        .idle_time       = 8800,
        .io_wait         = 0,
        .irq             = 100,
        .soft_irq        = 0,
        .steal_time      = 0,
        .guest_time      = 0,
        .guest_nice_time = 0,
    };

    CPUTimes curr {
        .user_time       = 1000,
        .nice_time       = 0,
        .system_time     = 500,
        .idle_time       = 8000,
        .io_wait         = 0,
        .irq             = 100,
        .soft_irq        = 0,
        .steal_time      = 0,
        .guest_time      = 0,
        .guest_nice_time = 0,
    };
    
    CHECK_FALSE(ProcMonitor::compute_utilization(prev, curr));
}

TEST(ProcMonitor, compute_utilization_correct) {
    CPUTimes prev {
        .user_time       = 1000,
        .nice_time       = 0,
        .system_time     = 500,
        .idle_time       = 8000,
        .io_wait         = 0,
        .irq             = 100,
        .soft_irq        = 0,
        .steal_time      = 0,
        .guest_time      = 0,
        .guest_nice_time = 0,
    };
    CPUTimes curr {
        .user_time       = 1150,
        .nice_time       = 0,
        .system_time     = 550,
        .idle_time       = 8800,
        .io_wait         = 0,
        .irq             = 100,
        .soft_irq        = 0,
        .steal_time      = 0,
        .guest_time      = 0,
        .guest_nice_time = 0,
    };

    std::optional<double> util = ProcMonitor::compute_utilization(prev, curr);

    CHECK_TRUE(util.has_value());
    DOUBLES_EQUAL(20.0, util.value(), 0.01);
}

TEST(ProcMonitor, compute_utilization_identical) {
    CPUTimes times {
        .user_time       = 1000,
        .nice_time       = 0,
        .system_time     = 500,
        .idle_time       = 8000,
        .io_wait         = 0,
        .irq             = 100,
        .soft_irq        = 0,
        .steal_time      = 0,
        .guest_time      = 0,
        .guest_nice_time = 0,
    };

    std::optional<double> util = ProcMonitor::compute_utilization(times, times);

    CHECK_TRUE(util.has_value());
    DOUBLES_EQUAL(0.0, util.value(), 0.01);
}

TEST(ProcMonitor, compute_utilization_rejects_idle_underflow) {
    CPUTimes prev {
        .user_time       = 1000,
        .nice_time       = 0,
        .system_time     = 500,
        .idle_time       = 8000,
        .io_wait         = 0,
        .irq             = 100,
        .soft_irq        = 0,
        .steal_time      = 0,
        .guest_time      = 0,
        .guest_nice_time = 0,
    };
    CPUTimes curr {
        .user_time       = 1150,
        .nice_time       = 0,
        .system_time     = 550,
        .idle_time       = 7999, // 1 below prev.idle_time
        .io_wait         = 0,
        .irq             = 100,
        .soft_irq        = 0,
        .steal_time      = 0,
        .guest_time      = 0,
        .guest_nice_time = 0,
    };

    CHECK_FALSE(ProcMonitor::compute_utilization(prev, curr));
}