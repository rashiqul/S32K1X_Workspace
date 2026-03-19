/**
 * @file Std_Types.h
 * @brief AUTOSAR Standard Types
 * 
 * Local implementation of AUTOSAR platform types for development
 * Based on AUTOSAR 4.x standard definitions
 */

#ifndef STD_TYPES_H
#define STD_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* Standard AUTOSAR type definitions */
typedef uint8_t boolean;

#ifndef TRUE
#define TRUE    ((boolean)1)
#endif

#ifndef FALSE
#define FALSE   ((boolean)0)
#endif

typedef int8_t   sint8;
typedef uint8_t  uint8;
typedef int16_t  sint16;
typedef uint16_t uint16;
typedef int32_t  sint32;
typedef uint32_t uint32;
typedef int64_t  sint64;
typedef uint64_t uint64;

typedef float    float32;
typedef double   float64;

/* Standard return type */
typedef uint8_t Std_ReturnType;

#define E_OK        ((Std_ReturnType)0x00u)
#define E_NOT_OK    ((Std_ReturnType)0x01u)

/* Version information type */
typedef struct {
    uint16 vendorID;
    uint16 moduleID;
    uint8 sw_major_version;
    uint8 sw_minor_version;
    uint8 sw_patch_version;
} Std_VersionInfoType;

#endif /* STD_TYPES_H */
