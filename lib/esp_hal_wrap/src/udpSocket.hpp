#pragma once

#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"

#include "streams.hpp"

class UdpSocket : public ByteStream, public VolatileStream {
    public: 
    UdpSocket(sockaddr_in& dest) : dest_addr(dest) {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            return; // todo Exception
        }
        ESP_LOGI(TAG, "Socket created");

        int err = bind(sock, (struct sockaddr *)&dest_addr, dest_addr.sin_len);
        if (err < 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            return; // todo Exception
        }
        ESP_LOGI(TAG, "Socket bound, port %d", ntohs(dest_addr.sin_port));

        active = true;
    }

    ~UdpSocket() {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }

    std::vector<uint8_t> read() override {
        char buffer[256];
        int len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                            (struct sockaddr *)&source_addr, &socklen);

        if (len < 0) {
            ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
            return {};
        }

        std::vector<uint8_t> ret(buffer, buffer + len);
        return ret;
    }

    void write(uint8_t b) override {

    }

    void write(std::vector<uint8_t> data) override {
        // Get the sender's ip address as string
        if (source_addr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str,
                        sizeof(addr_str) - 1);
        }

        ESP_LOGI(TAG, "Received %d bytes from %s:", data.size(), addr_str);
        int err = sendto(sock, &data[0], data.size(), 0, (struct sockaddr *)&source_addr,
                            sizeof(source_addr));
        if (err < 0) {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
            active = false;
        }
    }

    int sock;
    bool isValid() {return sock >= 0; }
    private:
    sockaddr_in & dest_addr;
    struct sockaddr_storage source_addr;
    socklen_t socklen = sizeof(source_addr);

    char addr_str[128];
};