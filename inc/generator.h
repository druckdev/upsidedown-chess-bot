#ifndef GENERATOR_H
#define GENERATOR_H

#include "chess.h"
#include "types.h"

/**
 * Generates all moves possible in the current chess-game state.
 */
struct list* generate_moves(struct chess* game, bool check_checkless,
                            bool king_moved);

#endif /* GENERATOR_H */
