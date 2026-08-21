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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "stat_parser.h"
#include "proc_monitor.h"
#include "sample_buffer.h"
#include "serialization.h"
#include "network.h"

static constexpr int DELAY_TIME = 5;
static constexpr std::string FILE_PATH = "/proc/stat";
static constexpr int BUFFER_CAPACITY = 5;
// TODO: Move me into some config/.env file
static constexpr uint16_t PORT = 1305;
static volatile std::sig_atomic_t stop_issued = 0;

static void handle_sig_int([[maybe_unused]] int signal) {
    stop_issued = 1;
}

static uint64_t get_system_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

int main() {
    struct sigaction sa{};
    sa.sa_handler = &handle_sig_int;
    
    sigemptyset(&sa.sa_mask);
    
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        std::cerr << "Failed to create SIGINT sigaction" << std::endl;
        return EXIT_FAILURE;
    }
    
    if (sigaction(SIGTERM, &sa, nullptr) == -1) {
        std::cerr << "Failed to create SIGTERM sigaction" << std::endl;
        return EXIT_FAILURE;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock == -1) {
        std::cerr << "Can't create socket!" << std::endl;
        return EXIT_FAILURE;
    }

    sockaddr_in hint = {};
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr) <= 0) {
        std::cerr << "Failed to call inet_pton" << std::endl;
        close(sock);
        return EXIT_FAILURE;
    }

    if (connect(sock, (struct sockaddr*)&hint, sizeof(hint)) == -1) {
        std::cerr << "Can't connect to server" << std::endl;
        close(sock);
        return EXIT_FAILURE;
    }

    std::cout << "Collecting samples, please wait..." << std::endl;
    
    SampleBuffer buff{BUFFER_CAPACITY};

    while (!stop_issued) {
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

        std::vector<uint8_t> frame = Serialization::serialize(sample);

        if (Network::send_all(
            sock, 
            frame.data(), 
            frame.size(), 
            MSG_NOSIGNAL
        ) == Network::TransmitState::FAIL) {
            std::cerr << "Failed to send frame to server." << std::endl;
            return EXIT_FAILURE;
        }

        sleep(DELAY_TIME);
    }

    std::cout << "Collected " << buff.size() << " samples" << std::endl;

    close(sock);

    return EXIT_SUCCESS;
}