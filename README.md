# Yap Language

Welcome to the Yap Programming Language! A BASIC replacement language for the Commodore 128 microcomputer.

## Development

### Prerequisites

A local installation of the LLVM MOS SDK.

Copy the contents of [`CMakeUserPresets.json.template`](./CMakeUserPresets.json.template) to a new
[`CMakeUserPresets.json`](./CMakeUserPresets.json) file and add the absolute path to the LLVM MOS SDK installation on
your system as the value of `CMAKE_PREFIX_PATH`.

```shell
docker build -t llvm-mos-docker:latest .
```

The project can also be built in the command line by running:

```shell
cmake --preset local
cmake --build build -j 6
```

```shell
x128 cmake-build-debug/yap_language.prg
```

Compile assembly to binary for 6502

```shell
../llvm-mos/bin/clang --target=mos -nostdlib -Wl,--oformat=binary [FILE]
```
