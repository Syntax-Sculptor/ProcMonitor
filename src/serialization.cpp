/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdint>
#include <vector>
#include <optional>

#include "sample_buffer.h"
#include "serialization.h"

static void append_uint64(uint64_t num, std::vector<uint8_t>& bytes) {
    for (int i = 7; i >= 0; i--) {
        bytes.push_back((num >> (i * 8)) & 0xFF);
    }
}

static void append_uint32(uint32_t num, std::vector<uint8_t>& bytes) {
    for (int i = 3; i >= 0; i--) {
        bytes.push_back((num >> (i * 8)) & 0xFF);
    }
}

static uint64_t read_uint64(const std::vector<uint8_t>& bytes, std::size_t offset) {
    uint64_t result = 0;

    for (int i = 0; i < 8; i++) {
        result = (result << 8) | bytes[offset + i];
    }

    return result; 
}

static uint32_t read_uint32(const std::vector<uint8_t>& bytes, std::size_t offset) {
    uint32_t result = 0;

    for (int i = 0; i < 4; i++) {
        result = (result << 8) | bytes[offset + i];
    }

    return result; 
}

std::vector<uint8_t> Serialization::serialize(const ProcMonitor::Sample& sample) {
    std::vector<uint8_t> payload{};
    std::vector<uint8_t> frame{};

    append_uint64(sample.timestamp, payload);
    append_uint64(sample.times.user_time, payload);
    append_uint64(sample.times.nice_time, payload);
    append_uint64(sample.times.system_time, payload);
    append_uint64(sample.times.idle_time, payload);
    append_uint64(sample.times.io_wait, payload);
    append_uint64(sample.times.irq, payload);
    append_uint64(sample.times.soft_irq, payload);
    append_uint64(sample.times.steal_time, payload);
    append_uint64(sample.times.guest_time, payload);
    append_uint64(sample.times.guest_nice_time, payload);
    append_uint32((uint32_t) payload.size(), frame);

    for (const uint8_t b : payload) {
        frame.push_back(b);
    }

    return frame;
}

std::optional<ProcMonitor::Sample> Serialization::deserialize(
    const std::vector<uint8_t>& frame
) {
    // If the frame is under 4 bytes, the payload is incomplete (unknown size)
    if (frame.size() < 4) {
        return std::nullopt;
    }

    uint32_t length = read_uint32(frame, 0);

    // Make sure frame contains all fields needed.
    if (length != Serialization::PAYLOAD_SIZE || frame.size() != 4 + length) {
        return std::nullopt;
    }

    ProcMonitor::Sample sample = {
        .timestamp = read_uint64(frame, 4),
        .times = CPUTimes {
            .user_time = read_uint64(frame, 12),
            .nice_time = read_uint64(frame, 20),
            .system_time = read_uint64(frame, 28),
            .idle_time = read_uint64(frame, 36),
            .io_wait = read_uint64(frame, 44),
            .irq = read_uint64(frame, 52),
            .soft_irq = read_uint64(frame, 60),
            .steal_time = read_uint64(frame, 68),
            .guest_time = read_uint64(frame, 76),
            .guest_nice_time = read_uint64(frame, 84),
        }
    };

    return sample;
}