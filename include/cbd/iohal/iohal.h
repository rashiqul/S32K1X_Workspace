/*******************************************************************************
 *
 *  File Name   : iohal.h
 *  Module      : I/O Hardware Abstraction Layer (IoHal)
 *  Description : Top-level IoHal public interface and architecture overview.
 *
 * -----------------------------------------------------------------------------
 * ARCHITECTURE
 * -----------------------------------------------------------------------------
 *
 *  ┌────────────────────────────────────────────────────────────────┐
 *  │                        APPLICATION                             │
 *  │                         main.c                                 │
 *  │            (task scheduling + business logic only)             │
 *  └──────────┬────────────────────┬────────────────────┬───────────┘
 *             │                    │                    │
 *             ▼                    ▼                    ▼
 *  ┌──────────────────┐  ┌──────────────────┐  ┌──────────────────┐
 *  │   iohal_can.h/c  │  │   iohal_os.h/c   │  │    iohal.h/c     │
 *  │  CAN channel     │  │  OS/RTOS services│  │  Init sequencer  │
 *  │  abstraction     │  │  abstraction     │  │  (ECU-M lite)    │
 *  └────────┬─────────┘  └────────┬─────────┘  └────────┬─────────┘
 *           │                     │                      │
 *           └─────────────────────┴──────────────────────┘
 *                                 │
 *                                 ▼
 *  ┌────────────────────────────────────────────────────────────────┐
 *  │                           BSW                                  │
 *  │   Can_43_FLEXCAN  CanIf  Dio  Mcu  Port  Platform  FreeRTOS   │
 *  │                 (NXP RTD 3.0.0 / AUTOSAR 4.7.0)               │
 *  └────────────────────────────────────────────────────────────────┘
 *
 * -----------------------------------------------------------------------------
 * MODULE RESPONSIBILITIES
 * -----------------------------------------------------------------------------
 *
 *  iohal.c      IoHal_Init() — drives the AUTOSAR EcuM-style startup sequence:
 *                 1. Platform_Init  (IRQ routing — must be first)
 *                 2. Mcu_Init / Mcu_InitClock / Mcu_SetMode
 *                 3. Port_Init      (GPIO pin directions)
 *                 4. IoHal_Can_Init (CanIf + FlexCAN + controller start)
 *
 *  iohal_can.c  Hides all FlexCAN / CanIf BSW details from the application.
 *               Owns the App_CanTxConfirmation BSW callback and dispatches
 *               it to an application-registered function pointer so that the
 *               IoHal layer never depends on application symbols.
 *
 *  iohal_os.c   Thin RTOS wrapper. Application tasks call IoHal_Os_* APIs
 *               instead of FreeRTOS directly — enables future RTOS swap
 *               without touching application code.
 *
 * -----------------------------------------------------------------------------
 * NAMING CONVENTIONS  (AUTOSAR SWS_BSW_00306)
 * -----------------------------------------------------------------------------
 *
 *  Functions  : IoHal_<SubModule>_<Action>   e.g. IoHal_Can_Transmit()
 *  Types      : IoHal_<SubModule>_<Name>Type e.g. IoHal_Can_PduType
 *  Macros     : IOHAL_<NAME>                 e.g. IOHAL_CAN_TX_PDU_ID
 *  Return     : Std_ReturnType (E_OK / E_NOT_OK) from Std_Types.h
 *  Null ptr   : NULL_PTR  (not NULL)
 *  Integers   : uint8 / uint16 / uint32  (Platform_Types.h, not stdint)
 *
 *  AUTOSAR    : 4.7.0 (hand-coded — no DaVinci tooling)
 *  Hardware   : NXP S32K144EVB-Q100 (ARM Cortex-M4 @ 48 MHz)
 *  Author     : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

#ifndef IOHAL_H
#define IOHAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 *******************************************************************************/
#include "Std_Types.h"

/* Sub-module headers — included here so application only needs iohal.h */
#include "iohal_can.h"
#include "iohal_os.h"

/*******************************************************************************
 * Public API
 *******************************************************************************/

/**
 * @brief  Initialise all IoHal sub-modules in AUTOSAR EcuM startup order.
 *
 *         Must be the first call in main() before any task or driver use.
 *
 * @return E_OK     All sub-modules initialised successfully.
 *         E_NOT_OK One or more sub-modules failed — check Det/DEM in future.
 */
Std_ReturnType IoHal_Init(void);

/**
 * @brief  Toggle the board LED (PTD0 on S32K144EVB-Q100).
 *
 *         Wraps Dio_FlipChannel so that application and callback code
 *         have no direct dependency on the Dio BSW driver.
 *         The internal state variable is updated atomically with the toggle.
 */
void IoHal_Led_Toggle(void);

/**
 * @brief  Return the current LED state.
 *
 *         State is maintained by the IoHal module — no hardware readback.
 *         Valid after the first call to IoHal_Init().
 *
 * @return 1U if the LED is ON, 0U if the LED is OFF.
 */
uint8 IoHal_Led_GetState(void);

#ifdef __cplusplus
}
#endif

#endif /* IOHAL_H */
