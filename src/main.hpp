/**
 * @file main.hpp
 * @author Gustice
 * @brief Main header
 * @details Concentrates includes and device specific configuration objects
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "BinDiff.hpp"
#include "fileAccess.hpp"
#include "paramReader.hpp"
#include "ScaledValue.hpp"
#include "adc.hpp"
#include "cliWrapper.hpp"
#include "displayWrapper.hpp"
#include "dmx_uart.hpp"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "etherInit.hpp"
#include "nvs.h"
#include "nvs_flash.h"
#include "otaUpdate.hpp"
#include "ratioControl.hpp"
#include "semaphore.hpp"
#include "tcpSocket.hpp"
#include "uart.hpp"
#include "udpSocket.hpp"
#include <Artnet.h>
#include <algorithm>
#include <array>
#include <iostream>
#include <lwip/netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>

/**
 * @brief Configuration structure for Device Hardware IO
 */
struct DeviceIoMap {
    /**
     * @brief Configuration structure of Uart Port
     */
    struct UartMap {
        uart_port_t port;
        gpio_num_t rxPin;
        gpio_num_t txPin;
    };

    /**
     * @brief Configuration structure of ADC Port
     */
    struct AdcMap {
        adc_unit_t unit;
        adc_channel_t port;
    };

    /// @brief Configuration of DMX-Hardware
    UartMap dmx;
    /// @brief Configuration of serial display interface
    UartMap display;
    /// @brief Configuration of potentiometer for illumination
    AdcMap intensity;
    /// @brief Configuration of potentiometer for ambient color brightness
    AdcMap ambiente;
};

/**
 * @brief Token for control of logging task
 */
struct LogSession {
    /// @brief Latest client as destination for syslog
    sockaddr_in latestClient;
    /// @brief Terminate flag
    bool terminate;
};

/**
 * @brief Log message structure
 */
struct LogMessage {
    /// @brief Message type enumeration
    enum Type {
        Event, ///< Event
        Meas,  ///< Measurement (new values)
        Error, ///< Error
    };
    /// @brief Message type
    Type type;
    /// @brief Message payload
    std::string message;

    /// @brief Constructor for make_unique calls
    /// @param t message type
    /// @param msg message payload
    LogMessage(Type t, std::string msg)
        : type(t), message(msg) {}
};
