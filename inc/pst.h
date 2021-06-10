#ifndef PST_H
#define PST_H

#include "board.h"
#include "chess.h"

/**
 * Calculates the difference in value the piece square table suggests for a
 * given move.
 */
int get_pst_val(struct chess* game, enum pos pos, enum piece_type piece_type);

#endif /* PST_H */
