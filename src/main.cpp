/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <csignal>
#include <cstdint>
#include <chrono>

#include "stat_parser.h"
#include "proc_monitor.h"
#include "sample_buffer.h"

static const int DELAY_TIME = 5;
static const std::string FILE_PATH = "/proc/stat";
static const int BUFFER_CAPACITY = 5;

static void handle_sig_int(int signal) {
    exit(signal);
}

uint64_t get_system_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

int main() {
    struct sigaction sa{};
    sa.sa_handler = &handle_sig_int;
    sa.sa_flags = SA_RESTART;
    
    sigemptyset(&sa.sa_mask);
    
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        std::cerr << "Failed to create SIGINT sigaction" << std::endl;
        return EXIT_FAILURE;
    };

    std::cout << "Collecting samples, please wait..." << std::endl;
    
    SampleBuffer buff{BUFFER_CAPACITY};

    while (true) {
        std::optional<CPUTimes> times = StatParser::parse_file(FILE_PATH);
        
        if (!times) {
            std::cerr << "Failed to open " << FILE_PATH << std::endl;
            return EXIT_FAILURE;
        }

        ProcMonitor::Sample sample {
            .timestamp = get_system_time(),
            .times = times.value()
        };

        buff.push(sample);

        sleep(DELAY_TIME);
    }

    std::cout << "Collected " << buff.size() << " samples" << std::endl;

    return EXIT_SUCCESS;
}