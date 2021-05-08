#ifndef GENERATOR_H
#define GENERATOR_H

#include "chess.h"
#include "types.h"

// Generate all legal moves for the piece at `pos`.
struct list* generate_moves_piece(struct PIECE board[], enum POS pos,
                                  int check_checkless, bool hit_allies);

/**
 * Generates all moves possible in the current chess-game state.
 */
struct list* generate_moves(struct chess* game, int check_checkless,
                            bool hit_allies);

#ifdef BUILD_TEST
void test_generator();
#endif /* BUILD_TEST */

#ifdef BUILD_BENCHMARK
void benchmark_generator();
#endif /* BUILD_BENCHMARK */

#endif /* GENERATOR_H */
