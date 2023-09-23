#pragma once

#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <memory>
#include <string_view>
using namespace std::literals::string_view_literals;

constexpr std::string_view OSAL_TAG {"OSAL"sv};

template <typename T> class MessageQueue {
  public:
    MessageQueue(size_t size) {
        xQueue = xQueueCreate(size, sizeof(T *));

        if (xQueue == 0) {
            ESP_LOGE(OSAL_TAG.data(), "Unable to create queue with size %d", size);
        }
        _isActive = true;
    }
    ~MessageQueue() {
        vQueueDelete(xQueue);
    }

    std::unique_ptr<T> enqueue(std::unique_ptr<T> message) {
        if (uxQueueSpacesAvailable(xQueue) == 0) {
            return message;
        }
        T *o = message.release();
        xQueueSend(xQueue, &o, (TickType_t)0);
        return message;
    }

    std::unique_ptr<T> dequeue(uint32_t timeout = portMAX_DELAY) {
        T *pMsg = nullptr;
        if (!xQueueReceive(xQueue, &pMsg, timeout)) {
            return {nullptr};
        }
        return std::unique_ptr<T>(pMsg);
    }

    std::unique_ptr<T> dequeueLatest(uint32_t timeout = portMAX_DELAY) {
        T *pMsg = nullptr;
        if (!xQueueReceive(xQueue, &pMsg, timeout)) {
            return {nullptr};
        }

        while (true == xQueueReceive(xQueue, &pMsg, (TickType_t)0)) {
        }
        return std::unique_ptr<T>(pMsg);
    }

    void signalClose() {
        _isActive = false;
    }
    bool isActive() {
        return _isActive;
    }

  private:
    bool _isActive;
    QueueHandle_t xQueue;
};
