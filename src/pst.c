#include <assert.h>

#include "board.h"
#include "chess.h"
#include "pst.h"

/**
 * Piece square table definitions
 * NOTE(Aurel): Naming convention
 *	pst - piece square table
 *
 *	eg - early game
 *	mg - mid game
 *	lg - late game
 */

#if 0
// clang-format off
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
// clang-format on
#endif

// clang-format off
int eg_psts[6][64] = {
{ // pawn
	+20,+20,+20,+20,+20,+20,+20,+20,
    -20,-20,-20,-20,-20,-20,-20,-20,
      0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,
    -20,-20,-20,-20,-20,-20,-20,-20,
	+20,+20,+20,+20,+20,+20,+20,+20,
},
{ // bishop
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-20,-10,-10,-10,-10,-10,-10,-20,
},
{ // knight
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-30,  0, 15, 15, 15, 10,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 10, 15, 15, 15,  5,-30,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50,
},
{ // rook
	 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0,
},
// The king is encouraged to get some space, since we don't need to worry about
// checks that much and it lessens the risk of a checkmate.
{
    -50, -30, -30, -30, -30, -30, -30, -50,
    -30, -30,  20,  20,  20,  20, -30, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -10,  30,   0,   0,  30, -10, -30,
    -30, -10,  30,   0,   0,  30, -10, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -30,  20,  20,  20,  20, -30, -30,
    -50, -30, -30, -30, -30, -30, -30, -50
},
// The queen is encouraged to move somewhere she's active. Hence corners and
// edges are rated worse than more central fields
{
    -30, -20, -10, -10, -10, -10, -20, -30,
    -20,   0,  20,  20,  20,  20,   0, -20,
    -10,  10,  20,  30,  30,  20,  10, -10,
    -10,  10,  30,  40,  40,  30,  10, -10,
    -10,  10,  30,  40,  40,  30,  10, -10,
    -10,  10,  20,  30,  30,  20,  10, -10,
    -20,   0,  20,  20,  20,  20,   0, -20,
    -30, -20, -10, -10, -10, -10, -20, -30
},
};
// clang-format on

int
get_pst_diff(struct chess* game, struct move* move, enum piece_type piece_type)
{
	int* pst = NULL;

	// indexing into the pst's starts at 0
	piece_type--;

	// retrieve correct pst
	// clang-format off
#if 0
	switch (get_game_phase(game)) {
	case EARLY_GAME: pst = eg_psts[piece_type]; break;
	case MID_GAME: pst = mg_psts[piece_type]; break;
	case LATE_GAME: pst = lg_psts[piece_type]; break;
	}
#else
	// only the early game pst is actually done yet
	pst = eg_psts[piece_type];
#endif
	// clang-format on

	if (!pst) {
		return 0;
	}

	return pst[move->target] - pst[move->start];
}
