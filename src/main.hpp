#pragma once

#include "driver/gpio.h"
#include "driver/uart.h"
#include "uart.hpp"
#include "adc.hpp"
#include <array>
#include <algorithm>
#include <lwip/netdb.h>

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

struct LogSession {
    sockaddr_in latestClient;
    bool terminate;
};

struct LogMessage {
    enum Type {
        Event,
        Meas,
        Error,
    };
    LogMessage(Type t, std::string msg) : type(t), message(msg) {}
    Type type;
    std::string message;
};
