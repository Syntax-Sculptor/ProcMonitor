/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SAMPLE_BUFFER_H
#define SAMPLE_BUFFER_H

#include <cstdint>
#include <vector>
#include <queue>

#include "proc_monitor.h"

class SampleBuffer {
public:
    /**
     * Constructs a new `SampleBuffer` with the given `capacity`. Samples added are
     * FIFO.
     * 
     * @param capacity How many `Sample`s the buffer can hold.
     * @throws `std::invalid_argument` if capacity is 0
     */
    explicit SampleBuffer(std::size_t capacity);

    /**
     * Adds a new Sample to the buffer. If the buffer's capacity is full, then
     * the oldest sample added is dropped.
     */
    void push(const ProcMonitor::Sample& sample);

    /**
     * Empties the buffer and returns all of the samples inside.
     * 
     * @return The samples removed from the buffer.
     */
    std::vector<ProcMonitor::Sample> drain();

    /**
     * @return How many samples are inside the buffer.
     */
    std::size_t size() const;

    /**
     * @return Whether the buffer is empty.
     */
    bool empty() const;

    /**
     * @return Whether the buffer is full.
     */
    bool full() const;
private:
    std::queue<ProcMonitor::Sample> samples;
    std::size_t capacity;
};

#endif