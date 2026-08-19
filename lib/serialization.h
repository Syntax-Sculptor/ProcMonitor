/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <cstdint>
#include <vector>
#include <optional>

#include "sample_buffer.h"

namespace Serialization {
    std::vector<uint8_t> serialize(const ProcMonitor::Sample& s);
    std::optional<ProcMonitor::Sample> deserialize(
        const std::vector<uint8_t>& frame
    );
    constexpr int PAYLOAD_SIZE = 88;
};

#endif