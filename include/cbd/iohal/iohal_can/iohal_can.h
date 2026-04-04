/*******************************************************************************
 *
 *  File Name   : iohal_can.h
 *  Module      : IoHal — CAN sub-module
 *  Description : CAN channel abstraction over NXP FlexCAN / CanIf BSW.
 *
 * -----------------------------------------------------------------------------
 * DESIGN RATIONALE
 * -----------------------------------------------------------------------------
 *
 *  The application must not call Can_43_FLEXCAN_Write() or CanIf_* APIs
 *  directly.  Doing so creates hard coupling to the NXP RTD driver and makes
 *  unit-testing impossible without the full BSW stack.
 *
 *  This module provides three responsibilities:
 *
 *  1. INIT     IoHal_Can_Init() drives the mandatory CanIf → FlexCAN →
 *              SetControllerMode sequence (order matters — see comments in .c).
 *
 *  2. TRANSMIT IoHal_Can_Transmit() accepts a logical CAN frame descriptor.
 *              Hardware object mapping (CanHardwareObject ID) stays private
 *              inside iohal_can.c — the application works with CAN IDs only.
 *
 *  3. CALLBACK App_CanTxConfirmation() is the BSW TX-complete callback symbol
 *              expected by CanIf.  iohal_can.c owns this symbol and dispatches
 *              to an application-registered function pointer, preventing any
 *              dependency from the IoHal layer back into application code.
 *
 *              Application registers its handler once during init:
 *                IoHal_Can_RegisterTxConfirmCallback(MyApp_OnCanTxDone);
 *
 * -----------------------------------------------------------------------------
 * SIGNAL FLOW
 * -----------------------------------------------------------------------------
 *
 *  [App task]
 *      │  IoHal_Can_Transmit(id, data, len)
 *      ▼
 *  [iohal_can.c]
 *      │  Can_43_FLEXCAN_Write(HW_OBJ, &pdu)
 *      ▼
 *  [FlexCAN HW] ──TX complete IRQ──►
 *                                   [Can_43_FLEXCAN ISR]
 *                                        │ CanIf_TxConfirmation()
 *                                        ▼
 *                                   [CanIf layer]
 *                                        │ App_CanTxConfirmation()
 *                                        ▼
 *                                   [iohal_can.c]
 *                                        │ registered callback (fn ptr)
 *                                        ▼
 *                                   [Application handler]
 *
 *  AUTOSAR    : 4.7.0 (hand-coded — no DaVinci tooling)
 *  Hardware   : NXP S32K144EVB-Q100, FlexCAN_0
 *  Author     : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

#ifndef IOHAL_CAN_H
#define IOHAL_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 *******************************************************************************/
#include "Std_Types.h"

/*******************************************************************************
 * Type definitions
 *******************************************************************************/

/**
 * @brief  Logical CAN frame descriptor passed to IoHal_Can_Transmit().
 *
 *         The application fills this structure; iohal_can.c maps it to the
 *         physical Can_PduType / hardware object required by the BSW driver.
 */
typedef struct
{
    uint32 id;        /**< CAN message identifier (11-bit standard or 29-bit extended) */
    uint8  length;    /**< Data length in bytes [0..8]                                  */
    uint8* sdu;       /**< Pointer to payload data (must remain valid until TX confirm) */
} IoHal_Can_PduType;

/**
 * @brief  TX-confirmation callback signature.
 *
 *         The application registers a function matching this signature via
 *         IoHal_Can_RegisterTxConfirmCallback().  It is called from the
 *         FlexCAN TX-complete interrupt context via CanIf — keep it short
 *         and ISR-safe (no blocking calls, no FreeRTOS API with "FromISR").
 *
 * @param  result  E_OK if the frame was transmitted successfully.
 */
typedef void (*IoHal_Can_TxConfirmCallbackType)(Std_ReturnType result);

/*******************************************************************************
 * Public API
 *******************************************************************************/

/**
 * @brief  Initialise the CAN sub-module.
 *
 *         Sequence (order is mandatory — see iohal_can.c for rationale):
 *           1. CanIf_Init()
 *           2. Can_43_FLEXCAN_Init()
 *           3. Can_43_FLEXCAN_SetControllerMode(CAN_CS_STARTED)
 *
 *         Call from IoHal_Init() only — do not call directly.
 *
 * @return E_OK on success, E_NOT_OK on failure.
 */
Std_ReturnType IoHal_Can_Init(void);

/**
 * @brief  Transmit a CAN frame.
 *
 *         Maps the logical frame to the configured FlexCAN hardware object
 *         and calls Can_43_FLEXCAN_Write().  Non-blocking — the TX-confirm
 *         callback fires from the interrupt when the frame leaves the bus.
 *
 * @param  pdu  Pointer to the frame descriptor. Must not be NULL_PTR.
 * @return E_OK if the frame was accepted by the driver, E_NOT_OK otherwise.
 */
Std_ReturnType IoHal_Can_Transmit(const IoHal_Can_PduType* pdu);

/**
 * @brief  Register the application TX-confirmation callback.
 *
 *         Call once during application initialisation before enabling tasks.
 *         Replaces any previously registered callback.
 *         Passing NULL_PTR disables callback dispatch.
 *
 * @param  callback  Function pointer matching IoHal_Can_TxConfirmCallbackType.
 */
void IoHal_Can_RegisterTxConfirmCallback(IoHal_Can_TxConfirmCallbackType callback);

/*******************************************************************************
 * BSW callback — implemented in iohal_can.c, called by CanIf
 *
 * Do NOT call this function from application code.
 * CanIf invokes it automatically upon FlexCAN TX-complete interrupt.
 *******************************************************************************/

/**
 * @brief  CanIf TX-complete notification (BSW → IoHal boundary).
 *
 *         Implemented here so that the application symbol is owned by the
 *         IoHal layer, not scattered across main.c.
 */
void App_CanTxConfirmation(PduIdType CanIfTxPduId, Std_ReturnType result);

#ifdef __cplusplus
}
#endif

#endif /* IOHAL_CAN_H */
