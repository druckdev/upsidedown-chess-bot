#ifndef GENERATOR_H
#define GENERATOR_H

#include "chess.h"
#include "types.h"

/**
 * Generates all moves possible in the current chess-game state.
 */
struct list* generate_moves(struct chess* game);


struct move_masks* init_move_masks();

#endif /* GENERATOR_H */
