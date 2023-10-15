#pragma once

#include <stdint.h>
#include <stdio.h>

typedef int QueueHandle_t;
typedef unsigned TickType_t;
typedef int BaseType_t;
typedef unsigned UBaseType_t;
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

QueueHandle_t xQueueCreate(uint32_t, uint32_t);
void vQueueDelete(QueueHandle_t);
BaseType_t xQueueSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait);
BaseType_t xQueueReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait );
UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue );
