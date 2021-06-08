#include <assert.h>

#include "pst.h"
#include "board.h"
#include "chess.h"

enum game_state { EARLY_GAME, MID_GAME, LATE_GAME };

enum game_state
get_game_state(struct chess* game)
{
	/*
	 * TODO(Aurel): How do we determine when the early/mid game ends? What is a
	 * good heuristic besides just move and piece count.
	 */
	return EARLY_GAME;
}

int
get_pst_diff(struct chess* game, struct move* move, enum PIECE_E piece_type)
{

	bool is_early_game = false,
	is_mid_game = false,
	is_late_game = false;

	// clang-format off
	switch (get_game_state(game)) {
	case EARLY_GAME: is_early_game = true; break;
	case MID_GAME: is_mid_game = true; break;
	case LATE_GAME: is_late_game = true; break;
	}
	// clang-format on

	if(is_early_game) {
		switch (piece_type) {
		case PAWN: return eg_pawn_pst[move->target] - eg_pawn_pst[move->start];
		case KNIGHT: return eg_knight_pst[move->target] - eg_knight_pst[move->start];
		case BISHOP: return eg_bishop_pst[move->target] - eg_bishop_pst[move->start];
		case KING: return eg_king_pst[move->target] - eg_king_pst[move->start];
		case QUEEN: return eg_queen_pst[move->target] - eg_queen_pst[move->start];
		case ROOK: return eg_rook_pst[move->target] - eg_rook_pst[move->start];
		default: assert((false, "invalid code path"));
		}
	}

#if 0
	if (is_mid_game) {
		switch (piece_type) {
		case PAWN: return mg_pawn_pst[move->target] - mg_pawn_pst[move->start];
		case KNIGHT: return mg_knight_pst[move->target] - mg_knight_pst[move->start];
		case BISHOP: return mg_bishop_pst[move->target] - mg_bishop_pst[move->start];
		case KING: return mg_king_pst[move->target] - mg_king_pst[move->start];
		case QUEEN: return mg_queen_pst[move->target] - mg_queen_pst[move->start];
		case ROOK: return mg_rook_pst[move->target] - mg_rook_pst[move->start];
		default: assert((false, "invalid code path"));
		}
	}

	if (is_late_game) {
		switch (piece_type) {
		case PAWN: return lg_pawn_pst[move->target] - lg_pawn_pst[move->start];
		case KNIGHT: return lg_knight_pst[move->target] - lg_knight_pst[move->start];
		case BISHOP: return lg_bishop_pst[move->target] - lg_bishop_pst[move->start];
		case KING: return lg_king_pst[move->target] - lg_king_pst[move->start];
		case QUEEN: return lg_queen_pst[move->target] - lg_queen_pst[move->start];
		case ROOK: return lg_rook_pst[move->target] - lg_rook_pst[move->start];
		default: assert((false, "invalid code path"));
		}
	}
#endif

	return 0;
}
