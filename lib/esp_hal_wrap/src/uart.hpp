#pragma once

#include "driver/gpio.h"
#include "driver/uart.h"
#include "streams.hpp"
#include <vector>

class Uart : public CharStream {
  public:
    enum BaudRate {
        _9600Bd = 9600,
        _115200Bd = 115200,
        _250000Bd = 250000,
    };

    enum StopBits {
        _1sb = UART_STOP_BITS_1,
        _1_5sb = UART_STOP_BITS_1_5,
        _2sb = UART_STOP_BITS_2,
    };

    Uart(uart_port_t port, gpio_num_t rxd, gpio_num_t txd, BaudRate rate);
    Uart(uart_port_t port, gpio_num_t rxd, gpio_num_t txd, BaudRate rate, uint16_t bufferSize,
         StopBits stopBits);
    ~Uart();

    std::string read() override;
    void write(char c) override;
    void write(std::string str) override;
    // local functions
    void write(uint8_t b);
    void write(uint8_t *b, int len);
    std::vector<uint8_t> readBytes(int byteCount);
    std::vector<uint8_t> readBytes(int byteCount, uint32_t timeout);

    const uart_port_t Port;
    const gpio_num_t RxdPin;
    const gpio_num_t TxdPin;
    const uint16_t BufferSize;

    QueueHandle_t RxQueue;

  private:
    BaudRate _baudRate;
    gpio_num_t _rtsPin = gpio_num_t::GPIO_NUM_NC;
    gpio_num_t _ctsPin = gpio_num_t::GPIO_NUM_NC;

    uint8_t *data;
    QueueHandle_t *rxQueue;
};
