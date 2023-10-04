/**
 * @file tcpSocket.hpp
 * @author Gustice
 * @brief Abstraction of TCP sockets for convenient usage
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "streams.hpp"
#include "tcpSocket.hpp"
#include <lwip/netdb.h>

#include "ip.hpp"

/**
 * @brief TCP-Socket
 */
class TcpSocket : public VolatileStream {
  public:
    /// @brief Constructor
    /// @note Socket not active until call of attach method
    /// @param dest Destination address info
    /// @param device Device info (source)
    TcpSocket(const sockaddr_in &dest, IpInfo &device)
        : dest_addr(dest), Device(device) {
        listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (listen_sock < 0) {
            ESP_LOGE("TCP", "Unable to create socket: errno %d", errno);
            return; // @todo Exception
        }

        int opt = 1;
        setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        ESP_LOGI("TCP", "Socket created");
    }

    /// @brief Destructor
    ~TcpSocket() {
        close(listen_sock);
    }

    /// @brief Attach to configured socket
    /// @note Blocks until successful or failed
    /// @return true if successful
    bool attach() {
        int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE("TCP", "Socket unable to bind: errno %d", errno);
            ESP_LOGE("TCP", "IPPROTO: %d", AF_INET);
            return false; // @todo Exception => close(listen_sock);
        }
        ESP_LOGI("TCP", "Socket bound, port %d", ntohs(dest_addr.sin_port));

        err = listen(listen_sock, 1);
        if (err != 0) {
            ESP_LOGE("TCP", "Error occurred during listen: errno %d", errno);
            return false; // @todo Exception => close(listen_sock);
        }
        active = true;
        return active;
    }

    /// @brief Check if socket is active
    /// @return true if active
    bool isActive() override {
        return active;
    }

    /// @brief Get socket id for stream-operations
    /// @return Id of socket
    int getSocketId() {
        return listen_sock;
    }

    /// @brief Address of this station (server perspective)
    const sockaddr_in &dest_addr;
    /// @brief Address of remote station (server perspective)
    const IpInfo &Device;

  private:
    int listen_sock;
};

/**
 * @brief TCP session
 */
class TcpSession : public CharStream, public VolatileStream {
  public:
    /**
     * @brief Configuration structure for Session
     */
    struct Config {
        int keepAlive;
        int keepIdle;
        int keepInterval;
        int keepCount;
    };

    /// @brief Constructor
    /// @note Constructor blocks
    /// @param config Configuration for session
    /// @param listener reference to socket
    TcpSession(Config &config, TcpSocket &listener) {
        socklen_t addr_len = sizeof(source_addr);
        _socket = accept(listener.getSocketId(), (struct sockaddr *)&source_addr, &addr_len);
        if (_socket < 0) {
            ESP_LOGE("TCP", "Unable to accept connection: errno %d", errno);
            active = false;
            return; // @todo exception
        }

        // Set tcp keepalive option
        setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, &config.keepAlive, sizeof(int));
        setsockopt(_socket, IPPROTO_TCP, TCP_KEEPIDLE, &config.keepIdle, sizeof(int));
        setsockopt(_socket, IPPROTO_TCP, TCP_KEEPINTVL, &config.keepInterval, sizeof(int));
        setsockopt(_socket, IPPROTO_TCP, TCP_KEEPCNT, &config.keepCount, sizeof(int));

        char _addr_str[24];
        inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, _addr_str,
                    sizeof(_addr_str) - 1);
        ESP_LOGI("TCP", "Socket accepted ip address: %s", _addr_str);

        active = true;
    }

    /// @brief Destructor
    ~TcpSession() {
        if (_socket < 0)
            return;

        ESP_LOGI("TCP", "Tearing down socket");
        shutdown(_socket, 0); // @todo something goes wrong on tidy up
        close(_socket);
    }

    /// @brief Get IP of source
    /// @return
    sockaddr_in getSource() {
        return source_addr;
    }

    /// @brief Read string
    /// @note This call blocks until data is received
    /// @return read string
    std::string read() override {
        int len = recv(_socket, _data, sizeof(_data) - 1, 0);

        if (len < 0) {
            ESP_LOGE("TCP", "Error occurred during receiving: errno %d", errno);
            active = false;
            return "";
        }

        if (len == 0) {
            ESP_LOGW("TCP", "Connection closed");
            active = false;
            return "";
        }

        _data[len] = 0;
        int i = 0;
        // @todo still specific implementation here
        while (_data[i] == 0xFF) { // while telnet command
            i += 3;                // skip this command
        }

        if (i == len) {                   // only configuration
            send(_socket, _data, len, 0); // just mirrow it ... don't have a clue of what i am doing
            return "";
        }

        ESP_LOGD("TCP", "Received %d bytes: %s", len, _data);
        std::string ret((const char *)&_data[i]);
        return ret;
    }

    /// @brief Write character to socket
    /// @param b character to write
    void write(char c) override {
        int written = send(_socket, &c, 1, 0);
        if (written < 0) {
            ESP_LOGE("TCP", "Error occurred during sending: errno %d", errno);
        }
    }

    /// @brief Write string to specific socket
    /// @param message payload
    /// @param dest Destination for message
    void write(std::string message) override {
        int written = send(_socket, message.c_str(), message.size(), 0);
        if (written < 0) {
            ESP_LOGE("TCP", "Error occurred during sending: errno %d", errno);
        }
    }

  private:
    int _port;
    int _socket = 0;
    uint8_t _data[128];
    sockaddr_in source_addr;
};
