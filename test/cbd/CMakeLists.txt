# Mock INTERFACE libraries (bsw_mock_headers, freertos_mock_headers) must be
# defined before unit_tests so test targets can link against them.
add_subdirectory(mocks)
add_subdirectory(unit_tests)