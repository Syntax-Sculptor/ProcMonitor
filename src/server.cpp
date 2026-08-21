/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <arpa/inet.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <unistd.h>
#include <vector>
#include <csignal>

#include "serialization.h"
#include "proc_monitor.h"
#include "network.h"

// TODO: Move me into some config/.env file
static constexpr uint16_t PORT = 1305;

static volatile std::sig_atomic_t stop_issued = 0;

static void handle_sig_int([[maybe_unused]] int signal) {
    stop_issued = 1;
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

    if (inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr) <= 0) {
        std::cerr << "Failed to call inet_pton" << std::endl;
        close(sock);
        return EXIT_FAILURE;
    }

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Failed to set SO_REUSEADDR" << std::endl;
        close(sock);
        return EXIT_FAILURE;
    }

    if (bind(sock, (struct sockaddr*)&hint, sizeof(hint)) == -1) {
        std::cerr << "Can't bind to IP/port" << std::endl;
        close(sock);
        return EXIT_FAILURE;
    }

    if (listen(sock, SOMAXCONN) == -1) {
        std::cerr << "Can't listen" << std::endl;
        close(sock);
        return EXIT_FAILURE;
    }

    int client = accept(sock, NULL, NULL);

    if (client == -1) {
        std::cerr << "Failed to accept client socket" << std::endl;
        close(sock);
        return EXIT_FAILURE;
    }

    std::vector<uint8_t> frame(
        Serialization::PAYLOAD_SIZE + Serialization::HEADER_SIZE
    );

    while (!stop_issued) {
        Network::TransmitState res = Network::recv_safe(
            client, 
            frame.data(), 
            Serialization::PAYLOAD_SIZE + Serialization::HEADER_SIZE
        );

        if (res == Network::TransmitState::FAIL) {
            std::cerr << "Failed to receive client data!" << std::endl;
            close(client);
            close(sock);
            return EXIT_FAILURE;
        }
        else if (res == Network::TransmitState::DISCONNECT) {
            close(client);
            close(sock);
            return EXIT_SUCCESS;
        }
        else if (res == Network::TransmitState::OK) {
            std::optional<ProcMonitor::Sample> sample = 
                Serialization::deserialize(frame);

            if (sample.has_value()) {
                std::cout << sample.value();
            }
        }
    }

    close(client);
    close(sock);
    return EXIT_SUCCESS;
}