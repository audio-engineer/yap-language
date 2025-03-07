cmake_minimum_required(VERSION 3.30)
project(yap_language C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Configuring for the native platform")

add_executable(yap-lang-native
        src/main.c
        src/lexer.c
        src/lexer.h
        src/parser.c
        src/parser.h
        src/vm.c
        src/vm.h
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(STATUS "Configuring optimized build")

    target_compile_options(yap-lang-native PRIVATE -Os)

    target_link_options(yap-lang-native PRIVATE -Os)
else ()
    message(STATUS "Configuring unoptimized build")
endif ()
