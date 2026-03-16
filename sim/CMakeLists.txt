cmake_minimum_required(VERSION 3.13)
project(drone_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_BINARY_DIR}/core_build
)

file(GLOB SRC_DIRS CONFIGURE_DEPENDS src/*)

foreach(dir ${SRC_DIRS})
    if(IS_DIRECTORY ${dir})
        add_subdirectory(${dir})
    endif()
endforeach()

# Shared configuration
add_library(sim_common INTERFACE)

target_include_directories(sim_common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/sim
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/core
)

target_link_libraries(sim_common INTERFACE
    drone_core
    sim_physics
)

function(add_sim TARGET SOURCE)
    add_executable(${TARGET} ${SOURCE})
    target_link_libraries(${TARGET} PRIVATE sim_common)
endfunction()

add_sim(drone_sim main.cpp)
add_sim(validate_sim_01 validate_case_01.cpp)
add_sim(validate_sim_02 validate_case_02.cpp)
