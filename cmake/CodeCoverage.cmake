# Simple Code Coverage Module for local development
# Adds coverage compiler flags for GCC/Clang

function(append_coverage_compiler_flags)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Appending code coverage compiler flags")
        add_compile_options(-fprofile-arcs -ftest-coverage)
        add_link_options(-fprofile-arcs -ftest-coverage)
        
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            link_libraries(gcov)
        endif()
    else()
        message(WARNING "Code coverage not supported for compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()
