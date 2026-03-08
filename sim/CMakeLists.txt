cmake_minimum_required(VERSION 3.13)
project(drone_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(drone_sim
    main.cpp
)

target_include_directories(drone_sim PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/sim
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/core
)

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../core 
    ${CMAKE_BINARY_DIR}/core_build
)

add_subdirectory(physics)


target_link_libraries(drone_sim PRIVATE drone_core)