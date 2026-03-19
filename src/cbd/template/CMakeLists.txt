add_library(template template.c)
target_include_directories(template PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/cbd/template
)