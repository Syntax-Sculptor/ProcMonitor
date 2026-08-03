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
    explicit SampleBuffer(std::size_t capacity);
    void push(const ProcMonitor::Sample& sample);
    std::vector<ProcMonitor::Sample> drain();
    std::size_t size() const;
    bool empty() const;
    bool full() const;
private:
    std::queue<ProcMonitor::Sample> samples;
    std::size_t capacity;
};

#endif