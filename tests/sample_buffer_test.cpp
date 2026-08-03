/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <vector>

#include "sample_buffer.h"
#include "proc_monitor.h"
#include "CppUTest/TestHarness.h"

TEST_GROUP(SampleBuffer) {

};

TEST(SampleBuffer, size_updates_on_push) {
    SampleBuffer buff{3};
    ProcMonitor::Sample sample{
        .timestamp = 0,
        .times = CPUTimes{}
    };

    UNSIGNED_LONGLONGS_EQUAL(0, buff.size());
    buff.push(sample);
    UNSIGNED_LONGLONGS_EQUAL(1, buff.size());
    buff.push(sample);
    UNSIGNED_LONGLONGS_EQUAL(2, buff.size());
}

TEST(SampleBuffer, push_respects_fifo) {
    SampleBuffer buff{3};
    ProcMonitor::Sample sample_1 {
        .timestamp = 1,
        .times = CPUTimes{}
    };
    ProcMonitor::Sample sample_2 {
        .timestamp = 3,
        .times = CPUTimes{}
    };
    ProcMonitor::Sample sample_3 {
        .timestamp = 2,
        .times = CPUTimes{}
    };

    buff.push(sample_1);
    buff.push(sample_2);
    buff.push(sample_3);

    std::vector<ProcMonitor::Sample> drained = buff.drain();
    UNSIGNED_LONGLONGS_EQUAL(sample_1.timestamp, drained.at(0).timestamp);
    UNSIGNED_LONGLONGS_EQUAL(sample_2.timestamp, drained.at(1).timestamp);
    UNSIGNED_LONGLONGS_EQUAL(sample_3.timestamp, drained.at(2).timestamp);
}

TEST(SampleBuffer, overflow_drops_oldest) {
    SampleBuffer buff{2};

    ProcMonitor::Sample sample_1 {
        .timestamp = 1,
        .times = CPUTimes{}
    };
    ProcMonitor::Sample sample_2 {
        .timestamp = 3,
        .times = CPUTimes{}
    };
    ProcMonitor::Sample sample_3 {
        .timestamp = 2,
        .times = CPUTimes{}
    };

    buff.push(sample_1);
    buff.push(sample_2);
    buff.push(sample_3);

    UNSIGNED_LONGLONGS_EQUAL(2, buff.size());

    std::vector<ProcMonitor::Sample> drained = buff.drain();
    UNSIGNED_LONGLONGS_EQUAL(sample_2.timestamp, drained.at(0).timestamp);
    UNSIGNED_LONGLONGS_EQUAL(sample_3.timestamp, drained.at(1).timestamp);
}

TEST(SampleBuffer, drain_returns_empty_vector) {
    SampleBuffer buff{1};
    std::vector<ProcMonitor::Sample> drained = buff.drain();
    UNSIGNED_LONGLONGS_EQUAL(0, drained.size());
}

TEST(SampleBuffer, drain_empties_buffer) {
    SampleBuffer buff{1};
    ProcMonitor::Sample sample{
        .timestamp = 0,
        .times = CPUTimes{}
    };
    buff.push(sample);
    buff.drain();

    UNSIGNED_LONGLONGS_EQUAL(0, buff.size());
}

TEST(SampleBuffer, empty_correctness) {
    SampleBuffer buff{1};
    CHECK_TRUE(buff.empty());
    ProcMonitor::Sample sample{
        .timestamp = 0,
        .times = CPUTimes{},
    };
    buff.push(sample);
    CHECK_FALSE(buff.empty());
}

TEST(SampleBuffer, full_respects_capacity) {
    SampleBuffer buff{2};
    ProcMonitor::Sample sample{
        .timestamp = 0,
        .times = CPUTimes{}
    };
    CHECK_FALSE(buff.full());
    buff.push(sample);
    CHECK_FALSE(buff.full());
    buff.push(sample);
    CHECK_TRUE(buff.full());
}