FROM ubuntu:24.04

# Skip UI setup
ENV DEBIAN_FRONTEND=noninteractive

# Install packages
RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    libusb-1.0-0-dev \
    libudev-dev \
    gdb-multiarch \
    git \
    python3 \
    python3-pip \
    python3-venv \
    g++ \
    just \
    pkg-config \
    build-essential \
    clang \
    clang-format \
    clangd \
    libc++-dev \
    libc++abi-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Pico SDK
RUN git clone --depth 1 --branch 2.2.0 https://github.com/raspberrypi/pico-sdk.git pico-sdk \
    && cd pico-sdk \
    && git submodule update --init

# Set Pico SDK path
ENV PICO_SDK_PATH=/pico-sdk

# Install picotool
RUN git clone https://github.com/raspberrypi/picotool.git \
    && cd picotool \
    && mkdir build \
    && cd build \
    && cmake -DPICO_SDK_PATH=$PICO_SDK_PATH .. \
    && make -j4 \
    && make install

WORKDIR /workspace

RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

RUN pip install --upgrade pip \
    && pip install numpy \
    && pip install matplotlib \
    && pip install pandas \
    && pip install pylint \
    && pip install 'python-lsp-server[all]'