cmake_minimum_required(VERSION 3.30)
project(yap_language C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXECUTABLE_NAME yap-lang-native)
set(LIBRARY_NAME yap-lang-native-lib)
set(TEST_EXECUTABLE_NAME yap-lang-native-tests)

include(FetchContent)
enable_testing()

# Fetch Unity
FetchContent_Declare(
        Unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG v2.6.1
)

FetchContent_MakeAvailable(Unity)

# Native library
add_library(${LIBRARY_NAME}
        src/lexer.c
        src/parser.c
        src/vm.c
)

target_include_directories(${LIBRARY_NAME} PUBLIC src)

# Native executable
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")

add_executable(${EXECUTABLE_NAME} src/main.c)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_NAME})

# Test executable
add_executable(${TEST_EXECUTABLE_NAME} tests/lexer.c)

target_link_libraries(${TEST_EXECUTABLE_NAME}
        PRIVATE
        ${LIBRARY_NAME}
        unity
)

add_test(NAME ${TEST_EXECUTABLE_NAME} COMMAND ${TEST_EXECUTABLE_NAME})
