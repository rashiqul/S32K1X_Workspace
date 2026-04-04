add_executable(test_template test_template.cpp)
target_link_libraries(test_template 
    PRIVATE GTest::gtest GTest::gtest_main template spdlog fmt
)
# Simplified CMake configuration
include(GoogleTest)
gtest_discover_tests(test_template)
