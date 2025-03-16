# Yap Language

Welcome to the Yap Programming Language! A BASIC replacement language for the Commodore 128 microcomputer.

## Development

### Setup

You need to have the following software installed on your development system:

- [cc65](https://cc65.github.io/)
- [LLVM/Clang](https://llvm.org/)
- [CMake](https://cmake.org/)
- [Ninja](https://ninja-build.org/)

1. Copy the contents of [`CMakeUserPresets.json.template`](./CMakeUserPresets.json.template) to a new
   [`CMakeUserPresets.json`](./CMakeUserPresets.json) file and add the absolute path to
   the [LLVM/Clang](https://llvm.org/) compiler binary on your system as the value of `CMAKE_C_COMPILER`.
2. Copy the contents of [`.env.template`](./.env.template) to a new [`.env`](./.env) file, and set `CC65_PATH` to the
   path to the location of `cc65` on your system.
   Also, set `LLVM_BIN_PATH` to the location of the `bin/` directory that contains the LLVM binaries.

### Develop

To lint the project, run:

```shell
make lint
```

This will run `ClangTidy` twice on the source code: Once with respect to the C99 standard for the Commodore target, and
once with respect to the CMake compilation database for the native target.

To format the project according to the configured style, run:

```shell
make format
```

This will run `ClangFormat` on the source code.

### Build in CLion

To build the native binaries, run the `yap-lang-native` run configuration in CLion, either with the `native-debug-local`
or `native-release-local` preset selected.

To build the Commodore binary, run the `build` run configuration in CLion.

### Build on the Command Line

The project can also be built on the command line.

#### Commodore 128/VICE

To build the Commodore 128 debug target, run:

```shell
make
```

You can also set the `BUILD_TYPE` variable to `Release` to build the target without debugging features enabled:

```shell
make BUILD_TYPE=Release
```

If you want to remove all build files before creating a new build, run:

```shell
make clean
```

#### Native

```shell
cmake --preset native-debug-local
cmake --build build-native-debug
```

```shell
cmake --preset native-release-local
cmake --build build-native-release
```

### Tests

Tests can be run either in CLion by running the `All CTest` configuration or on the command line by running:

```shell
ctest --test-dir build-native-release
```

### Run

```shell
x128 build-c128-release/yap-lang.prg
```
