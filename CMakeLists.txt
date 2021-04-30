cmake_minimum_required(VERSION 3.10)

# Project name and version
project(UpsideDownChessBot VERSION 0.1)

# specify the C++ standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# directories
set(SRC_DIR src)
set(INC_DIR inc)

# sources
set(SOURCES ${SRC_DIR}/main.c ${SRC_DIR}/board.c ${SRC_DIR}/chess.c
    ${SRC_DIR}/bot.c ${SRC_DIR}/generator.c ${SRC_DIR}/timemanagement.c
    ${SRC_DIR}/types.c)
set(HEADERS ${INC_DIR}/main.h ${INC_DIR}/board.h ${INC_DIR}/chess.h
    ${INC_DIR}/bot.h ${INC_DIR}/generator.h ${INC_DIR}/timemanagement.h
    ${INC_DIR}/types.h)

set(TEST_SOURCES ${SRC_DIR}/unity.c ${SRC_DIR}/unit_test.c 
    ${SRC_DIR}/board.c ${SRC_DIR}/chess.c ${SRC_DIR}/bot.c 
    ${SRC_DIR}/generator.c ${SRC_DIR}/timemanagement.c ${SRC_DIR}/types.c)
set(TEST_HEADERS ${INC_DIR}/unity.h ${INC_DIR}/unity_internals.h 
    ${INC_DIR}/board.h ${INC_DIR}/chess.h ${INC_DIR}/bot.h 
    ${INC_DIR}/generator.h ${INC_DIR}/timemanagement.h
    ${INC_DIR}/types.h)

# executables
add_executable(bot ${SOURCES} ${HEADERS})
add_executable(test ${TEST_SOURCES} ${TEST_HEADERS})

# includes
target_include_directories(bot PRIVATE inc)
target_include_directories(test PRIVATE inc)
