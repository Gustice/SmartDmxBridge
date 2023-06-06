#pragma once

#include <cstdint>
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