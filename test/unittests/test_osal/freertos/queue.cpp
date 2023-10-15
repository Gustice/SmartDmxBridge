#include "queue.h"
#include <string>

std::string lastCall;
int64_t pItemToTransfer = 0; // Unit Tests on PC run on 64-Architecture

QueueHandle_t xQueueCreate(uint32_t queueSize, uint32_t elementSize) {
    lastCall = "Constructor: queueSize=" + std::to_string(queueSize) +
               " elementSize=" + std::to_string(elementSize);
    return 0;
}

void vQueueDelete(QueueHandle_t) {}

BaseType_t xQueueSend(QueueHandle_t xQueue, const void *const pItem, TickType_t wait) {
    pItemToTransfer = *((const int64_t *const )pItem);
    lastCall = "Send: pItem=0x" + std::to_string((int64_t)pItem) + " wait=" + std::to_string(wait);
    return 0;
}

BaseType_t xQueueReceive(QueueHandle_t xQueue, void *const pvBuffer, TickType_t wait) {
    *((int64_t *)pvBuffer) = pItemToTransfer;
    lastCall = "Get: pItem=0x" + std::to_string(pItemToTransfer) + " wait=" + std::to_string(wait);
    return 1;
}

UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue ) {
    return 1;
}