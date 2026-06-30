cmake_minimum_required(VERSION 3.20)
project(asyncio-examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(asyncio INTERFACE)
target_include_directories(asyncio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(sleep sleep.cc)
target_link_libraries(sleep PRIVATE asyncio)
