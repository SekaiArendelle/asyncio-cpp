cmake_minimum_required(VERSION 3.28)
project(asyncio-examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(asyncio REQUIRED)

add_executable(coro_func coro_func.cc)
target_link_libraries(coro_func PRIVATE asyncio::asyncio)
