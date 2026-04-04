#******************************************************************************
#
#     File Name: src/cbd/iohal/CMakeLists.txt
#     Description: Build rules for the IoHal software component.
#
#                  IoHal wraps BSW (s32k1_bsp) and FreeRTOS (freertos) APIs
#                  so it can only be compiled for the ARM target.  The host
#                  x86 build (unit-test) skips this target because it has no
#                  BSW stack.  Add stub/mock variants here if x86 unit tests
#                  for IoHal are needed in future.
#
#     Author: Mohammad Rashiqul Alam
#
#******************************************************************************

add_library(iohal STATIC
    iohal.c
    iohal_can/iohal_can.c
    iohal_os/iohal_os.c
    iohal_app_tasks/iohal_app_tasks.c
)

target_include_directories(iohal PUBLIC
    # Umbrella header: iohal.h
    ${CMAKE_SOURCE_DIR}/include/cbd/iohal
    # Per-module headers — each subfolder added so flat #include still works
    ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_can
    ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_os
    ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_app_tasks
)

if(CMAKE_CROSSCOMPILING)
    target_link_libraries(iohal PRIVATE
        s32k1_bsp   # NXP RTD drivers: Mcu, Port, Dio, CanIf, Can_43_FLEXCAN
        freertos    # FreeRTOS kernel: task, queue, portable
    )
else()
    # Host build: use test-only mock headers for low-layer RTD/FreeRTOS includes.
    target_include_directories(iohal PRIVATE
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/bsw/include
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/freertos/include
    )
endif()
