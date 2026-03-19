/**
 * @file Platform_Types.h
 * @brief AUTOSAR Platform Types
 * 
 * Local implementation of AUTOSAR platform types for development
 * Based on AUTOSAR 4.x standard definitions
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/* CPU type definitions */
#define CPU_TYPE_8      8
#define CPU_TYPE_16     16
#define CPU_TYPE_32     32
#define CPU_TYPE_64     64

/* Bit order definitions */
#define MSB_FIRST       0
#define LSB_FIRST       1

/* Byte order definitions */
#define HIGH_BYTE_FIRST 0
#define LOW_BYTE_FIRST  1

/* CPU configuration for x86_64 */
#define CPU_TYPE        CPU_TYPE_64
#define CPU_BIT_ORDER   LSB_FIRST
#define CPU_BYTE_ORDER  LOW_BYTE_FIRST

/* Boolean type */
#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

/* Standard integer types */
typedef signed char         sint8_least;
typedef unsigned char       uint8_least;
typedef signed short        sint16_least;
typedef unsigned short      uint16_least;
typedef signed int          sint32_least;
typedef unsigned int        uint32_least;

#endif /* PLATFORM_TYPES_H */
