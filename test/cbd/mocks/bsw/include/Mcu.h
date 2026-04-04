#ifndef MCU_H
#define MCU_H

#include "Std_Types.h"

#ifndef STD_ON
#define STD_ON 1U
#endif

#ifndef STD_OFF
#define STD_OFF 0U
#endif

#ifndef NULL_PTR
#define NULL_PTR ((void*)0)
#endif

#ifndef MCU_NO_PLL
#define MCU_NO_PLL STD_ON
#endif

typedef struct {
    uint8 placeholder;
} Mcu_ConfigType;

typedef uint8 Mcu_PllStatusType;

#define MCU_PLL_LOCKED ((Mcu_PllStatusType)1U)
#define McuClockSettingConfig_0 ((uint8)0U)
#define McuModeSettingConf_0 ((uint8)0U)

void Mcu_Init(const Mcu_ConfigType* configPtr);
Std_ReturnType Mcu_InitClock(uint8 clockSetting);
Mcu_PllStatusType Mcu_GetPllStatus(void);
void Mcu_DistributePllClock(void);
void Mcu_SetMode(uint8 mode);

#endif /* MCU_H */
