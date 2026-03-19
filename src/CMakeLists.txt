
add_library(common_lib INTERFACE)
target_include_directories(common_lib INTERFACE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/libs/autosar-platform-types/include
)

link_libraries(common_lib)

file(GLOB subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(dir ${subdirs})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/CMakeLists.txt)
        add_subdirectory(${dir})
    endif()
endforeach()

install(FILES CMakeLists-export.txt RENAME CMakeLists.txt DESTINATION src)
