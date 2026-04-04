/*******************************************************************************
 *
 *  File Name   : iohal_os.h
 *  Module      : IoHal — OS sub-module
 *  Description : RTOS abstraction over FreeRTOS V11.1.0.
 *
 * -----------------------------------------------------------------------------
 * DESIGN RATIONALE
 * -----------------------------------------------------------------------------
 *
 *  Application tasks must not call FreeRTOS APIs (xTaskCreate, vTaskDelayUntil,
 *  etc.) directly.  This wrapper provides three benefits:
 *
 *  1. PORTABILITY  Swap FreeRTOS for another RTOS by changing iohal_os.c only.
 *                  Application task code is unaffected.
 *
 *  2. TESTABILITY  On x86 host builds the OS wrapper can be stubbed without
 *                  pulling in FreeRTOS port code.
 *
 *  3. SAFETY TYPING  IoHal_Os_TickType and IoHal_Os_TaskFunctionType give
 *                  application code RTOS-neutral type names.
 *
 * -----------------------------------------------------------------------------
 * USAGE PATTERN (application side)
 * -----------------------------------------------------------------------------
 *
 *  static void MyTask(void* pvParams)
 *  {
 *      IoHal_Os_TickType xLastWake = IoHal_Os_GetTickCount();
 *      for (;;) {
 *          IoHal_Os_DelayUntil(&xLastWake, IOHAL_OS_MS_TO_TICKS(10U));
 *          // ... periodic work ...
 *      }
 *  }
 *
 *  // In main() after IoHal_Init():
 *  static const IoHal_Os_TaskConfigType taskCfg = {
 *      .function  = MyTask,
 *      .name      = "MyTask",
 *      .stackSize = IOHAL_OS_MINIMAL_STACK_SIZE,
 *      .priority  = IOHAL_OS_IDLE_PRIORITY + 1U
 *  };
 *  IoHal_Os_CreateTask(&taskCfg);
 *  IoHal_Os_StartScheduler();
 *
 *  AUTOSAR    : 4.7.0 (hand-coded — no DaVinci tooling)
 *  Hardware   : NXP S32K144EVB-Q100 (ARM Cortex-M4)
 *  FreeRTOS   : V11.1.0 / GCC ARM_CM4F port
 *  Author     : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

#ifndef IOHAL_OS_H
#define IOHAL_OS_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 *******************************************************************************/
#include "Std_Types.h"

/* FreeRTOS tick type — included here to keep IoHal_Os_TickType concrete.
 * Replace with a forward-declared opaque type if full decoupling is needed. */
#include "FreeRTOS.h"
#include "task.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/

/** Convert milliseconds to OS ticks (wraps pdMS_TO_TICKS). */
#define IOHAL_OS_MS_TO_TICKS(ms) pdMS_TO_TICKS(ms)

/** Minimum task stack size in words (wraps configMINIMAL_STACK_SIZE). */
#define IOHAL_OS_MINIMAL_STACK_SIZE configMINIMAL_STACK_SIZE

/** Idle task priority (wraps tskIDLE_PRIORITY). */
#define IOHAL_OS_IDLE_PRIORITY tskIDLE_PRIORITY

/*******************************************************************************
 * Type definitions
 *******************************************************************************/

/** RTOS-neutral tick counter type (maps to FreeRTOS TickType_t). */
typedef TickType_t IoHal_Os_TickType;

/** Task entry-point function signature. */
typedef void (*IoHal_Os_TaskFunctionType)(void* pvParameters);

/**
 * @brief  Task creation descriptor.
 *
 *         Pass a pointer to a statically-allocated instance of this struct
 *         to IoHal_Os_CreateTask().
 */
typedef struct {
    IoHal_Os_TaskFunctionType function; /**< Task entry-point function        */
    const char* name;                   /**< Human-readable name (debug only) */
    uint32 stackSize;                   /**< Stack depth in words              */
    uint32 priority;                    /**< FreeRTOS priority level           */
} IoHal_Os_TaskConfigType;

/*******************************************************************************
 * Public API
 *******************************************************************************/

/**
 * @brief  Create an OS task from a configuration descriptor.
 *
 * @param  cfg  Pointer to task configuration. Must not be NULL_PTR.
 * @return E_OK on success, E_NOT_OK if the OS rejected the task creation.
 */
Std_ReturnType IoHal_Os_CreateTask(const IoHal_Os_TaskConfigType* cfg);

/**
 * @brief  Start the RTOS scheduler.
 *
 *         Does not return under normal operation.
 *         Call after all tasks have been created.
 */
void IoHal_Os_StartScheduler(void);

/**
 * @brief  Block the calling task until the next absolute deadline.
 *
 *         Mirrors vTaskDelayUntil() semantics — drift-free periodic delays.
 *
 * @param  pxPreviousWakeTime  Pointer to tick reference (updated each call).
 * @param  xTimeIncrement      Period in ticks — use IOHAL_OS_MS_TO_TICKS().
 */
void IoHal_Os_DelayUntil(IoHal_Os_TickType* pxPreviousWakeTime, IoHal_Os_TickType xTimeIncrement);

/**
 * @brief  Return the current OS tick count.
 *
 *         Use the return value to initialise pxPreviousWakeTime before the
 *         first IoHal_Os_DelayUntil() call in a task.
 *
 * @return Current tick count.
 */
IoHal_Os_TickType IoHal_Os_GetTickCount(void);

#ifdef __cplusplus
}
#endif

#endif /* IOHAL_OS_H */
