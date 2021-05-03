#ifndef GENERATOR_H
#define GENERATOR_H

#include "chess.h"
#include "types.h"

// Generate all legal moves for the piece at `pos`.
struct list* generate_moves_piece(struct PIECE board[], enum POS pos,
                                  bool check_checkless);

/**
 * Generates all moves possible in the current chess-game state.
 */
struct list* generate_moves(struct chess* game, bool check_checkless);

void test_move_generator();

#endif /* GENERATOR_H */
