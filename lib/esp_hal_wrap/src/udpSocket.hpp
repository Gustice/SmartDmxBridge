/**
 * @file udpSocket.hpp
 * @author Gustice
 * @brief Abstraction of UDP sockets for convenient usage
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */

#pragma once

#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "ip.hpp"
#include "streams.hpp"
#include <array>

/**
 * @brief UDP-Socket abstraction
 * @details Maintains UDF-Socket and provides stream functions
 */
class UdpSocket : public ByteStream, public VolatileStream {
  public:
    /// @brief Constructor
    /// @note Socket not active until call of attach method
    /// @param dest Destination address
    /// @param device Device info (source)
    UdpSocket(const sockaddr_in &dest, IpInfo &device)
        : dest_addr(dest), Device(device) {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE("UDP", "Unable to create socket: errno %d", errno);
            return; // @todo Exception
        }
        ESP_LOGI("UDP", "Socket created");
    }

    /// @brief Destructor
    ~UdpSocket() {
        active = false;
        ESP_LOGE("UDP", "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }

    /// @brief Attach to configured socket
    /// @note Blocks until successful or failed
    /// @return true if successful
    bool attach() {
        int err = bind(sock, (struct sockaddr *)&dest_addr, dest_addr.sin_len);
        if (err < 0) {
            ESP_LOGE("UDP", "Socket unable to bind: errno %d", errno);
            active = false;
            return false; // @todo Exception
        }
        ESP_LOGI("UDP", "Socket bound, port %d", ntohs(dest_addr.sin_port));

        active = true;
        return active;
    }

    /// @brief Read from socket
    /// @note This call blocks until data is received
    /// @return read bytes
    std::vector<uint8_t> read() override {
        char buffer[256];
        int len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&source_addr,
                           &socklen);

        if (len < 0) {
            ESP_LOGE("UDP", "recvfrom failed: errno %d", errno);
            return {};
        }

        std::vector<uint8_t> ret(buffer, buffer + len);
        return ret;
    }

    /// @brief Write character to socket
    /// @param b character to write
    void write(uint8_t b) override {}

    /// @brief Write bytes to socket
    /// @param data payload as series of bytes
    void write(std::vector<uint8_t> data) override {
        write(&data[0], data.size());
    }

    /// @brief Write string to specific socket
    /// @param message payload
    /// @param dest Destination for message
    void write(std::string &message, sockaddr_in &dest) {
        int err = sendto(sock, message.data(), message.length(), 0, (struct sockaddr *)&dest, sizeof(dest));
        if (err < 0) {
            ESP_LOGE("UDP", "Error occurred during sending: errno %d", errno);
            active = false;
        }
    }

    /// @brief Write string to specific socket or to last source
    /// @param data payload
    /// @param size size of data
    /// @param addr Destination for message (if nullptr it will be send to last source)
    void write(uint8_t *data, uint size, IpAddress *addr = nullptr) {
        // Get the sender's ip address as string
        int err;
        // @todo works for needed case but is not finished
        if (addr == nullptr) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str,
                        sizeof(addr_str) - 1);
            ESP_LOGI("UDP", "Received %d bytes from %s:", size, addr_str);
            err = sendto(sock, data, size, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
        } else {
            auto dest = source_addr;
            ((struct sockaddr_in *)&dest)->sin_addr.s_addr = addr->asWord();
            err = sendto(sock, data, size, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
        }

        if (err < 0) {
            ESP_LOGE("UDP", "Error occurred during sending: errno %d", errno);
            active = false;
        }
    }

    /// @brief Standard destination address
    const sockaddr_in &dest_addr;

    /// @brief Device info
    const IpInfo &Device;

    /// @brief query if stream is still active
    /// @return true if active
    bool isValid() {
        return sock >= 0;
    }

  private:
    struct sockaddr_storage source_addr; // Remote Station
    int sock;
    socklen_t socklen = sizeof(source_addr);

    char addr_str[128];
};