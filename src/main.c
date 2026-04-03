/*******************************************************************************
 *
 *  File Name: src/main.c
 *  Description: S32K144 firmware entry point — FreeRTOS LED blink demo.
 *
 *               Initialises MCU clocks and Port via RTD BSP drivers, then
 *               starts the FreeRTOS scheduler with a single LED task that
 *               blinks PTD0 (on-board LED) every 500 ms using vTaskDelay().
 *
 *               The demo mirrors the producer/consumer structure from the
 *               NXP FreeRTOS_Example_S32K144 S32DS project, simplified to a
 *               single LED task so CAN and other feature branches can add
 *               tasks alongside it without touching this file.
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
 * Task: LedBlinkTask
 *
 * Toggles PTD0 (on-board LED) every 500 ms.
 * Stack depth: configMINIMAL_STACK_SIZE (90 words / 360 bytes).
 * Priority   : tskIDLE_PRIORITY + 1 (lowest user priority).
 *
 * To verify FreeRTOS is running: connect a logic analyser or oscilloscope to
 * PTD0 — you should see a precise 1 Hz square wave (500 ms ON / 500 ms OFF).
 *******************************************************************************/
static void LedBlinkTask(void* pvParameters)
{
    (void)pvParameters;

    for (;;) {
        Dio_WriteChannel(DioConf_DioChannel_DioChannel_0, STD_HIGH); /* LED ON  */
        vTaskDelay(pdMS_TO_TICKS(500U));
        Dio_WriteChannel(DioConf_DioChannel_DioChannel_0, STD_LOW); /* LED OFF */
        vTaskDelay(pdMS_TO_TICKS(500U));
    }
}

/*******************************************************************************
 * main
 *******************************************************************************/
int main(void)
{
    /*--------------------------------------------------------------------------
     * MCU initialisation
     * - Initialise MCU driver with power/clock defaults
     * - Configure clock tree (McuClockSettingConfig_0) → 48 MHz FIRC
     *   (MCU_NO_PLL=STD_ON in this BSP config, so no PLL wait required)
     * - Set run mode
     *
     * Note: configCPU_CLOCK_HZ in FreeRTOSConfig.h must match the clock
     *       set here. Both are 48 MHz (CLOCK_IP_FIRC_FREQUENCY).
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
     * Port initialisation
     * - Configures PTD0 as GPIO output (connected to LED on S32K144EVB)
     *------------------------------------------------------------------------*/
    Port_Init(NULL_PTR);

    /*--------------------------------------------------------------------------
     * Create FreeRTOS tasks
     *------------------------------------------------------------------------*/
    xTaskCreate(LedBlinkTask, "LedBlink", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1U,
                NULL);

    /*--------------------------------------------------------------------------
     * Start scheduler — never returns.
     * FreeRTOS port (port.c) takes over SysTick at this point and drives the
     * 1 ms tick. SVC_Handler / PendSV_Handler / SysTick_Handler BSP weak
     * symbols are overridden by the FreeRTOS port at link time.
     *------------------------------------------------------------------------*/
    vTaskStartScheduler();

    /* Should never reach here — scheduler only returns on heap exhaustion. */
    for (;;) {
        /* intentional infinite loop — trap runaway execution */
    }

    return 0;
}
