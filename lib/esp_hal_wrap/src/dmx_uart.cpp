#include "dmx_uart.hpp"
#include "driver/timer.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include <algorithm>
#include <string.h>
#include <vector>

constexpr int StdBufferSize = 128;

void dmx_receive_task(void *pvParameters) {
    auto context = static_cast<Dmx512::DmxContext *>(pvParameters);
    uint16_t idx = 0;
    uint8_t buff[256]{0};
    while (true) {
        uart_event_t event;
        if (xQueueReceive(context->rxQueue, (void *)&event, (portTickType)1000)) {
            switch (event.type) {
            case UART_DATA:
                if (context->state == Dmx512::DmxState::Break) {
                    if (buff[0] == 0) {
                        context->state = Dmx512::DmxState::Data;
                        idx = 0;
                        context->timeStamp = xTaskGetTickCount();
                    }
                }
                uart_read_bytes(context->port, buff, event.size, portMAX_DELAY);
                if (context->state == Dmx512::DmxState::Data) {
                    xSemaphoreTake(context->syncRead, portMAX_DELAY);
                    for (int i = 0; i < event.size; i++) {
                        if (idx < DmxBufferSize) {
                            context->buffer[idx++] = buff[i];
                        }
                        context->received = idx >= DmxBufferSize;
                    }
                    xSemaphoreGive(context->syncRead);
                }
                break;
            case UART_BREAK:
                uart_flush_input(context->port);
                xQueueReset(context->rxQueue);
                context->state = Dmx512::DmxState::Break;
                break;
            case UART_FRAME_ERR:
            case UART_PARITY_ERR:
            case UART_BUFFER_FULL:
            case UART_FIFO_OVF:
            default:
                uart_flush_input(context->port);
                xQueueReset(context->rxQueue);
                context->state = Dmx512::DmxState::Idle;
                break;
            }
        }
    }
}

Dmx512::Dmx512(uart_port_t port, gpio_num_t rxd, gpio_num_t txd)
    : Uart(port, rxd, txd, Uart::BaudRate::_250000Bd, StdBufferSize, Uart::StopBits::_2sb) {
    ESP_ERROR_CHECK(uart_driver_install(Port, BufferSize * 2, 0, 2, &_rxQueue, 0));

    sendBuffer[0] = 0; // Start-Code
    _context = DmxContext{
        .port = Port,
        .buffer = receiveBuffer,
        .rxQueue = _rxQueue,
        .state = Dmx512::DmxState::Idle,
        .syncRead = xSemaphoreCreateMutex(),
        .timeStamp = 0,
        .received = false,
    };

    xTaskCreate(dmx_receive_task, "uart_event_task", 2048, &_context, 1, NULL);
}

void Dmx512::send() {
    uart_set_line_inverse(Port, UART_SIGNAL_TXD_INV);
    vTaskDelay(1);
    uart_set_line_inverse(Port, UART_SIGNAL_INV_DISABLE);
    usleep(10);
    Uart::write(sendBuffer, DmxBufferSize);
}

void Dmx512::set(const uint8_t dmx[DmxChannelCount]) {
    std::copy(&dmx[0], &dmx[DmxChannelCount], &sendBuffer[1]);
}

void Dmx512::set(const uint8_t *dmx, int size) {
    std::copy(dmx, &dmx[size], &sendBuffer[1]);
}

std::vector<uint8_t> Dmx512::get() {
    if (!_context.received)
        return {};

    xSemaphoreTake(_context.syncRead, portMAX_DELAY);
    _context.received = false; // reset
    std::vector<uint8_t> ret(_context.buffer, _context.buffer + DmxBufferSize);
    xSemaphoreGive(_context.syncRead);
    return ret;
}
