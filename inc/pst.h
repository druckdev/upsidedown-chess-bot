#ifndef PST_H
#define PST_H

#include "chess.h"
#include "board.h"

/**
 * Calculates the difference in value the piece square table suggests for a
 * given move.
 */
int get_pst_diff(struct chess* game, struct move* move, enum PIECE_E piece_type);

#endif /* PST_H */
