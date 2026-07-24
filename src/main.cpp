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

#include "stat_parser.h"

int main() {
    if (!StatParser::parse_file("/proc/stat")) {
        std::cerr << "Failed to open /proc/stat" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}