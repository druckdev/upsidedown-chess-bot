#include <assert.h>

#include "pst.h"
#include "board.h"
#include "chess.h"

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
    0,0,-20,0,0,-20,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,-20,0,0,-20,0,0,
},
{ // knight
    0,-20,  0,0,0,  0,-20,0,
    0,  0,  0,0,0,  0,  0,0,
    0,  0,+30,0,0,+20,  0,0, // +30 as this move might lead to a quick checkmate
    0,  0,  0,0,0,  0,  0,0,
    0,  0,  0,0,0,  0,  0,0,
    0,  0,+20,0,0,+20,  0,0,
    0,  0,  0,0,0,  0,  0,0,
    0,-20,  0,0,0,  0,-20,0,
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

// The king is encouraged to get some space, since we
// don't need to worry about checks that much and
// it lessens the risk of a checkmate.
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
// The queen is encouraged to move somewhere she's active.
// Hence corners and edges are ratet worse than more or less
// central fields
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

// TODO(Aurel): Change these values. They have just been copied over from the
// early game.
int mg_psts[6][64] = {
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
    0,0,-20,0,0,-20,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,-20,0,0,-20,0,0,
},
{ // knight
    0,-20,  0,0,0,  0,-20,0,
    0,  0,  0,0,0,  0,  0,0,
    0,  0,+30,0,0,+20,  0,0, // +30 as this move might lead to a quick checkmate
    0,  0,  0,0,0,  0,  0,0,
    0,  0,  0,0,0,  0,  0,0,
    0,  0,+20,0,0,+20,  0,0,
    0,  0,  0,0,0,  0,  0,0,
    0,-20,  0,0,0,  0,-20,0,
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

// The king is encouraged to get some space, since we
// don't need to worry about checks that much and
// it lessens the risk of a checkmate.
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
// The queen is encouraged to move somewhere she's active.
// Hence corners and edges are ratet worse than more or less
// central fields
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

// TODO(Aurel): Change these values. They have just been copied over from the
// early game.
int lg_psts[6][64] = {
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
    0,0,-20,0,0,-20,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,  0,0,0,  0,0,0,
    0,0,-20,0,0,-20,0,0,
},
{ // knight
    0,-20,  0,0,0,  0,-20,0,
    0,  0,  0,0,0,  0,  0,0,
    0,  0,+30,0,0,+20,  0,0, // +30 as this move might lead to a quick checkmate
    0,  0,  0,0,0,  0,  0,0,
    0,  0,  0,0,0,  0,  0,0,
    0,  0,+20,0,0,+20,  0,0,
    0,  0,  0,0,0,  0,  0,0,
    0,-20,  0,0,0,  0,-20,0,
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

// The king is encouraged to get some space, since we
// don't need to worry about checks that much and
// it lessens the risk of a checkmate.
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
// The queen is encouraged to move somewhere she's active.
// Hence corners and edges are ratet worse than more or less
// central fields
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

enum game_phase { EARLY_GAME, MID_GAME, LATE_GAME };

#define EG_MOVE_COUNT_MAX 20
#define MG_MOVE_COUNT_MAX 40

#define EG_PIECE_COUNT_MIN 30
#define MG_PIECE_COUNT_MIN 11

enum game_phase
get_game_phase(struct chess* game)
{
	/*
	 * TODO(Aurel): How do we determine when the early/mid game ends? What is a
	 * good heuristic besides just move and piece count.
	 */

	enum game_phase phase = EARLY_GAME;

	if (game->move_count > EG_MOVE_COUNT_MAX ||
		game->piece_count < EG_PIECE_COUNT_MIN)
		phase = MID_GAME;

	if (game->move_count > MG_MOVE_COUNT_MAX ||
			game->piece_count < MG_PIECE_COUNT_MIN)
		phase = LATE_GAME;

	return phase;
}

int
get_pst_diff(struct chess* game, struct move* move, enum PIECE_E piece_type)
{

	int *pst = NULL;

	// retrieve correct pst
	// clang-format off
	switch (get_game_phase(game)) {
	case EARLY_GAME: pst = eg_psts[piece_type]; break;
	case MID_GAME: pst = mg_psts[piece_type]; break;
	case LATE_GAME: pst = lg_psts[piece_type]; break;
	}
	// clang-format on

	if (!pst) {
		return 0;
	}
	return pst[move->target] - pst[move->start];
}
