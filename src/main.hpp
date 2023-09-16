#pragma once

#include "driver/gpio.h"
#include "driver/uart.h"
#include "uart.hpp"
#include "adc.hpp"
#include <array>
#include <algorithm>

struct DeviceIoMap
{
    struct UartMap {
        uart_port_t port;
        gpio_num_t rxPin;
        gpio_num_t txPin;
    };

    struct AdcMap {
        adc_unit_t unit;
        adc_channel_t port;
    };

    UartMap dmx;
    UartMap display;
    AdcMap intensity;
    AdcMap ambiente;
};
