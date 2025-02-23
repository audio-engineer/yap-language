# Yap Language

Welcome to the Yap Programming Language! A BASIC replacement language for the Commodore 128 microcomputer.

## Development

### Setup

You need to have the following software installed on your development system:

- [LLVM-MOS SDK](https://github.com/llvm-mos/llvm-mos-sdk)
- [CMake](https://cmake.org/)
- [Ninja](https://ninja-build.org/)

Consider running [build-llvm-mos](./build-llvm-mos) to install the prerequisite software:

```shell
chmod +x build-llvm-mos
./build-llvm-mos
```

Alternatively, the provided [Dockerfile](./Dockerfile) can be built. The image contains the entire necessary toolchain:

```shell
docker build -t llvm-mos-docker:latest .
```

Copy the contents of [`CMakeUserPresets.json.template`](./CMakeUserPresets.json.template) to a new
[`CMakeUserPresets.json`](./CMakeUserPresets.json) file and add the absolute path to the
[LLVM-MOS SDK](https://github.com/llvm-mos/llvm-mos-sdk) installation on your system as the value of
`CMAKE_PREFIX_PATH`.

### Develop

To lint the project:

```shell
clang-tidy --config-file .clang-tidy -p build src/*
```

To format the project according to the configured style:

```shell
clang-format -i src/*
```

### Build

The project can also be built in the command line by running:

```shell
cmake --preset local
cmake --build build -j 6
```

Compile assembly to binary:

```shell
mos-c128-clang -nostdlib -Wl,--oformat=binary [FILE]
```

### Run

```shell
x128 build/yap-lang.prg
```
