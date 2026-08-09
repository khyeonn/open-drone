# Default task
verify:
    @printf "\033[1;33m####################### Verifying container #######################\033[0m\n"
    @printf "PICO_SDK_PATH: $PICO_SDK_PATH"
    @printf "picotool version: `picotool version`"
    @printf "ARM GCC version: `arm-none-eabi-gcc --version | head -n 1`"
    @printf "CMake version: `cmake --version | head -n 1`"
    @printf "Ninja version: `ninja --version`"
    @printf "Python version: `python3 --version`"
    @printf ""

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
    @printf "\033[1;31m####################### Cleaning firmware build folder #######################\033[0m\n"
    rm -rf build
    @printf "\033[1;31m####################### Done cleaning firmware build folder! #######################\033[0m\n"

# Remove build-sim dir
clean-sim:
    @printf "\033[1;31m####################### Cleaning simulation build folder #######################\033[0m\n"
    rm -rf build-sim
    @printf "\033[1;31m####################### Done cleaning sim build folder! #######################\033[0m\n"

# Clean all
clean: clean-pico clean-sim

# Configure firmware build
configure-pico:
    @printf "\033[1;34m####################### Configuring firmware CMake #######################\033[0m\n"
    cmake -S . -B build \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    @printf "\033[1;34m####################### Done configuring firmware CMake! #######################\033[0m\n"

# Configure simulator build
configure-sim:
    @printf "\033[1;34m####################### Configuring simulator CMake #######################\033[0m\n"
    cmake -S sim -B build-sim \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    @printf "\033[1;34m####################### Done configuring sim CMake! #######################\033[0m\n"

# Configure both builds
configure: configure-pico configure-sim
    @printf "\033[1;34m####################### Merging compile commands #######################\033[0m\n"
    @just compile-commands
    @printf "\033[1;34m####################### Done merging compile commands! #######################\033[0m\n"

# Build firmware
build-pico:
    @printf "\033[1;34m####################### Building firmware... #######################\033[0m\n"
    cmake --build build -j
    just compile-commands
    @printf "\033[1;34m####################### Done building firmware! #######################\033[0m\n"

build-sim:
    @printf "\033[1;34m####################### Building simulator... #######################\033[0m\n"
    mkdir -p build-sim
    cmake -S sim -B build-sim -DCMAKE_BUILD_TYPE=Debug
    cmake --build build-sim -j
    just compile-commands
    @printf "\033[1;34m####################### Done building simulator! #######################\033[0m\n"

# Build everything
build: build-pico build-sim
    @just compile-commands

run-sim:
    @printf "\033[1;32m####################### Running simulation #######################\033[0m\n"
    ./build-sim/drone_sim
    @printf "\033[1;32m####################### Done running sim! #######################\033[0m\n"

validate case:
    @printf "\033[1;32m####################### Validating simulation case {{ case }} #######################\033[0m\n"
    ./build-sim/validate_sim_{{ case }}
    @printf "\033[1;32m####################### Done validating simulation case {{ case }} #######################\033[0m\n"

plot case:
    @printf "\033[1;32m####################### Plotting simulation case {{ case }} #######################\033[0m\n"
    python3 ./sim/validation/plot_sim_results.py --case case_{{ case }}
    @printf "\033[1;32m####################### Done generating plots for case {{ case }} #######################\033[0m\n"

# Launch the realtime IMU visualizer against the Pico over USB serial
visualize:
    @printf "\033[1;32m####################### Launching IMU visualizer #######################\033[0m\n"
    python3 ./visualize/imu_visualizer.py

# Launch the IMU visualizer with synthetic data (no hardware needed)
visualize-sim:
    @printf "\033[1;32m####################### Launching IMU visualizer (simulated data) #######################\033[0m\n"
    python3 ./visualize/imu_visualizer.py --source sim

# Replay a recorded telemetry CSV file
visualize-file file:
    @printf "\033[1;32m####################### Replaying {{ file }} #######################\033[0m\n"
    python3 ./visualize/imu_visualizer.py --source file --file {{ file }}

# Build + run the visualizer in a plain docker container (no devcontainer)
visualize-docker:
    @printf "\033[1;32m####################### Building visualizer image #######################\033[0m\n"
    docker compose build visualizer
    @printf "\033[1;32m####################### Launching IMU visualizer in docker #######################\033[0m\n"
    docker compose run --rm visualizer
