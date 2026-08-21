/*
 * Syntax-Sculptor
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <cstdint>
#include <cstddef>

namespace Network {
    enum class TransmitState {
        OK = 0,
        FAIL,
        DISCONNECT,
        INTERRUPT
    };

    TransmitState recv_safe(
        int socket, 
        uint8_t* buffer, 
        size_t length, 
        int flags = 0
    );

    TransmitState send_all(
        int sock, 
        const uint8_t* data, 
        size_t n, 
        int flags = 0
    );
}

#endif