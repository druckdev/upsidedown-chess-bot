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

int eg_knight_pst[64] = {
    0,-20,  0,0,0,  0,-20,0,
    0,  0,  0,0,0,  0,  0,0,
    0,  0,+30,0,0,+20,  0,0, // +30 as this move might lead to a quick checkmate
    0,  0,  0,0,0,  0,  0,0,
    0,  0,  0,0,0,  0,  0,0,
    0,  0,+20,0,0,+20,  0,0,
    0,  0,  0,0,0,  0,  0,0,
    0,-20,  0,0,0,  0,-20,0,
};

int eq_bishop_pst[64] = {
    0,0,-20,0,0,-20,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,-20,0,0,-20,0,0,
};

// The king is encouraged to get some space, since we
// don't need to worry about checks that much and
// it lessens the risk of a checkmate.
int eg_king_pst[64] = {
    -50, -30, -30, -30, -30, -30, -30, -50,
    -30, -30,  20,  20,  20,  20, -30, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -10,  30,   0,   0,  30, -10, -30,
    -30, -10,  30,   0,   0,  30, -10, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -30,  20,  20,  20,  20, -30, -30,
    -50, -30, -30, -30, -30, -30, -30, -50
}

// The queen is encouraged to move somewhere she's active.
// Hence corners and edges are ratet worse than more or less
// central fields
int eg_queen_pst[64] = {
    -30, -20, -10, -10, -10, -10, -20, -30,
    -20,   0,  20,  20,  20,  20,   0, -20,
    -10,  10,  20,  30,  30,  20,  10, -10,
    -10,  10,  30,  40,  40,  30,  10, -10,
    -10,  10,  30,  40,  40,  30,  10, -10,
    -10,  10,  20,  30,  30,  20,  10, -10,
    -20,   0,  20,  20,  20,  20,   0, -20,
    -30, -20, -10, -10, -10, -10, -20, -30 
}
#include "chess.h"
#include "board.h"

/**
 * Calculates the difference in value the piece square table suggests for a
 * given move.
 */
int get_pst_diff(struct PIECE* board, struct move* move);

#endif /* PST_H */
