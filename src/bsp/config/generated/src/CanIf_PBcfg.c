/*==================================================================================================
*   Project              : RTD AUTOSAR 4.7
*   Platform             : CORTEXM
*   Peripheral           : generic
*   Dependencies         : 
*
*   Autosar Version      : 4.7.0
*   Autosar Revision     : ASR_REL_4_7_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 3.0.0
*   Build Version        : S32K1_RTD_3_0_0_QLP04_D2509_ASR_REL_4_7_REV_0000_20250930
*
*   Copyright 2020-2025 NXP
*
*   NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be 
*   used strictly in accordance with the applicable license terms.  By expressly 
*   accepting such terms or by downloading, installing, activating and/or otherwise 
*   using the software, you are agreeing that you have read, and that you agree to 
*   comply with and are bound by, such license terms.  If you do not agree to be 
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/
/**
*   @file    CanIf_PBcfg.c
*   @version 3.0.0
*
*   @brief   AUTOSAR CanIf - module interface
*   @details Configuration Structures for PostBuild
*
*   @addtogroup CANIF_DRIVER
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "CanIf.h"
#include "Can_43_FLEXCAN_Cfg.h"
/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CANIF_VENDOR_ID_PBCFG_C                      43
#define CANIF_AR_RELEASE_MAJOR_VERSION_PBCFG_C       4
#define CANIF_AR_RELEASE_MINOR_VERSION_PBCFG_C       7
#define CANIF_AR_RELEASE_REVISION_VERSION_PBCFG_C    0
#define CANIF_SW_MAJOR_VERSION_PBCFG_C               3
#define CANIF_SW_MINOR_VERSION_PBCFG_C               0
#define CANIF_SW_PATCH_VERSION_PBCFG_C               0
/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
/* Application TX confirmation callback — implemented in main.c */
extern void App_CanTxConfirmation(PduIdType CanIfTxPduId, Std_ReturnType result);

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

#define CANIF_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "CanIf_MemMap.h"



/* Here is the configuration related to Can_43_FLEXCAN Driver */
static const CanIf_RxPduType * const CanIf_Can_43_FLEXCAN_HohToRxPduMapping[2U] =
{
    NULL_PTR,
    NULL_PTR
};
static const CanIf_CanDrvConfigType CanIf_Can_43_FLEXCAN_DrvConfig =
{
    /* .HohToRxPduMappingPtr */
    CanIf_Can_43_FLEXCAN_HohToRxPduMapping
};

static const CanIf_CanDrvConfigType * const CanIf_CanDrvConfig[1U] =
{
    &CanIf_Can_43_FLEXCAN_DrvConfig
};

/* TX PDU table — one entry for the CAN0 TX message buffer (CanHardwareObject_1).
 * CanTxPduId=0 maps to this entry; swPduHandle in Can_PduType must equal 0. */
static const CanIf_TxPduType CanIf_TxPduConfig[1U] =
{
    {
        /* .PduId */
        (PduIdType)0U,
        /* .CanId — standard 11-bit frame 0x123 */
        (Can_IdType)0x123U,
        /* .CanFrameType */
        CANIF_STANDARD_CAN,
        /* .CanHth — TX hardware object handle (CanHardwareObject_1) */
        (Can_HwHandleType)Can_43_FLEXCANConf_CanHardwareObject_CanHardwareObject_1,
        /* .CanIfCtrlId */
        (uint8)0U,
        /* .UserTxConfirmation — toggles LED on every successful TX */
        &App_CanTxConfirmation
    }
};

const CanIf_ConfigType CanIf_Config =
{
    /* .NumRxPdu */
    (uint8)0U,
    /* .NumTxPdu */
    (uint8)1U,
    CanIf_CanDrvConfig,
    /* .CanIf_RxPduConfigPtr */
    NULL_PTR,
    /* .CanIf_TxPduConfigPtr */
    CanIf_TxPduConfig
};

#define CANIF_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "CanIf_MemMap.h"

#ifdef __cplusplus
}
#endif

/** @} */

