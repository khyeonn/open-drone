add_library(drone_core STATIC)

# Public headers for consumers
target_include_directories(drone_core
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(estimation)

# Require C++17
target_compile_features(drone_core PUBLIC cxx_std_17)

# Warnings for core
target_compile_options(drone_core PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
