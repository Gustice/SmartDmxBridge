/**
 * @file semaphore.hpp
 * @author Gustice
 * @brief Semaphore abstraction
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */

#pragma once

#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

/**
 * @brief Semaphore (generic abstraction of underlying os-implementation)
 */
class Semaphore {
  public:
    /**
     * @brief Type of semaphore
     */
    enum Type {
        Binary,  ///< Binary semaphore, can only be true or false, no matter how often given
        Counting ///< Counting semaphore, keeps track of number of give and takes
    };

    /// @brief Type of semaphore
    const Type SemType;

    /// @brief Constructor for default type (binary semaphore)
    Semaphore()
        : SemType(Semaphore::Binary) {
        _handle = xSemaphoreCreateBinary();
    }

    /// @brief Constructor for counting semaphore
    /// @param maxCount Count limit
    /// @param init initial value for counter
    Semaphore(uint maxCount, uint init = 0)
        : SemType(Semaphore::Counting) {
        _handle = xSemaphoreCreateCounting(maxCount, init);
    }

    /// @brief Destructor
    ~Semaphore() {
        vSemaphoreDelete(_handle);
    };

    /// @brief Give signal
    void give() {
        xSemaphoreGive(_handle);
    }

    /// @brief Consume signal
    /// @param timeout timeout to wait for signal in ms (default: wait for ever)
    void take(uint32_t timeout = portMAX_DELAY) {
        xSemaphoreTake(_handle, timeout);
    }

  private:
    xSemaphoreHandle _handle;
};
