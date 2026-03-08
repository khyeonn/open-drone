# Default task
verify:
    @echo "####################### Verifying container #######################"
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

# Clean build
clean-pico:
    @echo "####################### Cleaning firmware build folder #######################"
    rm -rf build

# Clean build-sim
clean-sim:
    @echo "####################### Cleaning simulation build folder #######################"
    rm -rf build-sim

# Clean all
clean:
    @echo "####################### Cleaning all builds #######################"
    rm -rf build
    rm -rf build-sim

# Configuring CMake for firmwre
configure:
    @echo "####################### Configuring CMake #######################"
    cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build firmware
build-pico:
    @echo "####################### Building firmware... #######################"
    cmake --build build -j
    just compile-commands

build-sim:
    @echo "####################### Building simulator... #######################"
    mkdir -p build-sim
    cmake -S sim -B build-sim -DCMAKE_BUILD_TYPE=Debug
    cmake --build build-sim
    just compile-commands

run-sim:
    @echo "####################### Running simulation #######################"
    ./build-sim/drone_sim
