#pragma once

#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "streams.hpp"
#include "tcpSocket.hpp"
#include <lwip/netdb.h>

#include "ip.hpp"

class TcpSocket : public VolatileStream {
  public:
    TcpSocket(sockaddr_in &dest, IpInfo &device) : dest_addr(dest), Device(device) {
        listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (listen_sock < 0) {
            ESP_LOGE("TCP", "Unable to create socket: errno %d", errno);
            return; // todo Exception
        }

        int opt = 1;
        setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        ESP_LOGI("TCP", "Socket created");

        int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE("TCP", "Socket unable to bind: errno %d", errno);
            ESP_LOGE("TCP", "IPPROTO: %d", AF_INET);
            return; // todo Exception => close(listen_sock);
        }
        ESP_LOGI("TCP", "Socket bound, port %d", ntohs(dest_addr.sin_port));

        err = listen(listen_sock, 1);
        if (err != 0) {
            ESP_LOGE("TCP", "Error occurred during listen: errno %d", errno);
            return; // todo Exception => close(listen_sock);
        }

        active = true;
    }

    bool isActive() override {
        return active;
    }


    ~TcpSocket() {
        close(listen_sock);
    }

    int getSocketId() {
        return listen_sock;
    }

    sockaddr_in &dest_addr; // This Station
    const IpInfo &Device;
    struct sockaddr_storage source_addr; // Remote Station

  private:
    int listen_sock;
};

class TcpSession : public CharStream, public VolatileStream {
  public:
    struct Config {
        int keepAlive;
        int keepIdle;
        int keepInterval;
        int keepCount;
    };

    TcpSession(Config &config, TcpSocket listener) {
        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        _socket = accept(listener.getSocketId(), (struct sockaddr *)&source_addr, &addr_len);
        if (_socket < 0) {
            ESP_LOGE("TCP", "Unable to accept connection: errno %d", errno);
            active = false;
            return; // todo exception
        }

        // Set tcp keepalive option
        setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, &config.keepAlive, sizeof(int));
        setsockopt(_socket, IPPROTO_TCP, TCP_KEEPIDLE, &config.keepIdle, sizeof(int));
        setsockopt(_socket, IPPROTO_TCP, TCP_KEEPINTVL, &config.keepInterval, sizeof(int));
        setsockopt(_socket, IPPROTO_TCP, TCP_KEEPCNT, &config.keepCount, sizeof(int));

        inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, _addr_str,
                    sizeof(_addr_str) - 1);
        ESP_LOGI("TCP", "Socket accepted ip address: %s", _addr_str);

        active = true;
    }
    ~TcpSession() {
        if (_socket < 0)
            return;

        ESP_LOGI("TCP", "Tearing down socket");
        shutdown(_socket, 0); // todo something goes wrong on tidy up
        close(_socket);
    }

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
    void write(char c) override {
        int written = send(_socket, &c, 1, 0);
        if (written < 0) {
            ESP_LOGE("TCP", "Error occurred during sending: errno %d", errno);
        }
    }
    void write(std::string str) override {
        int written = send(_socket, str.c_str(), str.size(), 0);
        if (written < 0) {
            ESP_LOGE("TCP", "Error occurred during sending: errno %d", errno);
        }
    }

  private:
    int _port;
    int _socket = 0;
    uint8_t _data[128];
    char _addr_str[128];
};
