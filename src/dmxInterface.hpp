#pragma once

#include "driver/timer.h"
#include "esp_log.h"
#include "uart.hpp"
#include <string.h>

#define DMX_DEV_WIDTH 48

#define ESP_INTR_FLAG_DEFAULT 0

struct DmxContext {
    timer_group_t group;
    timer_idx_t timer;
    uart_port_t uartPort;
    gpio_num_t uartPin;
};
static DmxContext dmxContext;
static uint64_t last_timer_counter_value;

static bool inProgress = false;
static void IRAM_ATTR gpio_isr_handler(void *arg) {
    DmxContext *context = (DmxContext *)arg;
    if (inProgress) {
        inProgress = false;
        timer_pause(dmxContext.group, dmxContext.timer);
        timer_set_counter_value(dmxContext.group, dmxContext.timer, 0);
        gpio_set_intr_type(context->uartPin, GPIO_INTR_NEGEDGE);
    } else {
        inProgress = true;
        timer_start(dmxContext.group, dmxContext.timer);
        gpio_set_intr_type(context->uartPin, GPIO_INTR_ANYEDGE);
    }
}

static xSemaphoreHandle s_semph_dmx = xSemaphoreCreateBinary();

static bool IRAM_ATTR timer_isr_callback(void *args) {
    DmxContext *context = (DmxContext *)args;
    timer_pause(dmxContext.group, dmxContext.timer);
    last_timer_counter_value =
        timer_group_get_counter_value_in_isr(dmxContext.group, dmxContext.timer);
    timer_set_counter_value(dmxContext.group, dmxContext.timer, 0);

    BaseType_t high_task_awoken = pdFALSE;
    if (inProgress) {
        uart_flush(context->uartPort);
        xSemaphoreGiveFromISR(s_semph_dmx, &high_task_awoken);
        inProgress = false;
    }
    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}

#define TIMER_DIVIDER (80)                           //  Hardware timer clock divider
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER) // convert counter value to seconds

static void tg_timer_init(DmxContext &context, int timer_interval_us) {
    timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .divider = TIMER_DIVIDER,
    };
    timer_init(context.group, context.timer, &config);
    timer_set_counter_value(context.group, context.timer, 0);

    timer_set_alarm_value(context.group, context.timer, timer_interval_us);
    timer_enable_intr(context.group, context.timer);

    timer_isr_callback_add(context.group, context.timer, timer_isr_callback, &context, 0);
}

class DmxInterface {
  public:
    DmxInterface(Uart &port) : _port(port) {
        dmxMsg[0] = 0; // Start-Code

        for (size_t i = 0; i < DMX_DEV_WIDTH + 1; i++) {
            dmxMsg[i] = 0;
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

        dmxContext = DmxContext{TIMER_GROUP_0, TIMER_0, _port.Port, _port.RxdPin};
        gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
        gpio_isr_handler_add(port.RxdPin, gpio_isr_handler, &dmxContext);

        tg_timer_init(dmxContext, 50);
        ESP_LOGD("DMX", "Time scale is %d", TIMER_SCALE);
    }

    void set(uint8_t dmx[DMX_DEV_WIDTH]) {
        memcpy(&dmxMsg[1], dmx, DMX_DEV_WIDTH);
    }

    void send() {
        gpio_set_direction(_port.TxdPin, gpio_mode_t::GPIO_MODE_OUTPUT);
        gpio_set_level(_port.TxdPin, 0);
        usleep(100);
        gpio_set_direction(_port.TxdPin, gpio_mode_t::GPIO_MODE_INPUT);
        uart_set_pin(_port.Port, _port.TxdPin, _port.RxdPin, UART_PIN_NO_CHANGE,
                     UART_PIN_NO_CHANGE);

        _port.write(dmxMsg, DMX_DEV_WIDTH + 1);
    }

    std::vector<uint8_t> receive() {

        xSemaphoreTake(s_semph_dmx, portMAX_DELAY);
        ESP_LOGI("DMX", "Timer interrupt handled after %llu", last_timer_counter_value);
        auto ret = _port.readBytes(49);
        gpio_set_intr_type(dmxContext.uartPin, GPIO_INTR_NEGEDGE);

        return ret;
    }

    const int Size = DMX_DEV_WIDTH;

  private:
    Uart &_port;
    uint8_t dmxMsg[1 + DMX_DEV_WIDTH];
};
