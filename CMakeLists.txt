cmake_minimum_required(VERSION 3.30)
project(yap_language C)

option(COMMODORE "Build for the Commodore 128" OFF)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (COMMODORE)
    message(STATUS "Configuring for the Commodore 128 platform")

    set(LLVM_MOS_PLATFORM c128)

    find_package(llvm-mos-sdk REQUIRED)
else ()
    message(STATUS "Configuring for the native platform")
endif ()

add_executable(yap-lang
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

    target_compile_options(yap-lang PRIVATE -Os -flto)

    target_link_options(yap-lang PRIVATE -Os -flto)
else ()
    message(STATUS "Configuring unoptimized build")
endif ()

if (COMMODORE)
    target_compile_definitions(yap-lang PRIVATE -DCOMMODORE)

    set_target_properties(yap-lang PROPERTIES SUFFIX ".prg")
endif ()
