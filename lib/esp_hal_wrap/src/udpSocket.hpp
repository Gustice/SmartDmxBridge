#pragma once

#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "streams.hpp"
#include <array>

class IpAddress {
  public:
    IpAddress(uint32_t ipV4) {
        _address.dword = ipV4;
    }

    IpAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) {
        _address.bytes[0] = b0;
        _address.bytes[1] = b1;
        _address.bytes[2] = b2;
        _address.bytes[3] = b3;
    }
    uint8_t operator[](int index) const {
        return _address.bytes[index];
    };
    uint32_t asWord() const {
        return _address.dword;
    }

  private:
    union {
        uint8_t bytes[4]; // IPv4 address
        uint32_t dword;
    } _address;
};

struct IpInfo {
    IpAddress address {0u};
    IpAddress subnet {0u};
    IpAddress gateway {0u};
    std::array<uint8_t, 6> macAddress;

    IpAddress getBradcastAddress() const {
        auto sn = address.asWord() | ~subnet.asWord();
        return IpAddress(sn);
    }
};

class UdpSocket : public ByteStream, public VolatileStream {
  public:
    UdpSocket(sockaddr_in &dest, IpInfo &device) : dest_addr(dest), Device(device) {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE("UDP", "Unable to create socket: errno %d", errno);
            return; // todo Exception
        }
        ESP_LOGI("UDP", "Socket created");

        int err = bind(sock, (struct sockaddr *)&dest_addr, dest_addr.sin_len);
        if (err < 0) {
            ESP_LOGE("UDP", "Socket unable to bind: errno %d", errno);
            return; // todo Exception
        }
        ESP_LOGI("UDP", "Socket bound, port %d", ntohs(dest_addr.sin_port));

        active = true;
    }

    ~UdpSocket() {
        ESP_LOGE("UDP", "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
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

    sockaddr_in &dest_addr;              // This Station
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