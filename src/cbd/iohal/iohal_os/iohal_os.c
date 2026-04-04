/*******************************************************************************
 *
 *  File Name   : iohal_os.c
 *  Module      : IoHal — OS sub-module
 *  Description : RTOS abstraction over FreeRTOS V11.1.0.
 *
 *                All FreeRTOS API calls are confined to this translation unit.
 *                Application tasks include only iohal_os.h and call IoHal_Os_*
 *                wrappers — enabling future RTOS substitution without touching
 *                any application code.
 *
 *  AUTOSAR     : 4.7.0 (hand-coded — no DaVinci tooling)
 *  Hardware    : NXP S32K144EVB-Q100 (ARM Cortex-M4)
 *  FreeRTOS    : V11.1.0 / GCC ARM_CM4F port
 *  Author      : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

/*******************************************************************************
 * Includes
 *******************************************************************************/
#include "iohal_os.h"

/* FreeRTOS kernel headers — confined to this .c file */
#include "FreeRTOS.h"
#include "task.h"

/*******************************************************************************
 * Public API implementation
 *******************************************************************************/

Std_ReturnType IoHal_Os_CreateTask(const IoHal_Os_TaskConfigType* cfg)
{
    if (NULL_PTR == cfg) {
        return E_NOT_OK;
    }

    /*--------------------------------------------------------------------------
     * Map IoHal_Os_TaskConfigType fields to xTaskCreate() parameters.
     *
     * xTaskCreate signature:
     *   BaseType_t xTaskCreate(
     *       TaskFunction_t  pvTaskCode,
     *       const char*     pcName,
     *       configSTACK_DEPTH_TYPE usStackDepth,
     *       void*           pvParameters,
     *       UBaseType_t     uxPriority,
     *       TaskHandle_t*   pvCreatedTask );
     *------------------------------------------------------------------------*/

    BaseType_t ret = xTaskCreate(cfg->function, cfg->name, (configSTACK_DEPTH_TYPE)cfg->stackSize,
                                 NULL_PTR, (UBaseType_t)cfg->priority, NULL_PTR);

    return (pdPASS == ret) ? E_OK : E_NOT_OK;
}

void IoHal_Os_StartScheduler(void)
{
    vTaskStartScheduler();
}

void IoHal_Os_DelayUntil(IoHal_Os_TickType* pxPreviousWakeTime, IoHal_Os_TickType xTimeIncrement)
{
    vTaskDelayUntil(pxPreviousWakeTime, xTimeIncrement);
}

IoHal_Os_TickType IoHal_Os_GetTickCount(void)
{
    return xTaskGetTickCount();
}
