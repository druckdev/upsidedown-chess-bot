#include <assert.h>

#include "pst.h"
#include "board.h"
#include "chess.h"

int
get_pst_diff(struct PIECE* board, struct move* move, enum PIECE_E piece_type)
{

	bool is_early_game = true;

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
	bool is_mid_game = true;
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

	bool is_late_game = true;
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
