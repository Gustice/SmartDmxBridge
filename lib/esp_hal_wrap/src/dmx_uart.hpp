#pragma once

#include "uart.hpp"
#include <vector>
#include "configModel.hpp"

constexpr int DmxChannelCount = StageChannelsCount;
constexpr int DmxBufferSize = DmxChannelCount + 1; // buffer + start-code

// https://www.soundlight.de/techtips/dmx512/dmx512.htm
// http://mc.mikrocontroller.com/de/dmx512.php

class Dmx512 : Uart {
  public:
    enum class DmxState {
        Idle,
        Data,
        Break,
    };

    struct DmxContext {
        uart_port_t port;
        uint8_t *buffer;
        QueueHandle_t rxQueue;
        DmxState state;
        xSemaphoreHandle syncRead;
        uint32_t timeStamp;
        bool received;
    };

    Dmx512(uart_port_t port, gpio_num_t rxd, gpio_num_t txd);
    void send();

    void set(const uint8_t dmx[DmxChannelCount]);
    void set(const uint8_t *dmx, int size);
    void set(uint8_t channel, uint8_t value);

    std::vector<uint8_t> receive();
  
    DmxChannels getValues();
    
  private:
    QueueHandle_t _rxQueue;
    DmxContext _context;

    uint8_t sendBuffer[DmxBufferSize]{0};
    uint8_t receiveBuffer[DmxBufferSize]{0};
};
