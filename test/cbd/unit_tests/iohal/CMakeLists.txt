add_library(mock_iohal STATIC
    mocks/mock_iohal.cpp
)

add_library(mock_low_level STATIC
    mocks/mock_low_level.cpp
)

target_include_directories(mock_iohal
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mocks
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/libs/autosar-platform-types/include
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_can
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_os
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_app_tasks
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/bsw/include
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/freertos/include
)

target_link_libraries(mock_iohal
    PUBLIC
        GTest::gmock
)

target_include_directories(mock_low_level
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mocks
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/libs/autosar-platform-types/include
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_can
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_os
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_app_tasks
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/bsw/include
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/freertos/include
)

target_link_libraries(mock_low_level
    PUBLIC
        GTest::gmock
)

add_executable(test_iohal
    test_iohal.cpp
    ${CMAKE_SOURCE_DIR}/src/cbd/iohal/iohal.c
)

target_include_directories(test_iohal
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/libs/autosar-platform-types/include
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_can
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_os
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_app_tasks
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/bsw/include
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/freertos/include
)

target_link_libraries(test_iohal
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        mock_low_level
)

add_executable(test_iohal_can
    test_iohal_can.cpp
    ${CMAKE_SOURCE_DIR}/src/cbd/iohal/iohal_can/iohal_can.c
)

target_include_directories(test_iohal_can
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/libs/autosar-platform-types/include
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_can
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_os
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_app_tasks
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/bsw/include
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/freertos/include
)

target_link_libraries(test_iohal_can
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        mock_low_level
)

add_executable(test_iohal_os
    test_iohal_os.cpp
    ${CMAKE_SOURCE_DIR}/src/cbd/iohal/iohal_os/iohal_os.c
)

target_include_directories(test_iohal_os
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/libs/autosar-platform-types/include
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_can
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_os
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_app_tasks
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/bsw/include
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/freertos/include
)

target_link_libraries(test_iohal_os
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        mock_low_level
)

add_executable(test_iohal_app_tasks
    test_iohal_app_tasks.cpp
    ${CMAKE_SOURCE_DIR}/src/cbd/iohal/iohal_app_tasks/iohal_app_tasks.c
)

target_compile_definitions(test_iohal_app_tasks
    PRIVATE
        IOHAL_APP_TASK_LOOP_ITERATIONS=1
)

target_include_directories(test_iohal_app_tasks
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/libs/autosar-platform-types/include
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_can
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_os
        ${CMAKE_SOURCE_DIR}/include/cbd/iohal/iohal_app_tasks
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/bsw/include
        ${CMAKE_SOURCE_DIR}/test/cbd/mocks/freertos/include
)

target_link_libraries(test_iohal_app_tasks
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        mock_iohal
)

include(GoogleTest)
gtest_discover_tests(test_iohal)
gtest_discover_tests(test_iohal_can)
gtest_discover_tests(test_iohal_os)
gtest_discover_tests(test_iohal_app_tasks)
