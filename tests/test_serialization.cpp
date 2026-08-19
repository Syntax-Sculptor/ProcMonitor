/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdint>
#include <vector>

#include "sample_buffer.h"
#include "proc_monitor.h"
#include "serialization.h"
#include "CppUTest/TestHarness.h"

TEST_GROUP(Serialization) {

};

TEST(Serialization, rejects_empty_buffer) {
    CHECK_FALSE(Serialization::deserialize({}));
}

TEST(Serialization, rejects_buffer_shorter_than_prefix) {
    CHECK_FALSE(Serialization::deserialize({1, 2, 3}));
}

TEST(Serialization, rejects_truncated_payload) {
    ProcMonitor::Sample sample {
        .timestamp = 170000,
        .times = CPUTimes{
            .user_time = 111,
            .nice_time = 222,
            .system_time = 333,
            .idle_time = 444,
            .io_wait = 555,
            .irq = 666,
            .soft_irq = 777,
            .steal_time = 888,
            .guest_time = 999,
            .guest_nice_time = 1010,
        }
    };
    std::vector<uint8_t> bytes = Serialization::serialize(sample);
    bytes.resize(90);
    CHECK_FALSE(Serialization::deserialize(bytes));
}

TEST(Serialization, rejects_wrong_length_prefix) {
    ProcMonitor::Sample sample {
        .timestamp = 170000,
        .times = CPUTimes{
            .user_time = 111,
            .nice_time = 222,
            .system_time = 333,
            .idle_time = 444,
            .io_wait = 555,
            .irq = 666,
            .soft_irq = 777,
            .steal_time = 888,
            .guest_time = 999,
            .guest_nice_time = 1010,
        }
    };
    std::vector<uint8_t> bytes = Serialization::serialize(sample);
    bytes[3] = 0x67;
    CHECK_FALSE(Serialization::deserialize(bytes));
}

TEST(Serialization, round_trip_preserves_sample) {
    ProcMonitor::Sample sample {
        .timestamp = 170000,
        .times = CPUTimes{
            .user_time = 111,
            .nice_time = 222,
            .system_time = 333,
            .idle_time = 444,
            .io_wait = 555,
            .irq = 666,
            .soft_irq = 777,
            .steal_time = 888,
            .guest_time = 999,
            .guest_nice_time = 1010,
        }
    };

    std::vector<uint8_t> frame = Serialization::serialize(sample);
    std::optional<ProcMonitor::Sample> new_sample = 
        Serialization::deserialize(frame);

    CHECK_TRUE(new_sample.has_value());
    UNSIGNED_LONGLONGS_EQUAL(sample.timestamp, new_sample->timestamp);
    UNSIGNED_LONGLONGS_EQUAL(sample.times.user_time, new_sample->times.user_time);
    UNSIGNED_LONGLONGS_EQUAL(sample.times.nice_time, new_sample->times.nice_time);
    UNSIGNED_LONGLONGS_EQUAL(
        sample.times.system_time, new_sample->times.system_time
    );
    UNSIGNED_LONGLONGS_EQUAL(sample.times.idle_time, new_sample->times.idle_time);
    UNSIGNED_LONGLONGS_EQUAL(sample.times.io_wait, new_sample->times.io_wait);
    UNSIGNED_LONGLONGS_EQUAL(sample.times.irq, new_sample->times.irq);
    UNSIGNED_LONGLONGS_EQUAL(sample.times.soft_irq, new_sample->times.soft_irq);
    UNSIGNED_LONGLONGS_EQUAL(sample.times.steal_time, new_sample->times.steal_time);
    UNSIGNED_LONGLONGS_EQUAL(sample.times.guest_time, new_sample->times.guest_time);
    UNSIGNED_LONGLONGS_EQUAL(
        sample.times.guest_nice_time, 
        new_sample->times.guest_nice_time
    );
}

TEST(Serialization, serializes_sample) {
    ProcMonitor::Sample sample {
        .timestamp = 258,
        .times = CPUTimes{
            .user_time = 1,
            .nice_time = 2,
            .system_time = 3,
            .idle_time = 4,
            .io_wait = 5,
            .irq = 6,
            .soft_irq = 7,
            .steal_time = 8,
            .guest_time = 9,
            .guest_nice_time = 10,
        }
    };

    std::vector<uint8_t> frame = Serialization::serialize(sample);

    // Thanks Claude for writing this extremely long array for me <3.
    const uint8_t expected[] = {
        // length prefix: 88 = 0x00000058
        0x00, 0x00, 0x00, 0x58,
        // timestamp = 258 (0x0102)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
        // user_time = 1
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        // nice_time = 2
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
        // system_time = 3
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
        // idle_time = 4
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
        // io_wait = 5
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
        // irq = 6
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06,
        // soft_irq = 7
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
        // steal_time = 8
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
        // guest_time = 9
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09,
        // guest_nice_time = 10 (0x0A)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
    };

    UNSIGNED_LONGS_EQUAL(sizeof(expected), frame.size());
    MEMCMP_EQUAL(expected, frame.data(), sizeof(expected));
}