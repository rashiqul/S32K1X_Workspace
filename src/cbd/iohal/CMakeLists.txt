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

if(CMAKE_CROSSCOMPILING)

    add_library(iohal STATIC
        iohal.c
        iohal_can.c
        iohal_os.c
        iohal_app_tasks.c
    )

    target_include_directories(iohal PUBLIC
        # Public interface headers (iohal.h, iohal_can.h, iohal_os.h, iohal_app_tasks.h)
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal
    )

    target_link_libraries(iohal PRIVATE
        s32k1_bsp   # NXP RTD drivers: Mcu, Port, Dio, CanIf, Can_43_FLEXCAN
        freertos    # FreeRTOS kernel: task, queue, portable
    )

endif()
