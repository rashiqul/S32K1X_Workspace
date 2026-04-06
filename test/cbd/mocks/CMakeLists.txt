#******************************************************************************
#
#     File Name: test/cbd/mocks/CMakeLists.txt
#     Description: INTERFACE libraries exposing mock header paths for x86
#                  unit-test builds.
#
#                  bsw_mock_headers   — NXP RTD/MCAL header stubs
#                  freertos_mock_headers — FreeRTOS kernel header stubs
#
#                  Both are INTERFACE-only (headers, no compiled sources).
#                  Link against them from any test target that needs the
#                  mock include paths without pulling in compiled objects.
#
#     Author: Mohammad Rashiqul Alam
#
#******************************************************************************

add_library(bsw_mock_headers INTERFACE)
target_include_directories(bsw_mock_headers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/bsw/include
)

add_library(freertos_mock_headers INTERFACE)
target_include_directories(freertos_mock_headers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/freertos/include
)
