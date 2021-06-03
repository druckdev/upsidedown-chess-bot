#ifndef PST_H
#define PST_H
/**
 * Piece square table definitions
 * NOTE(Aurel): Naming convention
 *	pst - piece square table
 *
 *	eq - early game
 *	mg - mid game
 *	lg - late game
 */

#if 0
int template_pst[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
#endif

int eg_pawn_pst[64] = {
	+20,+20,+20,+20,+20,+20,+20,+20,
    -20,-20,-20,-20,-20,-20,-20,-20,
      0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,
    -20,-20,-20,-20,-20,-20,-20,-20,
	+20,+20,+20,+20,+20,+20,+20,+20,
};

#include "chess.h"
#include "board.h"

/**
 * Calculates the difference in value the piece square table suggests for a
 * given move.
 */
int get_pst_diff(struct PIECE* board, struct move* move);

#endif /* PST_H */
