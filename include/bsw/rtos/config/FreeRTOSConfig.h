/*******************************************************************************
 *
 *  File Name: src/rtos/config/FreeRTOSConfig.h
 *  Description: FreeRTOS kernel configuration for S32K144 (Cortex-M4F).
 *               Derived from NXP FreeRTOS_Example_S32K144 (FreeRTOS V11.1.0).
 *
 *  Hardware  : S32K144EVB-Q100
 *  CPU Clock : 48 MHz FIRC (no PLL — MCU_NO_PLL=STD_ON in BSP config)
 *  Tick Rate : 1 kHz (1 ms)
 *
 *  Author    : Mohammad Rashiqul Alam
 *
 *******************************************************************************
 *
 *  Copyright 2024-2026 NXP.
 *  Copyright (C) 2021-2022 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *  SPDX-License-Identifier: MIT
 *
 *******************************************************************************/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*----------------------------------------------------------------------------
 * CPU & Tick Configuration
 * configCPU_CLOCK_HZ must match the actual core clock set by Mcu_InitClock().
 * BSP uses FIRC at 48 MHz (Clock_Ip_Cfg_Defines.h: CLOCK_IP_FIRC_FREQUENCY).
 *--------------------------------------------------------------------------*/
#define configCPU_CLOCK_HZ (48000000UL)
#define configTICK_RATE_HZ ((TickType_t)1000)

/*----------------------------------------------------------------------------
 * Scheduler Configuration
 *--------------------------------------------------------------------------*/
#define configUSE_PREEMPTION 1
#define configUSE_TIME_SLICING 1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configIDLE_SHOULD_YIELD 1
#define configUSE_16_BIT_TICKS 0 /* 32-bit ticks on 32-bit MCU */

/*----------------------------------------------------------------------------
 * Task Configuration
 *--------------------------------------------------------------------------*/
#define configMAX_PRIORITIES 5
#define configMINIMAL_STACK_SIZE ((unsigned short)90) /* words */
#define configMAX_TASK_NAME_LEN 16

/*----------------------------------------------------------------------------
 * Memory Allocation
 * heap_1.c: static allocation only — tasks/semaphores created at startup, never freed.
 * Upgrade to heap_4.c when vTaskDelete() or pvPortFree() is needed.
 *--------------------------------------------------------------------------*/
#define configSUPPORT_STATIC_ALLOCATION 0
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configTOTAL_HEAP_SIZE ((size_t)(8 * 1024)) /* 8 KB */
#define configAPPLICATION_ALLOCATED_HEAP 0
#define configKERNEL_PROVIDED_STATIC_MEMORY 0

/*----------------------------------------------------------------------------
 * Hook Functions (disabled for initial integration)
 *--------------------------------------------------------------------------*/
#define configUSE_IDLE_HOOK 0
#define configUSE_TICK_HOOK 0
#define configUSE_MALLOC_FAILED_HOOK 0
#define configCHECK_FOR_STACK_OVERFLOW 0
#define configUSE_DAEMON_TASK_STARTUP_HOOK 0

/*----------------------------------------------------------------------------
 * Run-Time Stats & Trace
 *--------------------------------------------------------------------------*/
#define configGENERATE_RUN_TIME_STATS 0
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE() xTaskGetTickCount()
#define configUSE_TRACE_FACILITY 1
#define configUSE_STATS_FORMATTING_FUNCTIONS 1

/*----------------------------------------------------------------------------
 * Synchronization Primitives
 *--------------------------------------------------------------------------*/
#define configUSE_MUTEXES 1
#define configUSE_RECURSIVE_MUTEXES 1
#define configUSE_COUNTING_SEMAPHORES 1
#define configQUEUE_REGISTRY_SIZE 4
#define configUSE_QUEUE_SETS 0

/*----------------------------------------------------------------------------
 * Task Notifications
 *--------------------------------------------------------------------------*/
#define configUSE_TASK_NOTIFICATIONS 1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES 1

/*----------------------------------------------------------------------------
 * Software Timers
 *--------------------------------------------------------------------------*/
#define configUSE_TIMERS 1
#define configTIMER_TASK_PRIORITY 2
#define configTIMER_QUEUE_LENGTH 10
#define configTIMER_TASK_STACK_DEPTH 180

/*----------------------------------------------------------------------------
 * Miscellaneous
 *--------------------------------------------------------------------------*/
#define configUSE_CO_ROUTINES 0
#define configUSE_TICKLESS_IDLE 0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 1
#define configUSE_NEWLIB_REENTRANT 0
#define configENABLE_BACKWARD_COMPATIBILITY 1
#define configUSE_APPLICATION_TASK_TAG 0
#define configRECORD_STACK_HIGH_ADDRESS 0
#define configENABLE_MPU 0
#define configUSE_POSIX_ERRNO 0

/*----------------------------------------------------------------------------
 * Cortex-M Interrupt Priority
 *
 * S32K144 implements 4 priority bits (16 levels, 0=highest, 15=lowest).
 *
 * configKERNEL_INTERRUPT_PRIORITY     — SysTick / PendSV priority (lowest, 0xF0).
 * configMAX_SYSCALL_INTERRUPT_PRIORITY — Highest priority from which FreeRTOS
 *   API calls (FromISR variants) are permitted. Set to priority level 1 (0x10).
 *   ISR priorities ABOVE this value (lower number, e.g. 0) must NEVER call any
 *   FreeRTOS API.
 *--------------------------------------------------------------------------*/
#ifdef __NVIC_PRIO_BITS
#define configPRIO_BITS __NVIC_PRIO_BITS
#else
#define configPRIO_BITS 4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 0x0F
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 1

#define configKERNEL_INTERRUPT_PRIORITY                                                            \
    (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

#define configMAX_SYSCALL_INTERRUPT_PRIORITY                                                       \
    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/*----------------------------------------------------------------------------
 * Assertion
 *--------------------------------------------------------------------------*/
#define configASSERT(x)                                                                            \
    if ((x) == 0) {                                                                                \
        taskDISABLE_INTERRUPTS();                                                                  \
        for (;;)                                                                                   \
            ;                                                                                      \
    }

/*----------------------------------------------------------------------------
 * Map FreeRTOS port handler names to Cortex-M / BSP vector table names.
 * BSP Vector_Table.s declares SVC_Handler, PendSV_Handler, SysTick_Handler
 * as weak symbols — these defines redirect port.c's strong definitions to
 * override them at link time.
 *--------------------------------------------------------------------------*/
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/*----------------------------------------------------------------------------
 * Optional API Includes
 *--------------------------------------------------------------------------*/
#define INCLUDE_vTaskPrioritySet 1
#define INCLUDE_uxTaskPriorityGet 1
#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_vTaskDelayUntil 1
#define INCLUDE_vTaskDelay 1
#define INCLUDE_xTaskGetSchedulerState 1
#define INCLUDE_xTaskGetCurrentTaskHandle 1
#define INCLUDE_uxTaskGetStackHighWaterMark 1
#define INCLUDE_eTaskGetState 1
#define INCLUDE_xEventGroupSetBitFromISR 1
#define INCLUDE_xTimerPendFunctionCall 1
#define INCLUDE_xTaskAbortDelay 1
#define INCLUDE_xTaskGetHandle 1
#define INCLUDE_xTaskResumeFromISR 1
#define INCLUDE_xQueueGetMutexHolder 1

#endif /* FREERTOS_CONFIG_H */
