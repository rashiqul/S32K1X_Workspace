/*******************************************************************************
 *
 *  File Name   : iohal_can.c
 *  Module      : IoHal — CAN sub-module
 *  Description : CAN channel abstraction over NXP FlexCAN / CanIf BSW.
 *
 *                All FlexCAN hardware object IDs, controller indices, and
 *                CanIf configuration pointers are private to this file.
 *                The application works exclusively through IoHal_Can_*.
 *
 *  AUTOSAR     : 4.7.0 (hand-coded — no DaVinci tooling)
 *  Hardware    : NXP S32K144EVB-Q100, FlexCAN_0
 *  Author      : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

/*******************************************************************************
 * Includes
 *******************************************************************************/
#include "iohal_can.h"

/* BSW CAN driver headers */
#include "CanIf.h"
#include "Can_43_FLEXCAN.h"

/*******************************************************************************
 * Private constants — generated BSW configuration symbols
 *
 * These map logical application identifiers to physical BSW configuration.
 * Update if DaVinci / EcuC configuration changes.
 *******************************************************************************/

/** FlexCAN controller index for the primary CAN bus (CAN0 on S32K144EVB). */
#define IOHAL_CAN_CONTROLLER_ID Can_43_FLEXCANConf_CanController_CanController_0

/** TX hardware message buffer object used for application frame transmission. */
#define IOHAL_CAN_TX_HW_OBJ Can_43_FLEXCANConf_CanHardwareObject_CanHardwareObject_1

/** swPduHandle for the TX PDU — maps to CanIf_TxPduConfig[0]. */
#define IOHAL_CAN_TX_SW_PDU_HANDLE (0U)

/*******************************************************************************
 * Private state
 *******************************************************************************/

/** Application-registered TX confirmation callback. NULL_PTR = not registered. */
static IoHal_Can_TxConfirmCallbackType IoHal_Can_TxConfirmCbk = NULL_PTR;

/*******************************************************************************
 * Public API implementation
 *******************************************************************************/

Std_ReturnType IoHal_Can_Init(void)
{
    /*--------------------------------------------------------------------------
     * Initialisation order is mandatory:
     *
     * 1. CanIf_Init  — sets the internal CanIf_ConfigPtr.  Without this,
     *                  CanIf_TxConfirmation silently drops every callback
     *                  because CanIf has no valid config to look up.
     *
     * 2. Can_43_FLEXCAN_Init — initialises the FlexCAN peripheral registers
     *                          using the post-build configuration struct.
     *
     * 3. SetControllerMode(CAN_CS_STARTED) — brings the controller out of
     *                                         STOPPED state onto the bus.
     *------------------------------------------------------------------------*/

    CanIf_Init(NULL_PTR); /* precompile variant: param is ignored */

    Can_43_FLEXCAN_Init(&Can_43_FLEXCAN_Config);

    if (E_OK != Can_43_FLEXCAN_SetControllerMode(IOHAL_CAN_CONTROLLER_ID, CAN_CS_STARTED)) {
        return E_NOT_OK;
    }

    return E_OK;
}

Std_ReturnType IoHal_Can_Transmit(const IoHal_Can_PduType* pdu)
{
    if (NULL_PTR == pdu) {
        return E_NOT_OK;
    }

    /*--------------------------------------------------------------------------
     * Map the application-level IoHal_Can_PduType to the BSW Can_PduType.
     * The hardware object (IOHAL_CAN_TX_HW_OBJ) and swPduHandle are internal
     * constants — the application only supplies ID, length, and payload.
     *------------------------------------------------------------------------*/
    Can_PduType bswPdu;
    bswPdu.id = pdu->id;
    bswPdu.length = pdu->length;
    bswPdu.sdu = pdu->sdu;
    bswPdu.swPduHandle = IOHAL_CAN_TX_SW_PDU_HANDLE;

    return (E_OK == Can_43_FLEXCAN_Write(IOHAL_CAN_TX_HW_OBJ, &bswPdu)) ? E_OK : E_NOT_OK;
}

void IoHal_Can_RegisterTxConfirmCallback(IoHal_Can_TxConfirmCallbackType callback)
{
    /* NULL_PTR is a valid value — it disables callback dispatch */
    IoHal_Can_TxConfirmCbk = callback;
}

/*******************************************************************************
 * BSW callback — called from CanIf upon FlexCAN TX-complete interrupt
 *******************************************************************************/

void App_CanTxConfirmation(PduIdType CanIfTxPduId, Std_ReturnType result)
{
    (void)CanIfTxPduId; /* not used at this abstraction level */

    /*--------------------------------------------------------------------------
     * Dispatch to the application-registered callback.
     * This function executes in interrupt context — the registered handler
     * must be ISR-safe (no blocking calls, no FreeRTOS API without FromISR).
     *------------------------------------------------------------------------*/
    if (NULL_PTR != IoHal_Can_TxConfirmCbk) {
        IoHal_Can_TxConfirmCbk(result);
    }
}
