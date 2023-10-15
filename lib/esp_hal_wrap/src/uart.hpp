/**
 * @file uart.hpp
 * @author Gustice
 * @brief Abstraction of UART-interface
 * @date 2023-10-04
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "driver/gpio.h"
#include "driver/uart.h"
#include "streams.hpp"
#include <vector>

/**
 * @brief UART-Port
 */
class Uart : public CharStream {
  public:
    /// @brief Enumerator for supported baud rates
    enum BaudRate {
        _9600Bd = 9600,
        _38400Bd = 38400,
        _115200Bd = 115200,
        _250000Bd = 250000,
    };

    /// @brief Enumerator for supported stop-bits
    enum StopBits {
        _1sb = UART_STOP_BITS_1,
        _1_5sb = UART_STOP_BITS_1_5,
        _2sb = UART_STOP_BITS_2,
    };

    /// @brief Constructor
    /// @param port Uart module to use
    /// @param rxd Rx-pin
    /// @param txd Tx-pin
    /// @param rate baudrate to use
    Uart(uart_port_t port, gpio_num_t rxd, gpio_num_t txd, BaudRate rate);

    /// @brief Destructor
    virtual ~Uart();

    /// @brief Read string
    /// @return string
    std::string read() override;

    /// @brief Write character
    /// @param c character to write
    void write(char c) override;

    /// @brief Write string
    /// @param str string to write
    void write(std::string str) override;

    /// @brief Write byte
    /// @param b byte to write
    void write(uint8_t b);

    /// @brief Write series of bytes
    /// @param b Pointer to bytes
    /// @param len Size of series
    void write(uint8_t *b, int len);

    /// @brief Read bytes
    /// @param byteCount bytes to read
    /// @return read bytes
    std::vector<uint8_t> readBytes(int byteCount);

    /// @brief Read bytes
    /// @param byteCount bytes to read
    /// @param timeout timeout to wait
    /// @return read bytes
    std::vector<uint8_t> readBytes(int byteCount, uint32_t timeout);

    /// @brief Change baudrate
    /// @param baud new baudrate
    void changeBaud(BaudRate baud);

  protected:
    const uart_port_t Port;
    const gpio_num_t RxdPin;
    const gpio_num_t TxdPin;
    const uint16_t BufferSize;
    Uart(uart_port_t port, gpio_num_t rxd, gpio_num_t txd, BaudRate rate, uint16_t bufferSize,
         StopBits stopBits);

  private:
    BaudRate _baudRate;
    gpio_num_t _rtsPin = gpio_num_t::GPIO_NUM_NC;
    gpio_num_t _ctsPin = gpio_num_t::GPIO_NUM_NC;

    uint8_t *data;
};
