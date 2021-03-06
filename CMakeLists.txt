cmake_minimum_required(VERSION 3.10)

# Project name and version
project(UpsideDownChessBot VERSION 0.1)

# specify the C++ standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
# Set optimization level of 3 and undefine `NDEBUG` as this can lead to
# "non-deterministically" behaviour (see manpage ASSERT(3)).
# (And it actually does break our program)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -UNDEBUG")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# directories
set(SRC_DIR src)
set(INC_DIR inc)
set(TEST_DIR test)
set(BENCHMARK_DIR benchmark)
set(TEST_INC_DIR test/inc)
set(DEVEL_INC_DIR devel_inc)

# sources
set(SOURCES ${SRC_DIR}/board.c ${SRC_DIR}/chess.c ${SRC_DIR}/bot.c
    ${SRC_DIR}/generator.c ${SRC_DIR}/move.c ${SRC_DIR}/timer.c
    ${SRC_DIR}/pst.c ${SRC_DIR}/hashtable.c ${SRC_DIR}/param_config.c)

# sources specific to the bot itself - cotaining bot main
set(BOT_SOURCES ${SRC_DIR}/main.c ${SOURCES})

# sources specific only to testing - containing test main
set(TEST_SOURCES ${TEST_DIR}/unity.c ${TEST_DIR}/unit_test.c
	${TEST_DIR}/test_generator.c ${TEST_DIR}/test_bot.c ${TEST_DIR}/test_types.c
	${TEST_DIR}/test_hashtable.c ${SOURCES})

# sources specific only to benchmarking - containing benchmark main
set(BENCHMARK_SOURCES ${BENCHMARK_DIR}/benchmark.c ${SOURCES})

# executables
add_executable(bot ${BOT_SOURCES})
add_executable(tests ${TEST_SOURCES})
add_executable(benchmark ${BENCHMARK_SOURCES})

# includes
target_include_directories(bot PRIVATE ${INC_DIR})
target_include_directories(tests PRIVATE ${DEVEL_INC_DIR} ${TEST_INC_DIR} ${INC_DIR})
target_include_directories(benchmark PRIVATE ${DEVEL_INC_DIR} ${INC_DIR})

# preprocessor variables
target_compile_definitions(tests PRIVATE UNITY_OUTPUT_COLOR)
target_compile_definitions(tests PRIVATE UNITY_INCLUDE_EXEC_TIME)

target_compile_definitions(tests PRIVATE TEST_SAMPLES_ITERATIONS=1)

# only use debug printing if it's a debug build
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(DEBUG_PRINTS)
    add_compile_definitions(DEBUG_BOARD_WHEN_PLAYING)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

add_compile_definitions(DEBUG_PRINT_STREAM=stderr)

# Optimizations toggles for benchmarking #######################################

# Choose exactly one of the three
#add_compile_definitions(VANILLA_MINIMAX)
#add_compile_definitions(ALPHA_BETA_CUTOFFS)
add_compile_definitions(PRINCIPAL_VARIATION_SEARCH)

add_compile_definitions(PIECE_SQUARE_TABLES)

add_compile_definitions(TRANSPOSITION_TABLES)

################################################################################

add_compile_definitions(TRANSPOSITION_TABLE_SIZE=2048) # use a power of two!

# tests
include(CTest)
add_test(NAME tests COMMAND tests)
