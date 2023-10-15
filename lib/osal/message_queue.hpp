/**
 * @file message_queue.hpp
 * @author Gustice
 * @brief Message queue abstraction
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */

#pragma once

#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <memory>
#include <string_view>
using namespace std::literals::string_view_literals;

/// @brief Tag for logging
constexpr std::string_view OSAL_TAG{"OSAL"sv};

/**
 * @brief Message Queue (generic abstraction of underlying os-implementation)
 * 
 * @tparam T Type for message payload
 */
template <typename T>
class MessageQueue {
  public:
    /// @brief Constructor 
    /// @param size number of slots in queue
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

    /// @brief Push new message in queue
    /// @param message payload
    /// @return nullptr if successful else give back pointer to queued element if pushing failed
    std::unique_ptr<T> enqueue(std::unique_ptr<T> message) {
        if (uxQueueSpacesAvailable(xQueue) == 0) {
            return message;
        }
        T *o = message.release();
        xQueueSend(xQueue, &o, (TickType_t)0);
        return message;
    }

    /// @brief Pop message from queue
    /// @param timeout timeout to wait for message (default: wait for ever)
    /// @return pointer to dequeued element
    std::unique_ptr<T> dequeue(uint32_t timeout = portMAX_DELAY) {
        T *pMsg = nullptr;
        if (!xQueueReceive(xQueue, &pMsg, timeout)) {
            return {nullptr};
        }
        return std::unique_ptr<T>(pMsg);
    }

    /// @brief Pop latest element from queue and ignore all older elements
    /// @param timeout timeout to wait for message (default: wait for ever)
    /// @return pointer to dequeued element
    std::unique_ptr<T> dequeueLatest(uint32_t timeout = portMAX_DELAY) {
        T *pMsg = nullptr;
        if (!xQueueReceive(xQueue, &pMsg, timeout)) {
            return {nullptr};
        }

        while (true == xQueueReceive(xQueue, &pMsg, (TickType_t)0)) {
        }
        return std::unique_ptr<T>(pMsg);
    }

    /// @brief terminate message queue
    void signalClose() {
        _isActive = false;
    }

    /// @brief Check if message queue is active
    /// @return true if active
    bool isActive() {
        return _isActive;
    }

  private:
    bool _isActive;
    QueueHandle_t xQueue;
};
