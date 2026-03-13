add_library(sim_physics
    compute_flight_conditions.cpp
    ussa1976.cpp
)

target_link_libraries(sim_physics PUBLIC drone_core)
target_include_directories(sim_physics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)