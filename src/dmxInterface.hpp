#pragma once

// https://www.soundlight.de/techtips/dmx512/dmx512.htm
// http://mc.mikrocontroller.com/de/dmx512.php

#include "driver/timer.h"
#include "esp_log.h"
#include "uart.hpp"
#include <string.h>

constexpr int DmxChannelCount = 48;
constexpr int DmxBufferSize = DmxChannelCount + 1; // buffer + start-code

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
    xSemaphoreHandle semaphore;
    uint32_t timeStamp;
    bool received;
};

void dmx_receive_task(void *pvParameters) {
    DmxContext *context = (DmxContext *)pvParameters;
    uint16_t idx = 0;
    uint8_t buff[256]{0};
    while (true) {
        uart_event_t event;
        if (xQueueReceive(context->rxQueue, (void *)&event, (portTickType)1000)) {
            switch (event.type) {
            case UART_DATA:
                if (context->state == DmxState::Break) {
                    if (buff[0] == 0) {
                        context->state = DmxState::Data;
                        idx = 0;
                        context->timeStamp = xTaskGetTickCount();
                    }
                }
                uart_read_bytes(context->port, buff, event.size, portMAX_DELAY);
                if (context->state == DmxState::Data) {
                    xSemaphoreTake(context->semaphore, portMAX_DELAY);
                    for (int i = 0; i < event.size; i++) {
                        if (idx < DmxBufferSize) {
                            context->buffer[idx++] = buff[i];
                        }
                        context->received = idx >= DmxBufferSize;
                    }
                    xSemaphoreGive(context->semaphore);
                }
                break;
            case UART_BREAK:
                uart_flush_input(context->port);
                xQueueReset(context->rxQueue);
                context->state = DmxState::Break;
                break;
            case UART_FRAME_ERR:
            case UART_PARITY_ERR:
            case UART_BUFFER_FULL:
            case UART_FIFO_OVF:
            default:
                uart_flush_input(context->port);
                xQueueReset(context->rxQueue);
                context->state = DmxState::Idle;
                break;
            }
        }
    }
}

class DmxInterface {
  public:
    DmxInterface(Uart &port) : _port(port) {
        sendBuffer[0] = 0; // Start-Code

        for (size_t i = 0; i < DmxChannelCount + 1; i++) {
            sendBuffer[i] = 0;
        }

        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << port.RxdPin),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_NEGEDGE,
        };
        gpio_config(&io_conf);
        gpio_set_intr_type(port.RxdPin, GPIO_INTR_NEGEDGE);

        syncDmx = xSemaphoreCreateMutex();
        _context = DmxContext{
            .port = _port.Port,
            .buffer = receiveBuffer,
            .rxQueue = _port.RxQueue,
            .state = DmxState::Idle,
            .semaphore = syncDmx,
            .timeStamp = 0
        };

        xTaskCreate(dmx_receive_task, "uart_event_task", 2048, &_context, 1, NULL);
    }

    void set(const uint8_t dmx[DmxChannelCount]) {
        memcpy(&sendBuffer[1], dmx, DmxChannelCount);
    }

    void set(const uint8_t *dmx, int size) {
        memcpy(&sendBuffer[1], dmx, DmxChannelCount);
    }

    void send() {
        uart_set_line_inverse(_port.Port, UART_SIGNAL_TXD_INV);
        vTaskDelay(1);
        uart_set_line_inverse(_port.Port, UART_SIGNAL_INV_DISABLE);
        usleep(10);
        _port.write(sendBuffer, DmxBufferSize);
    }

    std::vector<uint8_t> receive() {
        if (!_context.received)
            return {};

        xSemaphoreTake(syncDmx, portMAX_DELAY);    
        _context.received = false; // reset
        std::vector<uint8_t> ret(_context.buffer, _context.buffer + DmxBufferSize);
        xSemaphoreGive(syncDmx);
        return ret;
    }

    const int Size = DmxChannelCount;

  private:
    Uart &_port;
    uint8_t sendBuffer[DmxBufferSize];
    uint8_t receiveBuffer[DmxBufferSize];
    xSemaphoreHandle syncDmx;
    DmxContext _context;
};
