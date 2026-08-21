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
    /**
     * Serializes `s` into an array of bytes.
     * 
     * @return An array of bytes serialized from `s`.
     */
    std::vector<uint8_t> serialize(const ProcMonitor::Sample& s);

    /**
     * Deserializes `frame` into a `Sample`.
     * 
     * @param frame An array of bytes from a serialized `Sample`.
     * @return `Sample` upon successful deserialization, `std::nullopt` if the size of
     * `frame` is too large/small or if the frame's prefix length is wrong..
     */
    std::optional<ProcMonitor::Sample> deserialize(
        const std::vector<uint8_t>& frame
    );
    
    constexpr size_t PAYLOAD_SIZE = 88;
    constexpr size_t HEADER_SIZE = 4;
};

#endif