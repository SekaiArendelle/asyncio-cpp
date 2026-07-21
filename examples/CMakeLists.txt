cmake_minimum_required(VERSION 3.28)
project(asyncio-examples LANGUAGES CXX)

find_package(asyncio REQUIRED)

add_executable(coro_func coro_func.cc)
target_compile_features(coro_func PUBLIC cxx_std_23)
set_target_properties(coro_func PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(coro_func PRIVATE asyncio::asyncio)
