#******************************************************************************
#
#     File Name: arm-none-eabi.cmake
#     Description: CMake toolchain file for ARM Cortex-M4 bare-metal target
#                  Targets NXP S32K144 (ARM Cortex-M4 @ up to 112 MHz)
#
#     Toolchain: arm-none-eabi-gcc (NXP GCC 10.2 or system arm-none-eabi-gcc)
#     Author   : Mohammad Rashiqul Alam
#
#******************************************************************************

# Tell CMake we are cross-compiling to a bare-metal target
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Prefer NXP bundled GCC; fall back to system arm-none-eabi-gcc
if(DEFINED ENV{NXP_GCC_PATH})
    set(GCC_BIN "$ENV{NXP_GCC_PATH}/")
elseif(EXISTS "/home/rashiqul/NXP/gcc-10.2-arm32-eabi/bin/arm-none-eabi-gcc")
    set(GCC_BIN "/home/rashiqul/NXP/gcc-10.2-arm32-eabi/bin/")
else()
    set(GCC_BIN "")
endif()

set(CMAKE_C_COMPILER   "${GCC_BIN}arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${GCC_BIN}arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "${GCC_BIN}arm-none-eabi-gcc")

# Find post-processing tools
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy HINTS "${GCC_BIN}")
find_program(CMAKE_OBJDUMP arm-none-eabi-objdump HINTS "${GCC_BIN}")
find_program(CMAKE_SIZE    arm-none-eabi-size    HINTS "${GCC_BIN}")

# Cortex-M4 with FPv4-SP-D16 (hard float)
# -fno-short-enums: use 32-bit enums (required to match NXP RTD ABI)
set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-short-enums")

set(CMAKE_C_FLAGS_INIT   "${MCU_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${MCU_FLAGS}")
# -x assembler-with-cpp: run C preprocessor on .s files (needed for #define, #ifdef, /* */ comments)
set(CMAKE_ASM_FLAGS_INIT "${MCU_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--gc-sections")

# Skip compiler validation (can't run ARM binaries on host)
set(CMAKE_C_COMPILER_WORKS   1 CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER_WORKS 1 CACHE INTERNAL "")

# Don't search host paths for libraries/headers
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
