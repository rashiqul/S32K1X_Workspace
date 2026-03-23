#******************************************************************************
#
#     File Name: src/CMakeLists.txt
#     Description: Builds the S32K144 firmware executable (ARM cross-compile)
#                  or the x86 common_lib and CBD submodules (host build).
#
#     Author: Mohammad Rashiqul Alam
#
#******************************************************************************

if(CMAKE_CROSSCOMPILING)
    # -------------------------------------------------------------------------
    # ARM target: S32K144 firmware executable
    # The NXP Platform SDK already provides Platform_Types.h / Std_Types.h via
    # s32k1_bsp, so we must NOT also pull in libs/autosar-platform-types here.
    # -------------------------------------------------------------------------
    add_executable(s32k144_firmware
        ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    )

    # Give the ELF a proper .elf extension
    set_target_properties(s32k144_firmware PROPERTIES SUFFIX ".elf")

    target_link_libraries(s32k144_firmware PRIVATE s32k1_bsp s32k1_linker_config)

    # Post-build: consolidate all artifacts into bin/
    add_custom_command(TARGET s32k144_firmware POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_FILE:s32k144_firmware>
                ${CMAKE_BINARY_DIR}/bin/s32k144_firmware.elf
        COMMAND ${CMAKE_OBJCOPY} -O ihex
                $<TARGET_FILE:s32k144_firmware>
                ${CMAKE_BINARY_DIR}/bin/s32k144_firmware.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary
                $<TARGET_FILE:s32k144_firmware>
                ${CMAKE_BINARY_DIR}/bin/s32k144_firmware.bin
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_BINARY_DIR}/s32k144_firmware.map
                ${CMAKE_BINARY_DIR}/bin/s32k144_firmware.map
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:s32k144_firmware>
        COMMENT "Consolidating firmware artifacts into bin/"
    )
else()
    # -------------------------------------------------------------------------
    # Host x86 build
    # -------------------------------------------------------------------------
    add_library(common_lib INTERFACE)
    target_include_directories(common_lib INTERFACE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/libs/autosar-platform-types/include
    )
    link_libraries(common_lib)

    # Scan and add all subdirectories with CMakeLists.txt
    file(GLOB subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
    foreach(dir ${subdirs})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir} AND
           EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/CMakeLists.txt)
            add_subdirectory(${dir})
        endif()
    endforeach()

    install(FILES CMakeLists-export.txt RENAME CMakeLists.txt DESTINATION src)
endif()
