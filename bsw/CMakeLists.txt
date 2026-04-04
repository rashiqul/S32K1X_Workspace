#******************************************************************************
#
#     File Name: bsw/CMakeLists.txt
#     Description: CMake build for the S32K144 Basic Software (BSW) —
#                  Board Support Package and FreeRTOS RTOS kernel.
#                  Compiles startup code, RTD AUTOSAR 4.7 drivers, and
#                  generated peripheral configuration.
#
#     Target MCU : NXP S32K144 (ARM Cortex-M4 @ up to 112 MHz)
#     AUTOSAR    : 4.7.0
#     RTD Version: 3.0.0
#     Author     : Mohammad Rashiqul Alam
#
#******************************************************************************

# Enable ASM here (after the toolchain file has been loaded by project())
# so that ARM assembly files (.s) are compiled with arm-none-eabi-gcc.
enable_language(ASM)

# ---------------------------------------------------------------------------
# Platform SDK - provides device headers and CMSIS types
# Priority: CMake var > environment var > bundled copy > S32DS install
# ---------------------------------------------------------------------------
if(NOT DEFINED S32DS_SDK_ROOT)
    if(DEFINED ENV{S32DS_SDK_ROOT})
        set(S32DS_SDK_ROOT "$ENV{S32DS_SDK_ROOT}")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/platform_sdk/RTD/BaseNXP_TS_T40D2M30I0R0/header/S32K144.h")
        # Use headers bundled in the repo (CI-friendly, no S32DS install required)
        set(S32DS_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/platform_sdk")
    else()
        # Fall back to default S32DS installation path (local dev with S32DS installed)
        set(S32DS_SDK_ROOT "/mnt/c/NXP/S32DS.3.6.6/S32DS/software/PlatformSDK_S32K1_S32M24")
    endif()
endif()

message(STATUS "[BSP] S32DS_SDK_ROOT = ${S32DS_SDK_ROOT}")

set(SDK_BASENXP_HEADER   "${S32DS_SDK_ROOT}/RTD/BaseNXP_TS_T40D2M30I0R0/header")
set(SDK_BASENXP_INCLUDE  "${S32DS_SDK_ROOT}/RTD/BaseNXP_TS_T40D2M30I0R0/include")
set(SDK_PLATFORM_INCLUDE "${S32DS_SDK_ROOT}/RTD/Platform_TS_T40D2M30I0R0/include")
set(SDK_PLATFORM_STARTUP "${S32DS_SDK_ROOT}/RTD/Platform_TS_T40D2M30I0R0/startup/include")

# Validate that SDK path exists
if(NOT EXISTS "${SDK_BASENXP_HEADER}/S32K144.h")
    message(WARNING "[BSP] S32K144.h not found at ${SDK_BASENXP_HEADER}. "
                    "Set S32DS_SDK_ROOT to the PlatformSDK_S32K1_S32M24 directory.")
endif()

# ---------------------------------------------------------------------------
# Source files
# ---------------------------------------------------------------------------
file(GLOB STARTUP_C_SOURCES   "${CMAKE_CURRENT_SOURCE_DIR}/startup/*.c")
file(GLOB STARTUP_ASM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/startup/*.s"
                               "${CMAKE_CURRENT_SOURCE_DIR}/startup/*.S")
file(GLOB RTD_SOURCES          "${CMAKE_CURRENT_SOURCE_DIR}/drivers/rtd/src/*.c")
file(GLOB GENERATED_SOURCES    "${CMAKE_CURRENT_SOURCE_DIR}/config/generated/src/*.c")
file(GLOB BOARD_SOURCES        "${CMAKE_CURRENT_SOURCE_DIR}/config/board/*.c")

# ---------------------------------------------------------------------------
# BSP library
# ---------------------------------------------------------------------------
add_library(s32k1_bsp STATIC
    ${STARTUP_C_SOURCES}
    ${STARTUP_ASM_SOURCES}
    ${RTD_SOURCES}
    ${GENERATED_SOURCES}
    ${BOARD_SOURCES}
)

# Required preprocessor symbols (from S32DS .cproject)
target_compile_definitions(s32k1_bsp PUBLIC
    CPU_S32K144HFT0VLLT       # Device variant
    VV_RESULT_ADDRESS=0x1FFF8000  # Test result SRAM address
    I_CACHE_ENABLE            # Enable instruction cache
    ENABLE_FPU                # Enable FPU
    GCC                       # Compiler identification
)

# C compile options for RTD sources
target_compile_options(s32k1_bsp PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -std=c99
        -fomit-frame-pointer
        -fno-short-enums
        -funsigned-bitfields
        -Wall
        -Wextra
        -Wstrict-prototypes
        -Wsign-compare
        -Wundef
        -Wdouble-promotion
        -Wno-unused-parameter
        -Wno-unknown-pragmas
    >
)

# Include paths
target_include_directories(s32k1_bsp PUBLIC
    # BSP local headers
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/rtd/include
    ${CMAKE_CURRENT_SOURCE_DIR}/config/generated/include
    ${CMAKE_CURRENT_SOURCE_DIR}/config/board
    ${CMAKE_CURRENT_SOURCE_DIR}/startup
    # Platform SDK headers (NXP S32DS installation)
    ${SDK_BASENXP_HEADER}
    ${SDK_BASENXP_INCLUDE}
    ${SDK_PLATFORM_INCLUDE}
    ${SDK_PLATFORM_STARTUP}
)

# ---------------------------------------------------------------------------
# Linker configuration (exported as INTERFACE so firmware target inherits it)
# ---------------------------------------------------------------------------
add_library(s32k1_linker_config INTERFACE)

target_link_options(s32k1_linker_config INTERFACE
    -T${CMAKE_CURRENT_SOURCE_DIR}/linker/linker_flash_s32k144.ld
    -specs=nano.specs
    -specs=nosys.specs
    -nostartfiles
    --entry=Reset_Handler
    -Wl,--gc-sections
    -Wl,-Map,${CMAKE_BINARY_DIR}/s32k144.map
    -Wl,--cref
)

# ---------------------------------------------------------------------------
# RTOS — FreeRTOS kernel (depends on s32k1_bsp above for device headers)
# ---------------------------------------------------------------------------
add_subdirectory(rtos)
