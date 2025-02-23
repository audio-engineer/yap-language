FROM ubuntu:latest AS builder

ENV TZ=Europe/Copenhagen

RUN set -eux; \
    \
    apt-get update; \
    apt-get upgrade -y; \
    # Install prerequisite dependencies
    apt-get install -y \
    software-properties-common \
    ninja-build \
    build-essential \
    python3 \
    wget \
    gdb \
    ; \
    add-apt-repository -y \
    ppa:git-core/ppa \
    ; \
    apt-get update; \
    apt-get install -y \
    git \
    ; \
    test -f /usr/share/doc/kitware-archive-keyring/copyright || \
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null; \
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ noble main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null; \
    apt-get update; \
    test -f /usr/share/doc/kitware-archive-keyring/copyright || \
    rm /usr/share/keyrings/kitware-archive-keyring.gpg; \
    apt-get install -y \
    kitware-archive-keyring \
    cmake=3.30.6-0kitware1ubuntu24.04.1 \
    ; \
    apt-mark hold cmake; \
    rm -rf /var/lib/apt/lists/*; \
    # Install LLVM-MOS
    cd ~; \
    git clone https://github.com/llvm-mos/llvm-mos.git; \
    cd llvm-mos/; \
    cmake -C clang/cmake/caches/MOS.cmake -G Ninja -S llvm -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/; \
    cmake --build build -j 5 -t distribution; \
    cmake --build build -j 5 -t install; \
    # Install LLVM-MOS-SDK
    cd ~; \
    git clone https://github.com/llvm-mos/llvm-mos-sdk; \
    cd llvm-mos-sdk/; \
    cmake -G Ninja -B build -DCMAKE_INSTALL_PREFIX=/usr/local/ -DLLVM_MOS=/usr/local/bin/; \
    cmake --build build -j 5 -t install; \
    rm -rf ~/llvm-mos ~/llvm-mos-sdk;
