#******************************************************************************
#
#     File Name: src/cbd/app_tasks/CMakeLists.txt
#     Description: Application task layer — ARM target only.
#                  Depends on iohal (which depends on s32k1_bsp + freertos).
#
#     Author: Mohammad Rashiqul Alam
#
#******************************************************************************

if(CMAKE_CROSSCOMPILING)

    add_library(app_tasks STATIC
        app_tasks.c
    )

    target_include_directories(app_tasks PUBLIC
        ${CMAKE_SOURCE_DIR}/include/cbd/app_tasks
    )

    target_link_libraries(app_tasks PRIVATE
        iohal
    )

endif()
