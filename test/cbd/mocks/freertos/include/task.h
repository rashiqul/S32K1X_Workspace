#ifndef TASK_H
#define TASK_H

#include "FreeRTOS.h"

typedef void (*TaskFunction_t)(void* pvParameters);
typedef void* TaskHandle_t;

BaseType_t xTaskCreate(TaskFunction_t taskCode, const char* taskName,
                       configSTACK_DEPTH_TYPE stackDepth, void* taskParams,
                       UBaseType_t taskPriority, TaskHandle_t* createdTask);

void vTaskStartScheduler(void);
void vTaskDelayUntil(TickType_t* previousWakeTime, TickType_t timeIncrement);
TickType_t xTaskGetTickCount(void);

#endif /* TASK_H */
