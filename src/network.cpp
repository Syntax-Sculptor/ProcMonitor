/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdint>
#include <cstddef>
#include <sys/socket.h>
#include <cerrno>

#include "network.h"

namespace Network {
    TransmitState recv_safe(int sock, uint8_t* buffer, size_t n, int flags) {
        size_t total_read = 0;

        while (total_read < n) {
            ssize_t bytes_read = recv(
                sock, 
                buffer + total_read, 
                n - total_read, 
                flags
            );

            // If the sender disconnects before sending the full frame, we can just
            // treat that as a failure.
            if (bytes_read == -1 || (total_read > 0 && bytes_read == 0)) {
                if (errno == EINTR) {
                    return TransmitState::INTERRUPT;
                }

                return TransmitState::FAIL;
            }
            else if (bytes_read == 0) {
                return TransmitState::DISCONNECT;
            }
            else {
                total_read += bytes_read;
            }
        }

        return TransmitState::OK;
    }
    
    TransmitState send_all(int sock, const uint8_t* data, size_t n, int flags) {
        size_t total_bytes_sent = 0;
        
        while (total_bytes_sent < n) {
            ssize_t sent_bytes = send(
                sock, 
                data + total_bytes_sent, 
                n - total_bytes_sent, 
                flags
            );

            if (sent_bytes == -1) {
                return TransmitState::FAIL;
            }

            total_bytes_sent += sent_bytes;
        }

        return TransmitState::OK;
    }
}