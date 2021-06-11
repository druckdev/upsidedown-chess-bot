#ifndef GENERATOR_H
#define GENERATOR_H

#include "chess.h"
#include "move.h"

/**
 * Generates all moves possible in the current chess-game state.
 */
struct move_list* generate_moves(struct chess* game, bool check_checkless,
                                 bool hit_allies);

#endif /* GENERATOR_H */
