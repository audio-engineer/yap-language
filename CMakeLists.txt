cmake_minimum_required(VERSION 3.30)
project(yap_language C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LLVM_MOS_PLATFORM c128)

find_package(llvm-mos-sdk REQUIRED)

add_executable(yap-lang
        src/main.c
        src/lexer.c
        src/lexer.h
        src/parser.c
        src/parser.h
)

target_compile_options(yap-lang PRIVATE -Os)

target_link_options(yap-lang PRIVATE -Os)

set_target_properties(yap-lang PROPERTIES
        SUFFIX ".prg"
)
