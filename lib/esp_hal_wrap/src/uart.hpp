#pragma once

#include "driver/gpio.h"
#include "driver/uart.h"
#include <string>

class Uart {
  public:
    enum BaudRate {
        _115200Bd = 115200,
        _250000Bd = 250000,
    };

    Uart(uart_port_t port, gpio_num_t rxd, gpio_num_t txd, BaudRate rate);
    ~Uart();

    std::string read();
    void write(char c);

  private:
    uart_port_t _port;
    gpio_num_t _rxdPin;
    gpio_num_t _txdPin;
    BaudRate _baudRate;
    gpio_num_t _rtsPin = gpio_num_t::GPIO_NUM_NC;
    gpio_num_t _ctsPin = gpio_num_t::GPIO_NUM_NC;

    uint8_t *data;
};
