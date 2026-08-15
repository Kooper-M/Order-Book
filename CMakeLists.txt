cmake_minimum_required(VERSION 3.19)

project(OrderBook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PYBIND11_FINDPYTHON ON)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

add_executable(orderbook_cli
    OrderBook.h
    OrderBook.cpp
    main.cpp
)

pybind11_add_module(OrderBook
    binding.cpp
    OrderBook.cpp
    OrderBook.h
)
set_target_properties(OrderBook PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_options(orderbook_cli PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(orderbook_cli PRIVATE stdc++exp)
target_link_libraries(OrderBook PRIVATE stdc++exp)

if(MINGW)
    target_link_options(OrderBook PRIVATE -static-libgcc -static-libstdc++)
endif()


target_include_directories(orderbook_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Headers
)

