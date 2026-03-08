add_executable(drone_pico
    main.cpp
)

# Include directories for core library headers
target_include_directories(drone_pico PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/core
    ${CMAKE_CURRENT_LIST_DIR}/../../include/pico
)

add_subdirectory(sensors)

# Link the core library and Pico SDK libraries
target_link_libraries(drone_pico
    drone_core
    pico_stdlib
    hardware_i2c
    hardware_pwm
    hardware_adc
)

# Enable USB output for stdio
pico_enable_stdio_uart(drone_pico 0)
pico_enable_stdio_usb(drone_pico 1)

# Generate UF2, ELF, HEX outputs
pico_add_extra_outputs(drone_pico)
