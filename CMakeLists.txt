cmake_minimum_required(VERSION 3.10)

# Project name and version
project(UpsideDownChessBot VERSION 0.1)

# specify the C++ standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)
# TODO(Aurel): Change this if you want optimizations.
set(CMAKE_BUILD_TYPE Debug)

# directories
set(SRC_DIR src)
set(INC_DIR inc)
set(TEST_DIR test)

# sources
set(SOURCES ${SRC_DIR}/board.c ${SRC_DIR}/chess.c
    ${SRC_DIR}/bot.c ${SRC_DIR}/generator.c ${SRC_DIR}/timemanagement.c
    ${SRC_DIR}/types.c)
set(HEADERS ${INC_DIR}/board.h ${INC_DIR}/chess.h
    ${INC_DIR}/bot.h ${INC_DIR}/generator.h ${INC_DIR}/timemanagement.h
    ${INC_DIR}/types.h)

# sources specific to the bot itself - cotaining bot main
set(BOT_SOURCES ${SRC_DIR}/main.c ${SOURCES})
set(BOT_HEADERS ${INC_DIR}/main.h ${HEADERS})

# sources specific only to testing - containing test main
set(TEST_SOURCES ${TEST_DIR}/unity.c ${TEST_DIR}/unit_test.c ${SOURCES})
set(TEST_HEADERS ${TEST_DIR}/unity.h ${TEST_DIR}/unity_internals.h ${HEADERS})

# sources specific only to benchmarking - containing benchmark main
set(BENCHMARK_SOURCES ${SRC_DIR}/benchmark.c ${SOURCES})
set(BENCHMARK_HEADERS ${HEADERS})

# executables
add_executable(bot ${BOT_SOURCES} ${BOT_HEADERS})
add_executable(test ${TEST_SOURCES} ${TEST_HEADERS})
add_executable(benchmark ${BENCHMARK_SOURCES} ${BENCHMARK_HEADERS})

# includes
target_include_directories(bot PRIVATE inc)
target_include_directories(test PRIVATE inc)
target_include_directories(benchmark PRIVATE inc)

# preprocessor variables
target_compile_definitions(test PRIVATE UNITY_OUTPUT_COLOR)
