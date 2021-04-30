#ifndef GENERATOR_H
#define GENERATOR_H

#include "chess.h"
#include "types.h"

/**
 * Generates all moves possible in the current chess-game state.
 */
struct list* generate_moves(struct chess* game);

void test_move_generator();
void benchmark_move_generator();

#endif /* GENERATOR_H */
