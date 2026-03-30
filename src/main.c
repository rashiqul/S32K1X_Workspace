/*******************************************************************************
 *
 *  File Name: src/main.c
 *  Description: S32K144 firmware entry point — FreeRTOS + CAN demo.
 *
 *               Three time-deterministic tasks using vTaskDelayUntil():
 *                 Task_10ms   — 10 ms period,  priority 3 (highest)
 *                 Task_20ms   — 20 ms period,  priority 2
 *                 Task_1000ms — 1000 ms period, priority 1; transmits CAN frame
 *
 *               LED on PTD0 is toggled exclusively from App_CanTxConfirmation,
 *               which fires from the FlexCAN TX-complete interrupt via CanIf.
 *
 *  Hardware  : S32K144EVB-Q100
 *  Compiler  : arm-none-eabi-gcc (GCC 10.2)
 *  AUTOSAR   : 4.7.0  |  RTD 3.0.0
 *  FreeRTOS  : V11.1.0  |  Port: GCC/ARM_CM4F  |  Heap: heap_1
 *  Author    : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

/* RTD BSP drivers */
#include "CanIf.h"
#include "Can_43_FLEXCAN.h"
#include "Dio.h"
#include "Mcu.h"
#include "Platform.h"
#include "Port.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* Generated configuration symbols (from bsp/config/generated/include/) */
extern const Mcu_ConfigType Mcu_Config_VS_0;

/*******************************************************************************
 * CAN TX confirmation callback
 * Called from the FlexCAN TX-complete interrupt via CanIf_TxConfirmation.
 * Toggling the LED here gives a visible pulse on every successfully sent frame.
 *******************************************************************************/
void App_CanTxConfirmation(PduIdType CanIfTxPduId, Std_ReturnType result)
{
    (void)CanIfTxPduId;
    (void)result;
    Dio_FlipChannel(DioConf_DioChannel_DioChannel_0);
}

/*******************************************************************************
 * Task periods
 *******************************************************************************/
#define TASK_PERIOD_10MS   pdMS_TO_TICKS(10U)
#define TASK_PERIOD_20MS   pdMS_TO_TICKS(20U)
#define TASK_PERIOD_1000MS pdMS_TO_TICKS(1000U)

/*******************************************************************************
 * Execution counters — inspect in debugger to verify scheduling.
 * After 1 s: task_10ms_count ≈ 100, task_20ms_count ≈ 50, task_1000ms_count = 1
 *******************************************************************************/
volatile uint32_t task_10ms_count;
volatile uint32_t task_20ms_count;
volatile uint32_t task_1000ms_count;

/*******************************************************************************
 * Task: Task_10ms — 10 ms periodic, priority 3 (highest)
 *******************************************************************************/
static void Task_10ms(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, TASK_PERIOD_10MS);
        task_10ms_count++;
        /* Place 10 ms work here */
    }
}

/*******************************************************************************
 * Task: Task_20ms — 20 ms periodic, priority 2
 *******************************************************************************/
static void Task_20ms(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, TASK_PERIOD_20MS);
        task_20ms_count++;
        /* Place 20 ms work here */
    }
}

/*******************************************************************************
 * Task: Task_1000ms — 1000 ms periodic, priority 1 (lowest user)
 *
 * Transmits a CAN frame every 1000 ms. The LED is toggled by
 * App_CanTxConfirmation (TX-complete interrupt callback) — not here.
 *******************************************************************************/
static void Task_1000ms(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    static const uint8 payload[8U] = {0x01U, 0x02U, 0x03U, 0x04U,
                                       0x05U, 0x06U, 0x07U, 0x08U};
    Can_PduType pdu;
    pdu.id          = 0x123U; /* standard 11-bit CAN ID */
    pdu.swPduHandle = 0U;     /* maps to CanIf_TxPduConfig[0] */
    pdu.length      = 8U;
    pdu.sdu         = (uint8 *)payload;

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, TASK_PERIOD_1000MS);
        task_1000ms_count++;
        Can_43_FLEXCAN_Write(Can_43_FLEXCANConf_CanHardwareObject_CanHardwareObject_1, &pdu);
    }
}

/*******************************************************************************
 * main
 *******************************************************************************/
int main(void)
{
    /*--------------------------------------------------------------------------
     * MCU initialisation — 48 MHz FIRC (MCU_NO_PLL=STD_ON)
     * Platform_Init must run first to route peripheral IRQs (FlexCAN).
     *------------------------------------------------------------------------*/
    Platform_Init(NULL_PTR);

    Mcu_Init(&Mcu_Config_VS_0);
    Mcu_InitClock(McuClockSettingConfig_0);
#if (MCU_NO_PLL == STD_OFF)
    while (MCU_PLL_LOCKED != Mcu_GetPllStatus()) {
        /* Spin until PLL acquires lock */
    }
    Mcu_DistributePllClock();
#endif
    Mcu_SetMode(McuModeSettingConf_0);

    /*--------------------------------------------------------------------------
     * Port initialisation — PTD0 as GPIO output (LED on S32K144EVB)
     *------------------------------------------------------------------------*/
    Port_Init(NULL_PTR);

    /*--------------------------------------------------------------------------
     * CAN initialisation
     * - CanIf_Init must run first: it sets CanIf_ConfigPtr, without which
     *   CanIf_TxConfirmation silently drops every callback
     * - Initialise Can_43_FLEXCAN driver with post-build config
     * - Start FLEXCAN_0 controller (enters normal bus-on state)
     *------------------------------------------------------------------------*/
    CanIf_Init(NULL_PTR);   /* precompile variant: parameter is ignored */
    Can_43_FLEXCAN_Init(&Can_43_FLEXCAN_Config);
    Can_43_FLEXCAN_SetControllerMode(Can_43_FLEXCANConf_CanController_CanController_0,
                                     CAN_CS_STARTED);

    /*--------------------------------------------------------------------------
     * Create tasks — higher frequency → higher priority so a ready 10 ms task
     * always preempts a running 20 ms or 1000 ms task.
     *------------------------------------------------------------------------*/
    xTaskCreate(Task_10ms,   "Task_10ms",   configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3U, NULL);
    xTaskCreate(Task_20ms,   "Task_20ms",   configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2U, NULL);
    xTaskCreate(Task_1000ms, "Task_1000ms", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1U, NULL);

    /*--------------------------------------------------------------------------
     * Start scheduler — never returns.
     *------------------------------------------------------------------------*/
    vTaskStartScheduler();

    for (;;) {
        /* intentional infinite loop — trap runaway execution */
    }

    return 0;
}
