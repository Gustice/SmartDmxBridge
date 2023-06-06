#include "uart.hpp"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"

#define BUF_SIZE (1028)

Uart::Uart(uart_port_t port, gpio_num_t rxd, gpio_num_t txd, BaudRate rate, StopBits stopBits)
    : Port(port), RxdPin(rxd), TxdPin(txd), _baudRate(rate) {
    uart_config_t uart_config = {
        .baud_rate = (int)_baudRate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = (uart_stop_bits_t)stopBits,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(Port, BUF_SIZE * 2, 0, 0, nullptr, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(Port, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(Port, TxdPin, RxdPin, _rtsPin, _ctsPin));

    // Configure a temporary buffer for the incoming data
    data = (uint8_t *)malloc(BUF_SIZE);
}

Uart::~Uart() {
    free(data);
}

std::string Uart::read() {
    int len = uart_read_bytes(Port, data, (BUF_SIZE - 1), 20 / portTICK_RATE_MS);
    data[len] = 0;
    std::string ret((const char *)data);
    return ret;
}

std::vector<uint8_t> Uart::readBytes(int byteCount) {
    int len = uart_read_bytes(Port, data, byteCount, 20 / portTICK_RATE_MS);
    std::vector<uint8_t> ret(data, data + len);
    return ret;
}

void Uart::write(char c) {
    uart_write_bytes(Port, (const char *)&c, 1);
}

void Uart::write(uint8_t b) {
    uart_write_bytes(Port, &b, 1);
}

void Uart::write(std::string str) {
    uart_write_bytes(Port, str.c_str(), str.size());
}

void Uart::write(uint8_t *b, int len) {
    uart_write_bytes(Port, b, len);
}
