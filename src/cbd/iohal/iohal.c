/*******************************************************************************
 *
 *  File Name   : iohal.c
 *  Module      : IoHal — init sequencer
 *  Description : Orchestrates BSW startup in AUTOSAR EcuM-style order.
 *
 *                This file is the single place responsible for the hardware
 *                bring-up sequence.  No application code (tasks, callbacks)
 *                should touch BSW init APIs directly.
 *
 *  AUTOSAR     : 4.7.0 (hand-coded — no DaVinci tooling)
 *  Hardware    : NXP S32K144EVB-Q100 (ARM Cortex-M4 @ 48 MHz FIRC)
 *  Author      : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

/*******************************************************************************
 * Includes
 *******************************************************************************/
#include "iohal.h"

/* BSW driver headers */
#include "Dio.h"
#include "Mcu.h"
#include "Platform.h"
#include "Port.h"

/* Generated post-build configuration symbols */
extern const Mcu_ConfigType Mcu_Config_VS_0;

/*******************************************************************************
 * Private function prototypes
 *******************************************************************************/
static Std_ReturnType IoHal_Mcu_Init(void);

/*******************************************************************************
 * Public API implementation
 *******************************************************************************/

void IoHal_Led_Toggle(void)
{
    /* TODO: Dio_FlipChannel(DioConf_DioChannel_DioChannel_0); */
}

/*******************************************************************************
 * Public API implementation
 *******************************************************************************/

Std_ReturnType IoHal_Init(void)
{
    Std_ReturnType status = E_OK;

    /*--------------------------------------------------------------------------
     * Step 1: Platform_Init
     * Must run before any peripheral driver because it routes IRQ vectors
     * (e.g. FlexCAN TX/RX) to the NVIC.  Calling CAN init before this
     * causes silent TX confirmation drops.
     *------------------------------------------------------------------------*/
    /* TODO: call Platform_Init(NULL_PTR); */

    /*--------------------------------------------------------------------------
     * Step 2: MCU init (clock configuration)
     * Configures FIRC @ 48 MHz.  PLL path guarded by MCU_NO_PLL compile flag.
     *------------------------------------------------------------------------*/
    status = IoHal_Mcu_Init();
    if (E_OK != status)
    {
        return E_NOT_OK;
    }

    /*--------------------------------------------------------------------------
     * Step 3: Port init
     * Configures all GPIO pin directions and initial states.
     * PTD0 → output (LED on S32K144EVB-Q100).
     *------------------------------------------------------------------------*/
    /* TODO: call Port_Init(NULL_PTR); */

    /*--------------------------------------------------------------------------
     * Step 4: CAN sub-module init
     *------------------------------------------------------------------------*/
    status = IoHal_Can_Init();
    if (E_OK != status)
    {
        return E_NOT_OK;
    }

    return E_OK;
}

/*******************************************************************************
 * Private implementation
 *******************************************************************************/

/**
 * @brief  MCU and clock initialisation.
 *
 *         Sequence:
 *           Mcu_Init → Mcu_InitClock → (PLL lock wait) → Mcu_SetMode
 *
 * @return E_OK on success.
 */
static Std_ReturnType IoHal_Mcu_Init(void)
{
    /* TODO: Mcu_Init(&Mcu_Config_VS_0); */

    /* TODO: Mcu_InitClock(McuClockSettingConfig_0); */

#if (MCU_NO_PLL == STD_OFF)
    /* TODO: while (MCU_PLL_LOCKED != Mcu_GetPllStatus()) {} */
    /* TODO: Mcu_DistributePllClock(); */
#endif

    /* TODO: Mcu_SetMode(McuModeSettingConf_0); */

    return E_OK;
}
