#ifndef CANIF_H
#define CANIF_H

#include "Std_Types.h"

typedef uint16 PduIdType;

#ifndef NULL_PTR
#define NULL_PTR ((void*)0)
#endif

void CanIf_Init(const void* configPtr);

#endif /* CANIF_H */
