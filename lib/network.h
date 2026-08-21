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
    /**
     * Represents the status of a network transfer function call.
     */
    enum class TransmitState {
        /** The data was read/written successfully. */
        OK = 0,

        /** The data wasn't read/written successfully. */
        FAIL,

        /** The client had disconnected. Only used for `Network::recv_safe`. */
        DISCONNECT,

        /** An exit signal was issued. Only used for `Network::recv_safe`. */
        INTERRUPT
    };

    /** 
     * Safely reads `n` bytes from `sock` and writes them into `buffer`.
     * Accounts for cases where `recv` receives fewer bytes than requested.
     * 
     * @param sock The socket to read the data from.
     * @param buffer Where the received data is written to. Must be at least 
     *               `n` bytes.
     * @param n Number of bytes to read into `buffer`.
     * @param flags Flags passed to the underlying `recv` call.
     * @return The result of receiving the data.
     *         * OK = Data was fully read.
     *         * FAIL = Failed to read data from `sock` or it was closed with some
     *           data read.
     *         * DISCONNECT = `sock` was closed and no data was read.
     *         * INTERRUPT = An interrupting signal was issued.
     */
    TransmitState recv_safe(
        int sock, 
        uint8_t* buffer, 
        size_t n, 
        int flags = 0
    );


    /** 
     * Safely sends `n` bytes from `data` to `sock`. Accounts for cases where 'send'
     * sends fewer bytes than requested.
     * 
     * @param sock The socket to write the data to.
     * @param data The data to transmit. Must be at least `n` bytes.
     * @param n How many bytes of `data` to send.
     * @param flags Flags passed to the underlying `send` call.
     * @return The result of sending the data.
     *         * OK = Data was fully sent.
     *         * FAIL = Failed to send data to `sock`.
     */
    TransmitState send_all(
        int sock, 
        const uint8_t* data, 
        size_t n, 
        int flags = 0
    );
}

#endif