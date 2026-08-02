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

#include "stat_parser.h"
#include "proc_monitor.h"

static const int DELAY_TIME = 5;
static const std::string FILE_PATH = "/proc/stat";

int main() {
    std::optional<CPUTimes> prev = StatParser::parse_file(FILE_PATH);

    if (!prev) {
        std::cerr << "Failed to open " << FILE_PATH << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Computing system utilization, please wait..." << std::endl;
    
    sleep(DELAY_TIME);

    std::optional<CPUTimes> curr = StatParser::parse_file(FILE_PATH);

    if (!curr) {
        std::cerr << "Failed to open " << FILE_PATH << std::endl;
        return EXIT_FAILURE;
    }

    std::optional<double> utilization = ProcMonitor::compute_utilization(
        prev.value(), 
        curr.value()
    );

    if (!utilization) {
        std::cerr << "Unexpected error calculating system utilization" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "System Utilization after " << DELAY_TIME << " seconds: "
        << utilization.value() << "%" << std::endl;

    return EXIT_SUCCESS;
}