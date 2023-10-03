/**
 * @file ip.hpp
 * @author Gustice
 * @brief Utility for IP
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */

#pragma once

#include <cstdint>
#include <array>

/**
 * @brief IP-Address class
 */
class IpAddress {
  public:
    /// @brief constructor from unsigned integer
    /// @param ipV4 ip-value unsigned integer
    IpAddress(uint32_t ipV4) {
        _address.dword = ipV4;
    }

    /// @brief constructor from discrete bytes
    /// @param b0 high-byte
    /// @param b3 low-byte
    IpAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) {
        _address.bytes[0] = b0;
        _address.bytes[1] = b1;
        _address.bytes[2] = b2;
        _address.bytes[3] = b3;
    }

    /// @brief Index-operator overload
    /// @param index to byte
    /// @return byte of ip-V4-address
    uint8_t operator[](int index) const {
        return _address.bytes[index];
    };
    
    /// @brief Get IP-address as word
    /// @return 
    uint32_t asWord() const {
        return _address.dword;
    }

  private:
    union {
        uint8_t bytes[4]; // IPv4 address
        uint32_t dword;
    } _address;
};

/**
 * @brief IP-set class
 */
struct IpInfo {
    /// @brief IP address
    IpAddress address {0u};
    /// @brief IP subnet
    IpAddress subnet {0u};
    /// @brief Gateway IP 
    IpAddress gateway {0u};
    /// @brief MAC-Address
    std::array<uint8_t, 6> macAddress;

    /// @brief Get broadcast address from IP and subnet
    /// @return 
    IpAddress getBradcastAddress() const {
        auto sn = address.asWord() | ~subnet.asWord();
        return IpAddress(sn);
    }
};