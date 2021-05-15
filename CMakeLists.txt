cmake_minimum_required(VERSION 3.10)

# Project name and version
project(UpsideDownChessBot VERSION 0.1)

# specify the C++ standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS_RELEASE "-O3")

# directories
set(SRC_DIR src)
set(INC_DIR inc)
set(TEST_DIR test)
set(TEST_INC_DIR test/inc)

# sources
set(SOURCES ${SRC_DIR}/board.c ${SRC_DIR}/chess.c
    ${SRC_DIR}/bot.c ${SRC_DIR}/generator.c ${SRC_DIR}/timemanagement.c
    ${SRC_DIR}/types.c)

# sources specific to the bot itself - cotaining bot main
set(BOT_SOURCES ${SRC_DIR}/main.c ${SOURCES})

# sources specific only to testing - containing test main
set(TEST_SOURCES ${TEST_DIR}/unity.c ${TEST_DIR}/unit_test.c
    ${TEST_DIR}/test_generator.c
    ${SOURCES})

# sources specific only to benchmarking - containing benchmark main
set(BENCHMARK_SOURCES ${SRC_DIR}/benchmark.c ${SOURCES})

# executables
add_executable(bot ${BOT_SOURCES})
add_executable(test ${TEST_SOURCES})
add_executable(benchmark ${BENCHMARK_SOURCES})

# includes
target_include_directories(bot PRIVATE ${INC_DIR})
target_include_directories(test PRIVATE ${TEST_INC_DIR} ${INC_DIR})
target_include_directories(benchmark PRIVATE ${INC_DIR})

# preprocessor variables
target_compile_definitions(test PRIVATE UNITY_OUTPUT_COLOR)
