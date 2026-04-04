/*******************************************************************************
 *
 *  File Name   : iohal_app_tasks.c
 *  Module      : IoHal — Application Tasks sub-module
 *  Description : Periodic task implementations, scheduling configuration,
 *                and CAN TX callback for the S32K144 firmware.
 *
 *                Three time-deterministic tasks (drift-free via DelayUntil):
 *                  Task_10ms   — 10 ms period,  priority 3 (highest)
 *                  Task_20ms   — 20 ms period,  priority 2
 *                  Task_1000ms — 1000 ms period, priority 1; transmits CAN
 *
 *                LED on PTD0 toggles on every CAN TX-complete interrupt via
 *                App_OnCanTxDone, registered through IoHal_Can.
 *
 *  Hardware    : S32K144EVB-Q100
 *  Author      : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

#include "iohal_app_tasks.h"
#include "iohal.h"

/*******************************************************************************
 * Task periods
 *******************************************************************************/
#define TASK_PERIOD_10MS    IOHAL_OS_MS_TO_TICKS(10U)
#define TASK_PERIOD_20MS    IOHAL_OS_MS_TO_TICKS(20U)
#define TASK_PERIOD_1000MS  IOHAL_OS_MS_TO_TICKS(1000U)

/*******************************************************************************
 * Execution counters — inspect in debugger to verify scheduling.
 * After 1 s: task_10ms_count ≈ 100, task_20ms_count ≈ 50, task_1000ms_count = 1
 *******************************************************************************/
volatile uint32 task_10ms_count;
volatile uint32 task_20ms_count;
volatile uint32 task_1000ms_count;

/*******************************************************************************
 * Private function prototypes
 *******************************************************************************/
static void App_OnCanTxDone(Std_ReturnType result);
static void Task_10ms(void* pvParameters);
static void Task_20ms(void* pvParameters);
static void Task_1000ms(void* pvParameters);

/*******************************************************************************
 * Task configuration descriptors
 *******************************************************************************/
static const IoHal_Os_TaskConfigType task_10ms_cfg = {
    .function  = Task_10ms,
    .name      = "Task_10ms",
    .stackSize = IOHAL_OS_MINIMAL_STACK_SIZE,
    .priority  = IOHAL_OS_IDLE_PRIORITY + 3U
};

static const IoHal_Os_TaskConfigType task_20ms_cfg = {
    .function  = Task_20ms,
    .name      = "Task_20ms",
    .stackSize = IOHAL_OS_MINIMAL_STACK_SIZE,
    .priority  = IOHAL_OS_IDLE_PRIORITY + 2U
};

static const IoHal_Os_TaskConfigType task_1000ms_cfg = {
    .function  = Task_1000ms,
    .name      = "Task_1000ms",
    .stackSize = IOHAL_OS_MINIMAL_STACK_SIZE,
    .priority  = IOHAL_OS_IDLE_PRIORITY + 1U
};

/*******************************************************************************
 * Public API implementation
 *******************************************************************************/

Std_ReturnType IoHal_AppTasks_Init(void)
{
    Std_ReturnType status = E_OK;

    IoHal_Can_RegisterTxConfirmCallback(App_OnCanTxDone);

    if (E_OK == status) { status = IoHal_Os_CreateTask(&task_10ms_cfg);   }
    if (E_OK == status) { status = IoHal_Os_CreateTask(&task_20ms_cfg);   }
    if (E_OK == status) { status = IoHal_Os_CreateTask(&task_1000ms_cfg); }

    return status;
}

/*******************************************************************************
 * Private implementation
 *******************************************************************************/

/**
 * @brief  CAN TX confirmation callback (ISR context).
 *         Toggles the board LED on every successfully transmitted frame.
 */
static void App_OnCanTxDone(Std_ReturnType result)
{
    (void)result;
    IoHal_Led_Toggle();
}

/**
 * @brief  Task_10ms — 10 ms periodic, priority 3 (highest user task).
 */
static void Task_10ms(void* pvParameters)
{
    (void)pvParameters;
    IoHal_Os_TickType xLastWakeTime = IoHal_Os_GetTickCount();

    for (;;) {
        IoHal_Os_DelayUntil(&xLastWakeTime, TASK_PERIOD_10MS);
        task_10ms_count++;
        /* Place 10 ms work here */
    }
}

/**
 * @brief  Task_20ms — 20 ms periodic, priority 2.
 */
static void Task_20ms(void* pvParameters)
{
    (void)pvParameters;
    IoHal_Os_TickType xLastWakeTime = IoHal_Os_GetTickCount();

    for (;;) {
        IoHal_Os_DelayUntil(&xLastWakeTime, TASK_PERIOD_20MS);
        task_20ms_count++;
        /* Place 20 ms work here */
    }
}

/**
 * @brief  Task_1000ms — 1000 ms periodic, priority 1 (lowest user task).
 *         Transmits a CAN frame every second via IoHal_Can_Transmit().
 */
static void Task_1000ms(void* pvParameters)
{
    (void)pvParameters;
    IoHal_Os_TickType xLastWakeTime = IoHal_Os_GetTickCount();

    static const uint8 payload[8U] = {0x01U, 0x02U, 0x03U, 0x04U,
                                       0x05U, 0x06U, 0x07U, 0x08U};
    IoHal_Can_PduType pdu;
    pdu.id     = 0x123U;
    pdu.length = 8U;
    pdu.sdu    = (uint8*)payload;

    for (;;) {
        IoHal_Os_DelayUntil(&xLastWakeTime, TASK_PERIOD_1000MS);
        task_1000ms_count++;
        IoHal_Can_Transmit(&pdu);
    }
}
