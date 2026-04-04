#*******************************************************************************
#
#     File Name: src/rtos/CMakeLists.txt
#     Description: Builds the FreeRTOS V11.1.0 kernel as a static library for
#                  the S32K144 ARM Cortex-M4F target.
#
#                  Port    : GCC/ARM_CM4F (hardware FPU, lazy context save)
#                  Heap    : heap_1 (static — upgrade to heap_4 when vTaskDelete
#                            or pvPortFree is required)
#                  Tick    : SysTick @ 1 kHz (configured by port.c directly)
#                  Handlers: SVC_Handler / PendSV_Handler / SysTick_Handler
#                            override BSP weak symbols at link time via
#                            FreeRTOSConfig.h defines.
#
#     Portability note: this target is self-contained. Other feature branches
#     (e.g. CAN driver) can simply add:
#         target_link_libraries(<their_target> PRIVATE freertos)
#     No BSP or linker changes are needed — FreeRTOS uses SysTick directly.
#
#     Author: Mohammad Rashiqul Alam
#
#*******************************************************************************

set(FREERTOS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/Source)

add_library(freertos STATIC
    # Core kernel
    ${FREERTOS_SRC}/tasks.c
    ${FREERTOS_SRC}/queue.c
    ${FREERTOS_SRC}/list.c
    ${FREERTOS_SRC}/timers.c
    ${FREERTOS_SRC}/event_groups.c
    ${FREERTOS_SRC}/stream_buffer.c

    # ARM Cortex-M4F port (SysTick, PendSV, context switch, FPU lazy save)
    ${FREERTOS_SRC}/portable/GCC/ARM_CM4F/port.c

    # Heap allocator — heap_1: simple static bump allocator, no freeing.
    # Switch to heap_4.c for projects that need vTaskDelete() / pvPortFree().
    ${FREERTOS_SRC}/portable/MemMang/heap_1.c
)

target_include_directories(freertos PUBLIC
    # FreeRTOS kernel headers (FreeRTOS.h, task.h, queue.h, semphr.h, ...)
    ${FREERTOS_SRC}/include

    # Port-specific header (portmacro.h)
    ${FREERTOS_SRC}/portable/GCC/ARM_CM4F

    # Project-level FreeRTOS configuration (FreeRTOSConfig.h)
    # Moved to include/bsw/rtos/config/ as part of src/include separation.
    ${PROJECT_SOURCE_DIR}/include/bsw/rtos/config
)

# Inherit BSP include paths so port.c can reach S32K144 device headers
# (e.g. for __NVIC_PRIO_BITS and cortex-m intrinsics if needed).
target_link_libraries(freertos PRIVATE s32k1_bsp)

# Suppress warnings in vendor FreeRTOS kernel code — we do not own these files.
target_compile_options(freertos PRIVATE
    -Wno-unused-parameter
    -Wno-sign-compare
)
