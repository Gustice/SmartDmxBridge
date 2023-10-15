/**
 * @file Streams.hpp
 * @author Gustice
 * @brief Abstraction of stream
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once

#include <string>
#include <vector>

/**
 * @brief Simplified abstraction for stream as basis for different interfaces
 */
class VolatileStream {
  public:
    /// @brief Get if stream is active
    /// @return true if active
    virtual bool isActive() {
        return active;
    }

  protected:
    bool active;
};

/**
 * @brief Common interface for character streams
 */
class CharStream {
  public:
    /// @brief Read method
    /// @return string
    virtual std::string read() = 0;

    /// @brief Write character
    /// @param c character to write
    virtual void write(char c) = 0;

    /// @brief Write string
    /// @param str string to write
    virtual void write(std::string str) = 0;
};

/**
 * @brief Common interface 
 */
class ByteStream {
    /// @brief Read method
    /// @return series of bytes
    virtual std::vector<uint8_t> read() = 0;

    /// @brief write single byte
    /// @param b byte to write
    virtual void write(uint8_t b) = 0;

    /// @brief write series of bytes
    /// @param data series to write
    virtual void write(std::vector<uint8_t> data) = 0;
};