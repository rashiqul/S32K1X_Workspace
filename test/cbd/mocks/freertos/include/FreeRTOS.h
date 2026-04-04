#ifndef FREERTOS_H
#define FREERTOS_H

#include "Std_Types.h"

typedef uint32 TickType_t;
typedef sint32 BaseType_t;
typedef uint32 UBaseType_t;
typedef uint16 configSTACK_DEPTH_TYPE;

#ifndef pdPASS
#define pdPASS ((BaseType_t)1)
#endif

#ifndef tskIDLE_PRIORITY
#define tskIDLE_PRIORITY ((UBaseType_t)0U)
#endif

#ifndef configMINIMAL_STACK_SIZE
#define configMINIMAL_STACK_SIZE ((configSTACK_DEPTH_TYPE)128U)
#endif

#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS(ms) ((TickType_t)(ms))
#endif

#ifndef NULL_PTR
#define NULL_PTR ((void*)0)
#endif

#endif /* FREERTOS_H */
