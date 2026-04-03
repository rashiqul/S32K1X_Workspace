/*******************************************************************************
 *
 *  File Name: src/main.c
 *  Description: S32K144 firmware entry point — FreeRTOS periodic task demo.
 *
 *               Three time-deterministic tasks using vTaskDelayUntil():
 *                 Task_10ms  — 10 ms period,  priority 3 (highest)
 *                 Task_20ms  — 20 ms period,  priority 2
 *                 Task_1000ms— 1000 ms period, priority 1 (lowest user)
 *
 *               vTaskDelayUntil() blocks until an absolute wake time, so
 *               execution jitter inside a task does not accumulate into
 *               period drift. Each task records its last wake time and adds
 *               a fixed increment each iteration.
 *
 *  Hardware  : S32K144EVB-Q100
 *  Compiler  : arm-none-eabi-gcc (GCC 10.2)
 *  AUTOSAR   : 4.7.0  |  RTD 3.0.0
 *  FreeRTOS  : V11.1.0  |  Port: GCC/ARM_CM4F  |  Heap: heap_1
 *  Author    : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

/* RTD BSP drivers */
#include "Dio.h"
#include "Mcu.h"
#include "Port.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* Generated configuration symbols (from bsp/config/generated/include/) */
extern const Mcu_ConfigType Mcu_Config_VS_0;

/*******************************************************************************
 * Task periods
 *******************************************************************************/
#define TASK_PERIOD_10MS    pdMS_TO_TICKS(10U)
#define TASK_PERIOD_20MS    pdMS_TO_TICKS(20U)
#define TASK_PERIOD_1000MS  pdMS_TO_TICKS(1000U)

/*******************************************************************************
 * Task: Task_10ms  — 10 ms periodic, priority 3
 *******************************************************************************/
static void Task_10ms(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, TASK_PERIOD_10MS);
        /* Place 10 ms work here */
    }
}

/*******************************************************************************
 * Task: Task_20ms  — 20 ms periodic, priority 2
 *******************************************************************************/
static void Task_20ms(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, TASK_PERIOD_20MS);
        /* Place 20 ms work here */
    }
}

/*******************************************************************************
 * Task: Task_1000ms — 1000 ms periodic, priority 1
 *
 * Toggles PTD0 (on-board LED) each execution → 0.5 Hz blink (1 s ON / 1 s OFF).
 *******************************************************************************/
static void Task_1000ms(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime    = xTaskGetTickCount();
    Dio_LevelType ledState      = STD_HIGH;

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, TASK_PERIOD_1000MS);
        Dio_WriteChannel(DioConf_DioChannel_DioChannel_0, ledState);
        ledState = (ledState == STD_HIGH) ? STD_LOW : STD_HIGH;
    }
}

/*******************************************************************************
 * main
 *******************************************************************************/
int main(void)
{
    /*--------------------------------------------------------------------------
     * MCU initialisation — 48 MHz FIRC (MCU_NO_PLL=STD_ON)
     * configCPU_CLOCK_HZ in FreeRTOSConfig.h must match this clock.
     *------------------------------------------------------------------------*/
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
