cmake_minimum_required(VERSION 3.30)
project(yap_language C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(yap-lang-native
        src/main.c
        src/lexer.c
        src/lexer.h
        src/parser.c
        src/parser.h
        src/vm.c
        src/vm.h
)
