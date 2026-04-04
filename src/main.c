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

/* SBC / transceiver */
#include "Sbc_Uja1169.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* Generated configuration symbols (from bsp/config/generated/include/) */
extern const Mcu_ConfigType Mcu_Config_VS_0;

/*******************************************************************************
 * CAN TX confirmation callback
 * Called from the FlexCAN TX-complete interrupt via CanIf_TxConfirmation.
 * NOTE: This only fires when the physical bus is active (UJA1169 SBC in Normal
 * mode). Until SBC SPI init is implemented this callback will never execute.
 *******************************************************************************/
void App_CanTxConfirmation(PduIdType CanIfTxPduId, Std_ReturnType result)
{
    (void)CanIfTxPduId;
    (void)result;
}

/*******************************************************************************
 * Task periods
 *******************************************************************************/
#define TASK_PERIOD_10MS pdMS_TO_TICKS(10U)
#define TASK_PERIOD_20MS pdMS_TO_TICKS(20U)
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
static void Task_10ms(void* pvParameters)
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
static void Task_20ms(void* pvParameters)
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
 * Toggles the Blue LED (PTD0) every 1000 ms and transmits a CAN frame
 * encoding the current LED state in byte 0 of the payload:
 *   Byte 0: 0x01 = LED ON, 0x00 = LED OFF
 *   Bytes 1-7: static diagnostic pattern (0x02..0x08)
 *
 * LED is driven here (not from App_CanTxConfirmation) so it blinks
 * regardless of whether CAN TX succeeds. This makes it easy to confirm
 * the task is running independently of the SBC/transceiver state.
 *******************************************************************************/
static void Task_1000ms(void* pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    Dio_LevelType ledState   = STD_LOW;

    uint8 payload[8U] = {0x00U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U, 0x07U, 0x08U};
    Can_PduType pdu;
    pdu.id          = 0x123U; /* standard 11-bit CAN ID */
    pdu.swPduHandle = 0U;     /* maps to CanIf_TxPduConfig[0] */
    pdu.length      = 8U;
    pdu.sdu         = payload;

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, TASK_PERIOD_1000MS);
        task_1000ms_count++;

        /* Toggle LED and encode state in byte 0 */
        ledState   = (ledState == STD_HIGH) ? STD_LOW : STD_HIGH;
        payload[0] = (ledState == STD_HIGH) ? 0x01U : 0x00U;
        Dio_WriteChannel(DioConf_DioChannel_DioChannel_0, ledState);

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
     * SBC initialisation — must run before CAN init.
     * Puts UJA1169 transceiver into Normal mode via bit-bang SPI on PTB14-17.
     * Until this call the CAN physical layer is silent (transceiver in standby).
     *------------------------------------------------------------------------*/
    Sbc_Uja1169_Init();

    /*--------------------------------------------------------------------------
     * CAN initialisation
     * - CanIf_Init must run first: it sets CanIf_ConfigPtr, without which
     *   CanIf_TxConfirmation silently drops every callback
     * - Initialise Can_43_FLEXCAN driver with post-build config
     * - Start FLEXCAN_0 controller (enters normal bus-on state)
     *------------------------------------------------------------------------*/
    CanIf_Init(NULL_PTR); /* precompile variant: parameter is ignored */
    Can_43_FLEXCAN_Init(&Can_43_FLEXCAN_Config);
    Can_43_FLEXCAN_SetControllerMode(Can_43_FLEXCANConf_CanController_CanController_0,
                                     CAN_CS_STARTED);

    /*--------------------------------------------------------------------------
     * Create tasks — higher frequency → higher priority so a ready 10 ms task
     * always preempts a running 20 ms or 1000 ms task.
     *------------------------------------------------------------------------*/
    xTaskCreate(Task_10ms, "Task_10ms", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3U,
                NULL);
    xTaskCreate(Task_20ms, "Task_20ms", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2U,
                NULL);
    xTaskCreate(Task_1000ms, "Task_1000ms", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1U,
                NULL);

    /*--------------------------------------------------------------------------
     * Start scheduler — never returns.
     *------------------------------------------------------------------------*/
    vTaskStartScheduler();

    for (;;) {
        /* intentional infinite loop — trap runaway execution */
    }

    return 0;
}
