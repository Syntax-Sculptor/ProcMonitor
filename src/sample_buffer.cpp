/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdint>
#include <stdexcept>

#include "sample_buffer.h"

SampleBuffer::SampleBuffer(std::size_t capacity) {
    if (capacity == 0) {
        throw std::invalid_argument("Expected nonzero capacity");
    }

    this->samples = {};
    this->capacity = capacity;
}

void SampleBuffer::push(const ProcMonitor::Sample& sample) {
    if (this->samples.size() == capacity) {
        this->samples.pop();
    }
    
    this->samples.push(sample);
}

std::vector<ProcMonitor::Sample> SampleBuffer::drain() {
    std::vector<ProcMonitor::Sample> emptied{};

    while (this->samples.size() > 0) {
        ProcMonitor::Sample element = this->samples.front();
        emptied.push_back(element);
        this->samples.pop();
    }

    return emptied;
}

bool SampleBuffer::empty() const {
    return this->samples.size() == 0;
}

bool SampleBuffer::full() const {
    return this->samples.size() == capacity;
}

std::size_t SampleBuffer::size() const {
    return this->samples.size();
}