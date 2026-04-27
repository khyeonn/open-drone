# Default task
verify:
    @echo "\033[1;33m####################### Verifying container #######################\033[0m"
    @echo "PICO_SDK_PATH: $PICO_SDK_PATH"
    @echo "picotool version: `picotool version`"
    @echo "ARM GCC version: `arm-none-eabi-gcc --version | head -n 1`"
    @echo "CMake version: `cmake --version | head -n 1`"
    @echo "Ninja version: `ninja --version`"
    @echo "Python version: `python3 --version`"
    @echo ""

compile-commands:
    #!/usr/bin/env python3
    import json
    files = ["build-sim/compile_commands.json", "build/compile_commands.json"]
    merged = []
    for f in files:
        try:
            with open(f) as fp:
                merged += json.load(fp)
        except FileNotFoundError:
            pass
    with open("compile_commands.json", "w") as fp:
        json.dump(merged, fp, indent=2)

# Remove build dir
clean-pico:
    @echo "\033[1;31m####################### Cleaning firmware build folder #######################\033[0m"
    rm -rf build
    @echo "\033[1;31m####################### Done cleaning firmware build folder! #######################\033[0m\n"

# Remove build-sim dir
clean-sim:
    @echo "\033[1;31m####################### Cleaning simulation build folder #######################\033[0m"
    rm -rf build-sim
    @echo "\033[1;31m####################### Done cleaning sim build folder! #######################\033[0m\n"

# Clean all
clean: clean-pico clean-sim

# Configure firmware build
configure-pico:
    @echo "\033[1;34m####################### Configuring firmware CMake #######################\033[0m"
    cmake -S . -B build \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    @echo "\033[1;34m####################### Done configuring firmware CMake! #######################\033[0m\n"

# Configure simulator build
configure-sim:
    @echo "\033[1;34m####################### Configuring simulator CMake #######################\033[0m"
    cmake -S sim -B build-sim \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    @echo "\033[1;34m####################### Done configuring sim CMake! #######################\033[0m\n"

# Configure both builds
configure: configure-pico configure-sim
    @echo "\033[1;34m####################### Merging compile commands #######################\033[0m"
    @just compile-commands
    @echo "\033[1;34m####################### Done merging compile commands! #######################\033[0m\n"

# Build firmware
build-pico:
    @echo "\033[1;34m####################### Building firmware... #######################\033[0m"
    cmake --build build -j
    just compile-commands
    @echo "\033[1;34m####################### Done building firmware! #######################\033[0m\n"

build-sim:
    @echo "\033[1;34m####################### Building simulator... #######################\033[0m"
    mkdir -p build-sim
    cmake -S sim -B build-sim -DCMAKE_BUILD_TYPE=Debug
    cmake --build build-sim -j
    just compile-commands
    @echo "\033[1;34m####################### Done building simulator! #######################\033[0m\n"

# Build everything
build: build-pico build-sim
    @just compile-commands

run-sim:
    @echo "\033[1;32m####################### Running simulation #######################\033[0m"
    ./build-sim/drone_sim
    @echo "\033[1;32m####################### Done running sim! #######################\033[0m\n"

validate case:
    @echo "\033[1;32m####################### Validating simulation case {{ case }} #######################\033[0m"
    ./build-sim/validate_sim_{{ case }}
    @echo "\033[1;32m####################### Done validating simulation case {{ case }} #######################\033[0m\n"

plot case:
    @echo "\033[1;32m####################### Plotting simulation case {{ case }} #######################\033[0m"
    python3 ./sim/validation/plot_sim_results.py --case case_{{ case }}
    @echo "\033[1;32m####################### Done generating plots for case {{ case }} #######################\033[0m\n"
