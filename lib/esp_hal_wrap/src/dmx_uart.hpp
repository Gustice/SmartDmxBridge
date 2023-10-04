/**
 * @file dmx_uart.hpp
 * @author Gustice
 * @brief Abstraction of UART-interface used as DMX-Channel
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */

#pragma once

#include "configModel.hpp"
#include "uart.hpp"
#include <vector>

constexpr int DmxChannelCount = StageChannelsCount;
constexpr int DmxBufferSize = DmxChannelCount + 1; // buffer + start-code

/**
 * @brief DMX-Port based of UART interface
 * @details Useful information about DMX and the implementation in ESP-IDF can be found
 *  - https://www.soundlight.de/techtips/dmx512/dmx512.htm
 *  - https://mc.mikrocontroller.com/de/dmx512.php
 */
class Dmx512 : Uart {
  public:
    /**
   * @brief DMX-State
   */
    enum class DmxState {
        Idle,  ///< DMX in idle
        Data,  ///< DMX receiving Data
        Break, ///< Break-Signal / Sync-flag
    };

    /**
     * @brief Structure for DMX-State-machine
     */
    struct DmxContext {
        /// @brief Uart module to use
        uart_port_t port;
        /// @brief Buffer to data
        uint8_t *buffer;
        /// @brief Handle for receive queue
        QueueHandle_t rxQueue;
        /// @brief DMX-state
        DmxState state;
        /// @brief Handle for synchronisation semaphore
        xSemaphoreHandle syncRead;
        /// @brief timestamp
        uint32_t timeStamp;
        /// @brief received flag
        bool received;
    };

    /// @brief Constructor
    /// @param port uart port
    /// @param rxd Rx-pin
    /// @param txd Tx-pin
    Dmx512(uart_port_t port, gpio_num_t rxd, gpio_num_t txd);

    /// @brief Send buffered data
    void send();

    /// @brief Set buffer by fixed size array
    /// @note does not send buffer, needs send command
    /// @param dmx payload
    void set(const uint8_t dmx[DmxChannelCount]);

    /// @brief Set buffer by pointer and size
    /// @note does not send buffer, needs send command
    /// @param dmx payload
    /// @param size size of payload
    void set(const uint8_t *dmx, int size);

    /// @brief Set single value
    /// @param channel channel
    /// @param value value
    void set(uint8_t channel, uint8_t value);

    /// @brief Get data from read buffer
    /// @return Payload as vector
    std::vector<uint8_t> receive();

    /// @brief Get buffered data (from send)
    /// @return
    DmxChannels getValues();

  private:
    QueueHandle_t _rxQueue;
    DmxContext _context;

    uint8_t sendBuffer[DmxBufferSize]{0};
    uint8_t receiveBuffer[DmxBufferSize]{0};
};
