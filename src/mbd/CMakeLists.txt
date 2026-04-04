# Model-Based Design (MBD) components
# Each subdirectory contains a generated or handwritten MBD component.

file(GLOB subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(dir ${subdirs})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/CMakeLists.txt)
        add_subdirectory(${dir})
    endif()
endforeach()
