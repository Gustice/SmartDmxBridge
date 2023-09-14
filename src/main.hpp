#pragma once

#include "driver/gpio.h"
#include "driver/uart.h"
#include "uart.hpp"
#include <array>
#include <algorithm>

struct DeviceIoMap
{
    struct UartMap {
        uart_port_t port;
        gpio_num_t rxPin;
        gpio_num_t txPin;
    };

    UartMap dmx;
    UartMap display;
};
