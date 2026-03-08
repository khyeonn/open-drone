cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico2 CACHE STRING "Board type")

set(PICO_SDK_PATH "/pico-sdk")

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(drone LANGUAGES C CXX ASM)

pico_sdk_init()

# Subdirectories
add_subdirectory(core)
add_subdirectory(platform/pico)