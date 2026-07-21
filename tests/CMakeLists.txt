cmake_minimum_required(VERSION 3.28)
project(asyncio-tests LANGUAGES CXX)

find_package(asyncio REQUIRED)

enable_testing()

file(GLOB TEST_SOURCES "test_*.cc")

foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_compile_features(${test_name} PUBLIC cxx_std_23)
    set_target_properties(${test_name} PROPERTIES CXX_EXTENSIONS OFF)
    target_link_libraries(${test_name} PRIVATE asyncio::asyncio)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
