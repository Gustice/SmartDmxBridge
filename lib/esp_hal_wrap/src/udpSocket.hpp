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

sockaddr_in createIpV4Config(uint32_t ip, uint16_t port) {
    return  {
            .sin_len = sizeof(sockaddr_in), .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr{
                .s_addr = htonl(ip),
            },
            .sin_zero {
                0
            }
        };
}

class UdpSocket : public ByteStream, public VolatileStream {
  public:
    UdpSocket(const sockaddr_in &dest, IpInfo &device) : dest_addr(dest), Device(device) {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE("UDP", "Unable to create socket: errno %d", errno);
            return; // todo Exception
        }
        ESP_LOGI("UDP", "Socket created");
        active = true;
    }

    ~UdpSocket() {
        ESP_LOGE("UDP", "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }

    void attach() {
        int err = bind(sock, (struct sockaddr *)&dest_addr, dest_addr.sin_len);
        if (err < 0) {
            ESP_LOGE("UDP", "Socket unable to bind: errno %d", errno);
            active = false;
            return; // todo Exception
        }
        ESP_LOGI("UDP", "Socket bound, port %d", ntohs(dest_addr.sin_port));
    }

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

    void write(uint8_t b) override {}

    void write(std::vector<uint8_t> data) override {

        write(&data[0], data.size());
    }

    void write(std::string & message, sockaddr_in & dest) {
        int err = sendto(sock, message.data(), message.length(), 0, (struct sockaddr *)&dest, sizeof(dest));
        if (err < 0) {
            ESP_LOGE("UDP", "Error occurred during sending: errno %d", errno);
            active = false;
        }
    }

    void write(uint8_t *data, uint size, IpAddress *addr = nullptr) {
        // Get the sender's ip address as string
        int err;
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

    const sockaddr_in &dest_addr;
    const IpInfo &Device;
    int sock;
    bool isValid() {
        return sock >= 0;
    }
    struct sockaddr_storage source_addr; // Remote Station
  private:
    socklen_t socklen = sizeof(source_addr);

    char addr_str[128];
};