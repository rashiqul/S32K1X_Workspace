/*******************************************************************************
 *
 *  File Name: src/main.c
 *  Description: S32K144 firmware entry point.
 *               Initialises MCU clocks, configures Port PTD0 as GPIO output,
 *               then blinks the on-board LED (PTD0) indefinitely.
 *
 *  Hardware  : S32K144EVB-Q100
 *  Compiler  : arm-none-eabi-gcc (GCC 10.2)
 *  AUTOSAR   : 4.7.0  |  RTD 3.0.0
 *  Author    : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

#include "Dio.h"
#include "Mcu.h"
#include "Port.h"

/* Generated configuration symbols (from bsp/config/generated/include/) */
extern const Mcu_ConfigType Mcu_Config_VS_0;

/*******************************************************************************
 * Local helpers
 *******************************************************************************/

/** Busy-wait delay (approximate, depends on clock speed and optimisation). */
static void Delay(volatile uint32 cycles)
{
    while (cycles-- > 0U) {
        __asm__ __volatile__("nop");
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
     * - Configure clock tree (McuClockSettingConfig_0)
     * - If PLL is enabled in config: wait for lock and distribute PLL clock
     * - Set run mode
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
    //Port_Init(NULL_PTR);

    /*--------------------------------------------------------------------------
     * Main loop: blink LED on PTD0
     *------------------------------------------------------------------------*/
    for (;;) {
        //Dio_WriteChannel(DioConf_DioChannel_DioChannel_0, STD_HIGH); /* LED ON  */
        Delay(500000U);
        //Dio_WriteChannel(DioConf_DioChannel_DioChannel_0, STD_LOW); /* LED OFF */
        //Delay(500000U);
    }

    return 0; /* Never reached */
}
