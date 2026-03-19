cmake_minimum_required(VERSION 3.22.0)
project(PackageTest)

find_package(CCppTemplateWorkspace CONFIG REQUIRED)

# Import upstream sources
set(upstream_dir ${PROJECT_BINARY_DIR}/upstream)
file(GLOB upstream_cmake_files RELATIVE ${upstream_dir} ${upstream_dir}/*/src/CMakeLists.txt)
foreach(cmake_file ${upstream_cmake_files})
    get_filename_component(dir ${cmake_file} DIRECTORY)
    add_subdirectory(${upstream_dir}/${dir} ${dir})
endforeach()

target_link_libraries(template PRIVATE template_stub)

add_executable(test_package src/test_package.cpp)
target_link_libraries(test_package PRIVATE template)