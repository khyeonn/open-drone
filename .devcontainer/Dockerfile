# Build picotool
FROM debian:bookworm-slim AS picotool-builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake ninja-build git g++ pkg-config build-essential \
    libusb-1.0-0-dev libudev-dev ca-certificates \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 --branch 2.2.0 https://github.com/raspberrypi/pico-sdk.git /pico-sdk \
    && cd /pico-sdk && git submodule update --init --depth 1

RUN git clone --depth 1 https://github.com/raspberrypi/picotool.git /picotool \
    && cmake -S /picotool -B /picotool/build \
    -DCMAKE_BUILD_TYPE=Release \
    -DPICO_SDK_PATH=/pico-sdk \
    -GNinja \
    && cmake --build /picotool/build \
    && cmake --install /picotool/build --prefix /picotool-dist


# Final image
FROM debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake ninja-build \
    gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib \
    libusb-1.0-0 libudev1 \
    gdb-multiarch \
    git curl ca-certificates \
    python3 python3-pip python3-venv \
    g++ pkg-config build-essential \
    clang clang-format clangd libc++-dev libc++abi-dev \
    && rm -rf /var/lib/apt/lists/*

# Just is avaible via apt only in Debian 13+ so build from prebuilt binaries
RUN curl --proto '=https' --tlsv1.2 -sSf \
    https://just.systems/install.sh | bash -s -- --to /usr/local/bin

COPY --from=picotool-builder /pico-sdk /pico-sdk
COPY --from=picotool-builder /picotool-dist /usr/local

ENV PICO_SDK_PATH=/pico-sdk

RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

RUN pip install --no-cache-dir --upgrade pip \
    && pip install --no-cache-dir \
    numpy matplotlib pandas pylint \
    'python-lsp-server[all]'

WORKDIR /workspace