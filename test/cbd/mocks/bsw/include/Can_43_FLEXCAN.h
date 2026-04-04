#ifndef CAN_43_FLEXCAN_H
#define CAN_43_FLEXCAN_H

#include "CanIf.h"
#include "Std_Types.h"

typedef uint8 Can_ReturnType;

typedef uint8 Can_ControllerStateType;

typedef struct {
    uint8 placeholder;
} Can_43_FLEXCAN_ConfigType;

typedef struct {
    uint32 id;
    uint8 length;
    uint8* sdu;
    PduIdType swPduHandle;
} Can_PduType;

#define CAN_OK ((Can_ReturnType)0U)
#define CAN_NOT_OK ((Can_ReturnType)1U)

#define CAN_CS_STARTED ((Can_ControllerStateType)1U)

#define Can_43_FLEXCANConf_CanController_CanController_0 ((uint8)0U)
#define Can_43_FLEXCANConf_CanHardwareObject_CanHardwareObject_1 ((uint16)1U)

extern const Can_43_FLEXCAN_ConfigType Can_43_FLEXCAN_Config;

void Can_43_FLEXCAN_Init(const void* configPtr);
Std_ReturnType Can_43_FLEXCAN_SetControllerMode(uint8 controller, Can_ControllerStateType state);
Can_ReturnType Can_43_FLEXCAN_Write(uint16 hwObj, const Can_PduType* pduInfo);

#endif /* CAN_43_FLEXCAN_H */
