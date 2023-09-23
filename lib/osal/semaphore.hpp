#pragma once

#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class Semaphore {
  public:
    enum Type { Binary, Counting };

    const Type SemType;
    Semaphore() : SemType(Semaphore::Binary) {
        _handle = xSemaphoreCreateBinary();
    }

    Semaphore(uint maxCount, uint init = 0) : SemType(Semaphore::Counting) {
        _handle = xSemaphoreCreateCounting(maxCount, init);
    }

    ~Semaphore() {
        vSemaphoreDelete(_handle);
    };

    void give() {
        xSemaphoreGive(_handle);
    }

    void take(uint32_t timeout = portMAX_DELAY) {
        xSemaphoreTake(_handle, timeout);
    }

  private:
    xSemaphoreHandle _handle;
};
