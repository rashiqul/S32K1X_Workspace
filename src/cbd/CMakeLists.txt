# GHS compiler support removed for local x86_64 development

file(GLOB subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(dir ${subdirs})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/CMakeLists.txt)
        add_subdirectory(${dir})

        set(component_name ${dir})

        file(GLOB_RECURSE sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${component_name}/*.[ch])
        install(FILES ${sources} DESTINATION src/${component_name})
        install(FILES ${dir}/CMakeLists-export.txt RENAME CMakeLists.txt DESTINATION src/${component_name})
        install(FILES ${PROJECT_SOURCE_DIR}/include/rtwtypes.h OPTIONAL DESTINATION src/${component_name}/stub)
        install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/cbd/${component_name}/ OPTIONAL DESTINATION src/${component_name})
        install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/cbd/rte/${component_name}/ OPTIONAL DESTINATION src/${component_name}/stub)
        install(FILES ${PROJECT_BINARY_DIR}/config.h DESTINATION src/${component_name})
    endif()
endforeach()
